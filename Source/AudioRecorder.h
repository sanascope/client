/*
  ==============================================================================

   This file is part of the JUCE examples.
   Copyright (c) 2017 - ROLI Ltd.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Utils.h"
#include "FileUploader.h"

/*
class RecorderThread : public AudioIODeviceCallback , public Thread {
public:
	int foo;
	RecorderThread();
	~RecorderThread();
	bool startRecording();
	bool stopRecording();

private:
	int fileNumber;
	WavAudioFormat* wav;
	AudioFormatWriter* writer;
};
*/




class AudioRecorder : public AudioIODeviceCallback {
public:
	AudioRecorder() {
		backgroundThread.startThread();
	}

	~AudioRecorder() {
		stop();
	}

	//==============================================================================
	void startRecording(/*const File& file*/) {
		stop();

		if (sampleRate > 0) {
			// Create an OutputStream to write to our destination file...
			File docs = File::getSpecialLocation(File::userDocumentsDirectory);
			File sana = File(docs.getFullPathName() + File::getSeparatorChar() + "sanascope_public");
			if (!sana.exists()) { sana.createDirectory(); }
			File outputFile = File(sana.getFullPathName() + File::getSeparatorChar() + Utils::getTimeLabel() + ".wav");

			currentFileName = outputFile.getFullPathName();

			Utils::log("deleting file " + outputFile.getFullPathName());
			if (outputFile.exists()) { outputFile.deleteFile(); }

			if (auto fileStream = std::unique_ptr<FileOutputStream>(outputFile.createOutputStream())) {
				// Now create a WAV writer object that writes to our output stream...
				WavAudioFormat wavFormat;

				// setting WAF format with sample rate to 32kHz
				if (auto writer = wavFormat.createWriterFor(fileStream.get(), /*sampleRate*/ 32000.0 , 1, 16, {}, 0)) {
					fileStream.release(); // (passes responsibility for deleting the stream to the writer object that is now using it)

					// Now we'll create one of these helper objects which will act as a FIFO buffer, and will
					// write the data to disk on our background thread.
					threadedWriter.reset(new AudioFormatWriter::ThreadedWriter(writer, backgroundThread, 32768));

					// And now, swap over our active writer pointer so that the audio callback will start using it..
					const ScopedLock sl(writerLock);
					activeWriter = threadedWriter.get();
				}
			}
		} else {
		    Utils::log("No device sample rate defined!!!");
		}
	}

	void stop() {
		// First, clear this pointer to stop the audio callback from using our writer object..
		{
			const ScopedLock sl(writerLock);
			activeWriter = nullptr;
		}

		// Now we can delete the writer object. It's done in this order because the deletion could
		// take a little time while remaining data gets flushed to disk, so it's best to avoid blocking
		// the audio callback while this happens.
		Utils::log("resetting writer...");
		threadedWriter.reset();

        uploadAudioFile();
	}

	void uploadAudioFile() {
	    if(currentFileName.isEmpty())
            return;

        File file(currentFileName);
        FileUploader file_upload(file, "localhost");
        Utils::log("Ready to upload file: " + currentFileName);

        file_upload.startThread();
        Utils::log("Upload Thread started...");

        if(file_upload.waitForThreadToExit(1000000));
        Utils::log(file_upload.getResponseString());
	}

	bool isRecording() const {
		return activeWriter.load() != nullptr;
	}

	//==============================================================================
	void audioDeviceAboutToStart(AudioIODevice* device) override {
		sampleRate = device->getCurrentSampleRate();
		Utils::log("device sampling rate: " + std::to_string(sampleRate) + " --> hardcoding WAV sample rate to 32kHz...");
	}

	void audioDeviceStopped() override {
		sampleRate = 0;
	}

	void audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
		float** outputChannelData, int numOutputChannels,
		int numSamples) override {
		const ScopedLock sl(writerLock);

		if (activeWriter.load() != nullptr) {
			activeWriter.load()->write(inputChannelData, numSamples);

			// Create an AudioBuffer to wrap our incoming data, note that this does no allocations or copies, it simply references our input data
			AudioBuffer<float> buffer(const_cast<float**> (inputChannelData), numInputChannels, numSamples);
			nextSampleNum += numSamples;
		}

		// We need to clear the output buffers, in case they're full of junk..
		for (int i = 0; i < numOutputChannels; ++i)
			if (outputChannelData[i] != nullptr)
				FloatVectorOperations::clear(outputChannelData[i], numSamples);
	}

private:
	int fileNumber = 0;

	TimeSliceThread backgroundThread { "Audio Recorder Thread" }; // the thread that will write our audio data to disk
	std::unique_ptr<AudioFormatWriter::ThreadedWriter> threadedWriter; // the FIFO used to buffer the incoming data
	double sampleRate = 0.0;
	int64 nextSampleNum = 0;
    String currentFileName;

	CriticalSection writerLock;
	std::atomic<AudioFormatWriter::ThreadedWriter*> activeWriter { nullptr };


};
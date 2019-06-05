/*
  ==============================================================================

	RecorderThread.cpp
	Created: 22 May 2019 4:13:11pm
	Author:  alan

  ==============================================================================
*/

#include "AudioRecorder.h"

/*
RecorderThread::RecorderThread() : Thread("FileRecorderThread") {
	fileNumber = 0;

	Utils::log("creating audio format...");
	wav = new WavAudioFormat();
	writer = nullptr;
}

  RecorderThread::~RecorderThread() {
	  if (writer) { delete writer; }
	  delete wav;
  }

  bool RecorderThread::startRecording() {
	  Utils::log("getting documents directory...");
	  File docs = File::getSpecialLocation(File::userDocumentsDirectory);
	  File outputFile = File(docs.getFullPathName() + File::getSeparatorChar() + "test_"
		  + std::to_string(++fileNumber) + ".wav");

	  Utils::log("deleting file...");
	  if (outputFile.exists()) { outputFile.deleteFile(); }

	  Utils::log("(Re)Creating writer...");
	  FileOutputStream* fileWriterStream = outputFile.createOutputStream();
	  writer = wav->createWriterFor(fileWriterStream, 44100, 1, 16, NULL, 0);
	  return writer; // nullpointer check
  }

  bool RecorderThread::stopRecording() {
	  if (writer) { delete writer; }
	  writer = nullptr;
	  return !writer; // nullpointer check
  }
  */
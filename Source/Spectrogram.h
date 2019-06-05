/*
  ==============================================================================

	Spectrogram.h
	Created: 22 May 2019 2:10:42pm
	Author:  alan

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class Spectrogram : public AudioVisualiserComponent,
	public AudioIODeviceCallback {
public:
	Spectrogram();
	~Spectrogram();
	void audioDeviceAboutToStart(AudioIODevice *) override ;
	void audioDeviceStopped() override ;

	void paint(Graphics&) override;
	void resized() override;

private:
	enum {
		fftOrder = 12,
		fftSize = 1 << fftOrder
	};

	dsp::FFT forwardFFT;
	Image spectrogramImage;
	float fifo[fftSize];
	float fftData[2 * fftSize];
	int fifoIndex = 0;
	bool nextFFTBlockReady = false;

	void timerCallback() override;
	void pushNextSampleIntoFifo(float sample) noexcept;
	void drawNextLineOfSpectrogram();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Spectrogram)

		// Geerbt über AudioIODeviceCallback
		virtual void audioDeviceIOCallback(const float ** inputChannelData, int numInputChannels, float ** outputChannelData, int numOutputChannels, int numSamples) override;
};

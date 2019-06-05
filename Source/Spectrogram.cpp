/*
  ==============================================================================

	Spectrogram.cpp
	Created: 22 May 2019 2:10:42pm
	Author:  alan

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Spectrogram.h"

//==============================================================================

Spectrogram::Spectrogram() : AudioVisualiserComponent(1),
	forwardFFT(fftOrder),
	spectrogramImage(Image::RGB, 128, 512, true) {

	setSamplesPerBlock(256);
    setBufferSize(2048);
    setRepaintRate(30);
}

//==============================================================================
void Spectrogram::audioDeviceAboutToStart(AudioIODevice*) {
	clear();
}

void Spectrogram::audioDeviceStopped() {
	clear();
}

Spectrogram::~Spectrogram() {}

void Spectrogram::timerCallback() {
	if (nextFFTBlockReady) {
		drawNextLineOfSpectrogram();
		nextFFTBlockReady = false;
		repaint();
	}
}

void Spectrogram::pushNextSampleIntoFifo(float sample) noexcept {
	if (fifoIndex == fftSize)
	{
		if (!nextFFTBlockReady)
		{
			zeromem(fftData, sizeof(fftData));
			memcpy(fftData, fifo, sizeof(fifo));
			nextFFTBlockReady = true;
		}
		fifoIndex = 0;
	}
	fifo[fifoIndex++] = sample;
}

void Spectrogram::drawNextLineOfSpectrogram() {
	auto rightHandEdge = spectrogramImage.getWidth() - 1;
	auto imageHeight = spectrogramImage.getHeight();
	spectrogramImage.moveImageSection(0, 0, 1, 0, rightHandEdge, imageHeight);
	forwardFFT.performFrequencyOnlyForwardTransform(fftData);
	auto maxLevel = FloatVectorOperations::findMinAndMax(fftData, fftSize / 2);
	for (auto y = 1; y < imageHeight; ++y)
	{
		auto skewedProportionY = 1.0f - std::exp(std::log(y / (float) imageHeight) * 0.2f);
		auto fftDataIndex = jlimit(0, fftSize / 2, (int) (skewedProportionY * fftSize / 2));
		auto level = jmap(fftData[fftDataIndex], 0.0f, jmax(maxLevel.getEnd(), 1e-5f), 0.0f, 1.0f);
		spectrogramImage.setPixelAt(rightHandEdge, y, Colour::fromHSV(level, 1.0f, level, 1.0f));
	}
}

void Spectrogram::audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
	float** outputChannelData, int numOutputChannels,
	int numberOfSamples) {
	if (numInputChannels == 0) { return; }
	for (auto i = 0; i < numberOfSamples; ++i) {
		pushNextSampleIntoFifo((*inputChannelData)[i]);
	}
}

void Spectrogram::paint(Graphics& g) {
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));   // clear the background

	g.setOpacity(1.0f);
	g.drawImage(spectrogramImage, getLocalBounds().toFloat());
}

void Spectrogram::resized() {
	// This method is where you should set the bounds of any child
	// components that your component contains..

}

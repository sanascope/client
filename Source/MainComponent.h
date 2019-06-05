/*
  ==============================================================================

	This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Utils.h"
#include "AudioLiveScrollingDisplay.h"
#include "Spectrogram.h"
#include "AudioRecorder.h"

//==============================================================================
/*
	This component lives inside our window, and this is where you should put all
	your controls and content.
*/
class MainComponent : public AudioAppComponent,
	public ChangeListener {
public:

	//==============================================================================
	MainComponent();
	~MainComponent();

	//==============================================================================
	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
	void releaseResources() override;



	//==============================================================================
	void changeListenerCallback(ChangeBroadcaster*) override;

	//==============================================================================
	void paint(Graphics& g) override;
	void resized() override;

private:
	//==============================================================================

	enum AudioState { Stopped, Playing };

	void toggleRecording();
	void startRecording();
	void stopRecording();

	LiveScrollingAudioDisplay liveAudioScroller;
	Spectrogram* spectrogram;
	AudioRecorder* recorder;
	TextButton startStopButton;

	AudioState state;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

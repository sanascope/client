/*
  ==============================================================================

	This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================

MainComponent::~MainComponent() {
	// This shuts down the audio device and clears the audio source.
	deviceManager.removeChangeListener(this);
	deviceManager.removeAudioCallback(&liveAudioScroller);
	deviceManager.removeAudioCallback(spectrogram);
	deviceManager.removeAudioCallback(recorder);
	delete spectrogram;
	delete recorder;
	shutdownAudio();
}

MainComponent::MainComponent() {
	// Make sure you set the size of the component after
	// you add any child components.

	// UI
	setOpaque(true);
	addAndMakeVisible(&liveAudioScroller);
	addAndMakeVisible(startStopButton);
	startStopButton.setButtonText("Start");
	startStopButton.onClick = [&] { toggleRecording(); };
	spectrogram = new Spectrogram();
	addAndMakeVisible(spectrogram);
	recorder = new AudioRecorder();
	setSize(300, 450);

	// Logic
	state = AudioState::Stopped;

	deviceManager.addChangeListener(this); // detects Headphone events

	Utils::log("asking for permissions...");
	// Permissions
	if (RuntimePermissions::isRequired(RuntimePermissions::writeExternalStorage)
		&& !RuntimePermissions::isGranted(RuntimePermissions::writeExternalStorage)) {
		RuntimePermissions::request(RuntimePermissions::writeExternalStorage,
			[&] (bool granted) {});
	}

	if (RuntimePermissions::isRequired(RuntimePermissions::recordAudio)
		&& !RuntimePermissions::isGranted(RuntimePermissions::recordAudio)) {
		RuntimePermissions::request(RuntimePermissions::recordAudio,
		        [&] (bool granted) { setAudioChannels(1, 2); });
	} else {
	    setAudioChannels(1, 2);
	}

	Utils::log("adding audio callback...");
    deviceManager.addAudioCallback(&liveAudioScroller);
	deviceManager.addAudioCallback(spectrogram);
	deviceManager.addAudioCallback(recorder);
	Utils::log("constructor finished.");
}


/*
 * TODO Checkwhether headphones are connected
*/
void MainComponent::toggleRecording() {
	AudioIODevice* device = deviceManager.getCurrentAudioDevice();
	if (device) {
		Utils::log("Device name: " + device->getName());
		Utils::log("Device type: " + device->getTypeName());
	} else {
		Utils::log("No audio device found");
	}
	switch (state) {
		case MainComponent::Stopped:
			Utils::log("Starting Record...");
			startRecording();
			break;
		case MainComponent::Playing:
			Utils::log("Stopping Record...");
			stopRecording();
			break;
		default:
			break;
	}
}

void MainComponent::startRecording() {
	recorder->startRecording();
	state = Playing;
	startStopButton.setButtonText("Stop");
}

void MainComponent::stopRecording() {
	recorder->stop();
	state = Stopped;
	startStopButton.setButtonText("Start");
}


//==============================================================================

void MainComponent::changeListenerCallback(ChangeBroadcaster *) {
	Utils::log("Change detected");
}

//==============================================================================

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
	// This function will be called when the audio device is started, or when
	// its settings (i.e. sample rate, block size, etc) are changed.

	// You can use this function to initialise any resources you might need,
	// but be careful - it will be called on the audio thread, not the GUI thread.
	Utils::log("prepared to play");
}

void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) {
	//Utils::log("getting next audio block...");
	if (bufferToFill.buffer->getNumChannels() == 0) { return; }
	for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); channel++) {
		//Utils::log("inspecting channel nr " + std::to_string(channel) + "...");
		if (channel == 0) {
			switch (state) {
				case MainComponent::Stopped:
					// silence when inactive
					//Utils::log("clearing buffer...");
					bufferToFill.clearActiveBufferRegion();
					break;
				default:
					break;
			}
		} else {
			// distribute the input from source 0 over all output channels
			bufferToFill.buffer->copyFrom(
				channel, bufferToFill.startSample,                  // destination
				*bufferToFill.buffer, 0, bufferToFill.startSample,  // source
				bufferToFill.numSamples                             // number of samples
			);
		}
	}
	//Utils::log("Audio block got");
}

void MainComponent::releaseResources() {
	// This will be called when the audio device stops, or when it is being
	// restarted due to a setting change.

	// For more details, see the help for AudioProcessor::releaseResources()
}


//==============================================================================

void MainComponent::paint(Graphics& g) {
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void MainComponent::resized() {
	// This is called when the MainContentComponent is resized.
	// If you add any child components, this is where you should
	// update their positions.
	Utils::log("resizing...");
	startStopButton.setBounds(10, getHeight() - 10 - 40, getWidth() - 20, 40);
	liveAudioScroller.setBounds(10, 10, getWidth() - 20, 80);
	spectrogram->setBounds(10,
		20 + liveAudioScroller.getHeight(),
		getWidth() - 20,
		//getHeight() - liveAudioScroller.getHeight() - startStopButton.getHeight() - 40
		liveAudioScroller.getHeight());
	Utils::log("resized.");
}

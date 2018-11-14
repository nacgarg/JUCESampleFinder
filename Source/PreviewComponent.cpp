/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.1

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "PreviewComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
PreviewComponent::PreviewComponent (std::shared_ptr<Analysis> a)
    : state(Stopped)
{
    //[Constructor_pre] You can add your own custom stuff here..
	const MessageManagerLock mml;
	formatManager.registerBasicFormats();
	transportSource.addChangeListener(this);
	filename = a->filename;
    //[/Constructor_pre]


    //[UserPreSize]
	Image playImage = ImageCache::getFromMemory(BinaryData::play_icon_png, BinaryData::play_icon_pngSize);

	playButton.setImages(true, true, true,
		playImage, 0.9f, Colours::black,
		playImage, 1.0f, Colours::black,
		playImage, 0.9f, Colours::black, 0.0f);

	Image stopImage = ImageCache::getFromMemory(BinaryData::stop_icon_png, BinaryData::stop_icon_pngSize);

	stopButton.setImages(true, true, true,
		stopImage, 0.9f, Colours::black,
		stopImage, 1.0f, Colours::black,
		stopImage, 0.9f, Colours::black, 0.0f);
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
	// Load file from Analysis.filename
	auto file = File(a->filename);
	auto* reader = formatManager.createReaderFor(file);
	if (reader != nullptr)
	{
		std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true)); // [11]
		transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);                     // [12]
		playButton.setEnabled(true);                                                                    // [13]
		readerSource.reset(newSource.release());                                                        // [14]
	}
	Component::addAndMakeVisible(&playButton);
	playButton.setButtonText("Play");
	playButton.onClick = [this] {playButtonClicked(); };
	Component::addAndMakeVisible(&stopButton);
	stopButton.setButtonText("Stop");
	stopButton.onClick = [this] { stopButtonClicked(); };
    //[/Constructor]
}

PreviewComponent::~PreviewComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]



    //[Destructor]. You can add your own custom destruction code here..
	releaseResources();
    //[/Destructor]
}

//==============================================================================
void PreviewComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
	g.fillAll(Colours::azure);
	auto r = getLocalBounds();
	r.removeFromLeft(50);
	g.drawText(File(filename).getFileName(), r, Justification::centred);
    //[/UserPaint]
}

void PreviewComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
	auto r = getLocalBounds();
	stopButton.setBounds(r.removeFromLeft(20));
	playButton.setBounds(r.removeFromLeft(20));

    //[/UserResized]
}

void PreviewComponent::mouseDrag (const MouseEvent& e)
{
    //[UserCode_mouseDrag] -- Add your code here...
	DragAndDropContainer::performExternalDragDropOfFiles({ filename }, false);
    //[/UserCode_mouseDrag]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void PreviewComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) {
	if (readerSource.get() == nullptr)
	{
		bufferToFill.clearActiveBufferRegion();
		return;
	}
	transportSource.getNextAudioBlock(bufferToFill);
}

void PreviewComponent::prepareToPlay(int samplesExpected, double sampleRate) {
	transportSource.prepareToPlay(samplesExpected, sampleRate);

}

void PreviewComponent::releaseResources() {
	transportSource.releaseResources();
}

void PreviewComponent::changeListenerCallback(ChangeBroadcaster* source) {
	if (source == &transportSource)
	{
		if (transportSource.isPlaying())
			changeState(Playing);
		else
			changeState(Stopped);
	}
}

void PreviewComponent::playButtonClicked() {
	changeState(Starting);
}

void PreviewComponent::stopButtonClicked()
{
	changeState(Stopping);

}

void PreviewComponent::changeState(TransportState newState) {
	if (state != newState)
	{
		state = newState;
		std::cerr << "state " << state << std::endl;
		switch (state)
		{
		case Stopped:                           // [3]
			stopButton.setEnabled(false);
			playButton.setEnabled(true);
			transportSource.setPosition(0.0);
			break;
		case Starting:                          // [4]
			playButton.setEnabled(false);
			transportSource.start();
			break;
		case Playing:                           // [5]
			stopButton.setEnabled(true);
			break;
		case Stopping:                          // [6]
			transportSource.stop();
			break;
		}
	}
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PreviewComponent" componentName=""
                 parentClasses="public Component, public ChangeListener, public AudioSource"
                 constructorParams="std::shared_ptr&lt;Analysis&gt; a" variableInitialisers="state(Stopped)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.33"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <METHODS>
    <METHOD name="mouseDrag (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="ff323e44"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

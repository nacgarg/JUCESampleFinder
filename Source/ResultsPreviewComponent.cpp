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

#include "ResultsPreviewComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
ResultsPreviewComponent::ResultsPreviewComponent (std::vector<std::shared_ptr<Analysis>> result_analyses)
{
    //[Constructor_pre] You can add your own custom stuff here..
	const MessageManagerLock mml;
	for (auto a : result_analyses) {
		auto component = std::make_shared<PreviewComponent>(a);
		preview_components.push_back(component);
		addAndMakeVisible(component.get());
		mixer.addInputSource(component.get(), false);
	}
	setAudioChannels(0, 2);
    //[/Constructor_pre]


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

ResultsPreviewComponent::~ResultsPreviewComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]



    //[Destructor]. You can add your own custom destruction code here..
	shutdownAudio();
    //[/Destructor]
}

//==============================================================================
void ResultsPreviewComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
	g.drawText("hi", getLocalBounds(), Justification::centred);
    //[/UserPaint]
}

void ResultsPreviewComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
	auto r = getLocalBounds();
	for (std::shared_ptr<PreviewComponent> &a : preview_components) {
		a->setBounds(r.removeFromTop(25));
	}
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void ResultsPreviewComponent::prepareToPlay(int s, double sr) {
	mixer.prepareToPlay(s, sr);
}
void ResultsPreviewComponent::releaseResources() {
	mixer.releaseResources();
}
void ResultsPreviewComponent::getNextAudioBlock(const AudioSourceChannelInfo &bf) {
	mixer.getNextAudioBlock(bf);
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ResultsPreviewComponent"
                 componentName="" parentClasses="public AudioAppComponent" constructorParams="std::vector&lt;std::shared_ptr&lt;Analysis&gt;&gt; result_analyses"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.33" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

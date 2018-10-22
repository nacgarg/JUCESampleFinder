/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Sample.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public Component, public FileDragAndDropTarget {
 public:
  //==============================================================================
  MainComponent();
  ~MainComponent();
  void paint(Graphics&) override;
  void resized() override;

  //==============================================================================
  bool isInterestedInFileDrag(const StringArray& files);
  void filesDropped(const StringArray& files, int x, int y);

 private:
  //==============================================================================
  // Your private member variables go here...
  TextButton librarylocationButton;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SampleProcessor.h"
#include "ResultsPreviewComponent.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public Component, public FileDragAndDropTarget, public Button::Listener {
 public:
  //==============================================================================
  MainComponent();
  ~MainComponent();
  void paint(Graphics&) override;
  void resized() override;

  //==============================================================================
  bool isInterestedInFileDrag(const StringArray& files) override;
  void filesDropped(const StringArray& files, int x, int y) override;
  void fileDragEnter(const StringArray& files, int x, int y) override;

  void buttonClicked(Button*) override;

 private:
  //==============================================================================
  // Your private member variables go here...
  ImageButton* settingsButton;
  SampleProcessor sampleProcessor;
  ProgressBar* searchProgressBar;
  ResultsPreviewComponent* resultsComponent = nullptr;
  File libraryLocation;
  ApplicationProperties* appProps;
  PropertiesFile* propFile;

  double analysis_progress;
  static void searchCallback(std::vector<std::shared_ptr<Analysis>> results, void *this_pointer);
  void setLibraryLocation(File newLibraryLocation);
  const char* PROP_FILE_LIBRARY_LOCATION_KEY = "sampleLibraryLocation";
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include <fstream>
#include <iostream>
#include <iterator>

//==============================================================================
MainComponent::MainComponent() {
  setSize(300, 300);
  addAndMakeVisible(librarylocationButton);
}

MainComponent::~MainComponent() {}

//==============================================================================
void MainComponent::paint(Graphics& g) {
  // (Our component is opaque, so we must completely fill the background with a solid
  // colour)
  //   g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

  g.setFont(Font(16.0f));
  g.setColour(Colours::white);
  g.drawText("Drop file", getLocalBounds(), Justification::centred, true);
}

void MainComponent::resized() {
  // This is called when the MainComponent is resized.
  // If you add any child components, this is where you should
  // update their positions.
  librarylocationButton.setBounds(50, 50, getWidth() - 100, getHeight() - 100);
}

bool MainComponent::isInterestedInFileDrag(const StringArray& files) {
  // TODO:  Only return true if there's only one file and it's .wav
  return true;
}

void MainComponent::filesDropped(const StringArray& files, int x, int y) {
  // TODO Handle incoming file
  for (auto file : files) {
    std::cout << file;
  }
}

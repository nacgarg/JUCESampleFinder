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
    const std::string sampleLibraryLocation =
    "/Users/SamSmith/Desktop/test";  // For testing

    analyzeThread = new FileAnalyzer(sampleLibraryLocation);

    analyzeThread->run();
//    analyzeThread->waitForThreadToExit(10000);
}

MainComponent::~MainComponent() {
    delete analyzeThread;
}

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
  // sampleDropTarget.setBounds(50, 50, getWidth() - 100, getHeight() - 100);
}

bool MainComponent::isInterestedInFileDrag(const StringArray& files) {
  // TODO:  Only return true if there's only one file and it's .wav
    std::cout << "seems legit?" << std::endl;
  return files.size() == 1 && (files[0].endsWith(".wav") || files[0].endsWith(".mp3") || files[0].endsWith(".aif"));
}

void MainComponent::filesDropped(const StringArray& files, int x, int y) {
  // TODO Handle incoming file
    std::cout << "file dropped" << std::endl;
    for (int i= 0; i<files.size(); i++) {
        std::cout << "here's a file lmao" << files[i] << std::endl;
    
  }
    analyses = analyzeThread->analyses;
    std::cout << "enter the index of the one you want to search for: " << std::endl;
    File inputFile(files[0]);
    std::cout << files[0] << std::endl;
    auto s = new MonoSample(inputFile);
    std::shared_ptr<Analysis> blah = s->computeAnalysis();
    delete s;
    
    std::cout << "Finding sample most simliar to " << blah->filename << std::endl;
    Analysis::sortAnalyses(analyses, *blah, 10);
    for (int i = 0; i < 10; i++) {
        std::cout << "found " << analyses[i]->filename << " ranked " << i << std::endl;
    }
}
void MainComponent::fileDragEnter(const StringArray& files, int x, int y) {
    std::cout <<"reeee"<<std::endl;
}


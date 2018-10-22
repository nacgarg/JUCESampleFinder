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
  // Ideally, MonoSamples are only used when loading files and performing analyssi. They
  // produce a "comparison array" which is a compressed combination of FFT and Hilbert
  // data. This data is then used for comparison instead of continuing to save the entire
  // MonoSample to save memory.
  const std::string sampleLibraryLocation =
      "/run/media/nachi/KINGSTON/WAV/";  // For testing
  std::vector<std::shared_ptr<Analysis>> analyses;
  DirectoryIterator iter(File(sampleLibraryLocation), true, "*.wav");
  while (iter.next()) {
    File currFile(iter.getFile());
    std::cout << "Loading " << currFile.getFileName() << std::endl;
    auto s = new MonoSample(currFile);
    std::shared_ptr<Analysis> a = s->computeAnalysis();
    // for (int i = 0; i < 1000; i++) {
    analyses.push_back(a);
    // }
    delete s;
  }
  std::ofstream out_file("test.out");  // For debugging in gnuplot
  std::copy(analyses[2]->analysisData.begin(), analyses[2]->analysisData.end(),
            std::ostream_iterator<float>(out_file, "\n"));
  std::cout << "Loaded " << analyses.size() << " samples." << std::endl;
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
  // sampleDropTarget.setBounds(50, 50, getWidth() - 100, getHeight() - 100);
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

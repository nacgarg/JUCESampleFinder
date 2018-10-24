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
  // Ideally, MonoSamples are only used when loading files and performing analysis. They
  // produce an Analysis which is a compressed combination of FFT and Hilbert
  // data. This data is then used for comparison instead of continuing to save the entire
  // MonoSample to save memory.
  const std::string sampleLibraryLocation =
      "/home/nachi/code/JUCESampleFinder/test/";  // For testing
  std::vector<std::shared_ptr<Analysis>> analyses;
  DirectoryIterator iter(File(sampleLibraryLocation), true, "*.wav,*.aif,*.mp3");
  int i = 0;
  while (iter.next()) {
    std::cout << iter.getEstimatedProgress() << "\r";
    File currFile(iter.getFile());
    if (!currFile.existsAsFile()) continue;
    File analysisFile(currFile.getFullPathName() + ".saf");
    std::shared_ptr<Analysis> a;
    if (analysisFile.existsAsFile()) {
      // read analysis from file
      auto ifp = analysisFile.createInputStream();
      if (ifp->failedToOpen()) {
        // TODO actually have exceptions and stuff
        std::cerr << "Couldn't open analysis file for reading" << std::endl;
        continue;
      }
      a = Analysis::read(*ifp);

      delete ifp;
    } else {
      auto s = new MonoSample(currFile);
      a = s->computeAnalysis();
      auto ofp = analysisFile.createOutputStream();
      if (!ofp->openedOk()) {
        continue;
      }
      Analysis::serialize(*ofp, *a);
      delete ofp;
      delete s;
    }

    analyses.push_back(a);
    i++;
  }
  std::cout << "\n\n";

  // std::ofstream out_file("test.out");  // For debugging in gnuplot
  // std::copy(analyses[2]->analysisData.begin(), analyses[2]->analysisData.end(),
  //           std::ostream_iterator<float>(out_file, "\n"));
  int input;
  std::cout << "enter the index of the one you want to search for: " << std::endl;
  std::cin >> input;
  auto testAnalysis = analyses[input];
  std::cout << "Finding sample most simliar to " << testAnalysis->filename << std::endl;
  Analysis::sortAnalyses(analyses, *testAnalysis, 10);
  for (int i = 0; i < 10; i++) {
    std::cout << "found " << analyses[i]->filename << " ranked " << i << std::endl;
  }
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

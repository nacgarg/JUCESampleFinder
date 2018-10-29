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
    Image settingsImage = ImageCache::getFromMemory(BinaryData::settings_icon_png, BinaryData::settings_icon_pngSize);
    settingsButton = new ImageButton("settingsButton");
    settingsButton->setImages(true, true, true,
                              settingsImage, 0.9f, Colours::transparentBlack,
                              settingsImage, 1.0f, Colours::transparentBlack,
                              settingsImage, 0.5f, Colours::transparentBlack,
                              0.5f);
    settingsButton->setTooltip("Access SampleFinder settings");
//    settingsButton->setTopLeftPosition(10, 10);
    settingsButton->addListener(this);
    sampleProcessor.set_library_location({"/home/nachi/code/JUCESampleFinder/test"});
    setSize(300, 300);
    addAndMakeVisible(settingsButton);
    if (sampleProcessor.library_exists()) {
        sampleProcessor.analyze_files();
    }
}

MainComponent::~MainComponent() {
    delete settingsButton;
}

//==============================================================================
void MainComponent::paint(Graphics &g) {
    // (Our component is opaque, so we must completely fill the background with a solid
    // colour)
    g.fillAll(Colours::whitesmoke);

    g.setFont(Font(16.0f));
    g.setColour(Colours::black);
    g.drawText("Drop file", getLocalBounds(), Justification::centred, true);
}

void MainComponent::resized() {
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    settingsButton->setBounds(getWidth() - 20, getWidth() - 20, 20, 20);
}

bool MainComponent::isInterestedInFileDrag(const StringArray &files) {
    // TODO:  Only return true if there's only one file and it's .wav
    return files.size() == 1 && (files[0].endsWith(".wav") || files[0].endsWith(".mp3") ||
                                 files[0].endsWith(".aif"));
}

void MainComponent::searchCallback(std::vector<std::shared_ptr<Analysis>> results) {
    std::cout << "searchCallback" << std::endl;
    std::ostringstream str_results;
    str_results << "found similar: " << std::endl;
    for (const std::shared_ptr<Analysis> &a : results) {
        str_results << a->filename << std::endl;
    }
    AlertWindow::showMessageBox(AlertWindow::InfoIcon, "Results", str_results.str(), "cool");
}

void MainComponent::filesDropped(const StringArray &files, int x, int y) {
    // TODO Handle incoming file
    std::cout << "file dropped" << std::endl;
    sampleProcessor.find_similar(File(files[0]), 10, searchCallback);
}

void MainComponent::fileDragEnter(const StringArray &files, int x, int y) {
    std::cout << "enter" << std::endl;
}

void MainComponent::buttonClicked(Button *btn) {
    std::cout << "button clicked" << std::endl;
    if (btn == settingsButton) {
        std::cout << "Settings opened" << std::endl;
        FileChooser libraryChooser("Please select your sample folder",
                                   File::getSpecialLocation(File::userHomeDirectory),
                                   "*");
        if (libraryChooser.browseForDirectory()) {
            File libraryDir(libraryChooser.getResult());
            // TODO: Make it save settings in the PropertyFile thingy
            sampleProcessor.set_library_location({libraryDir});
            if (sampleProcessor.library_exists()) {
                sampleProcessor.analyze_files();
            }
        }
    }
}

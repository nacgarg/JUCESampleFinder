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
MainComponent::MainComponent() : appProps(new ApplicationProperties) {
    Image settingsImage = ImageCache::getFromMemory(BinaryData::settings_icon_png, BinaryData::settings_icon_pngSize);
    settingsButton = new ImageButton("settingsButton");
    settingsButton->setImages(true, true, true,
                              settingsImage, 0.9f, Colours::transparentBlack,
                              settingsImage, 1.0f, Colours::transparentBlack,
                              settingsImage, 0.5f, Colours::transparentBlack,
                              0.0f);
    settingsButton->setTooltip("Access SampleFinder settings");
    settingsButton->addListener(this);
    double& progress_ref = analysis_progress;
    searchProgressBar = new ProgressBar(progress_ref);
    setSize(300, 300);
    addAndMakeVisible(settingsButton);
    addAndMakeVisible(searchProgressBar);

	// load settings from properties file
	auto settingOptions = PropertiesFile::Options();
	settingOptions.applicationName = "SampleFinder";
	settingOptions.filenameSuffix = ".cfg";
	appProps->setStorageParameters(settingOptions);
	propFile = appProps->getUserSettings();
	setLibraryLocation(File(propFile->getValue(PROP_FILE_LIBRARY_LOCATION_KEY)));
}

MainComponent::~MainComponent() {
    delete settingsButton;
    delete searchProgressBar;
	delete resultsComponent;
	delete appProps;
}

//==============================================================================
void MainComponent::paint(Graphics &g) {
    // (Our component is opaque, so we must completely fill the background with a solid
    // colour)
    g.fillAll(Colours::whitesmoke);

    g.setFont(Font(16.0f));
    g.setColour(Colours::black);
    g.drawText("Drop file", getLocalBounds(), Justification::centred, true);
	if (resultsComponent) {
		g.drawText("Drop file", getLocalBounds(), Justification::centredBottom, true);
	}
}

void MainComponent::resized() {
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
	const MessageManagerLock mml;
    settingsButton->setBounds(getWidth() - 25, getWidth() - 25, 20, 20);
    searchProgressBar->setBounds(25, 200, getWidth() - 50, 20);
	auto r = getLocalBounds();
	r.removeFromBottom(20);
	if (resultsComponent) {
		resultsComponent->setBounds(r);
	}
}

bool MainComponent::isInterestedInFileDrag(const StringArray &files) {
    // TODO:  Only return true if there's only one file and it's .wav
    return files.size() == 1 && (files[0].endsWith(".wav") || files[0].endsWith(".mp3") ||
                                 files[0].endsWith(".aif"));
}

void MainComponent::searchCallback(std::vector<std::shared_ptr<Analysis>> results, void *this_pointer) {
	std::cout << "searchCallback" << std::endl;
    std::ostringstream str_results;
    str_results << "found similar: " << std::endl;
    for (const std::shared_ptr<Analysis> &a : results) {
        str_results << a->filename << std::endl;
    }
	MainComponent * self = static_cast<MainComponent*>(this_pointer);
	{
		const MessageManagerLock mml;
		delete self->resultsComponent;
	}
	self->resultsComponent = new ResultsPreviewComponent(results);
	{
		const MessageManagerLock mml;
		self->addAndMakeVisible(self->resultsComponent);
		self->addAndMakeVisible(self->settingsButton);
	}
	self->resized();
    //AlertWindow::showMessageBox(AlertWindow::InfoIcon, "Results", str_results.str(), "cool");
}

void MainComponent::filesDropped(const StringArray &files, int x, int y) {
    // TODO Handle incoming file
    std::cout << "file dropped" << std::endl;
    sampleProcessor.find_similar(File(files[0]), 10, searchCallback, this);
}

void MainComponent::fileDragEnter(const StringArray &files, int x, int y) {
    std::cout << "enter" << std::endl;
}

void MainComponent::buttonClicked(Button *btn) {
    std::cout << "button clicked" << std::endl;
    if (btn == settingsButton) {
        std::cout << "Settings opened" << std::endl;
		File searchDir = File::getSpecialLocation(File::userHomeDirectory);
		if (libraryLocation.exists()) {
			searchDir = libraryLocation;
		}
        FileChooser libraryChooser("Please select your sample folder",
                                  searchDir,
                                   "*");
		
        if (libraryChooser.browseForDirectory()) {
            File libraryDir(libraryChooser.getResult());
            // TODO: Allow multiple library folders
			setLibraryLocation(libraryDir);
        }
    }
}

void MainComponent::setLibraryLocation(File newLibraryLocation) {
	sampleProcessor.set_library_location({ newLibraryLocation });
	libraryLocation = newLibraryLocation;
	delete resultsComponent;
	if (sampleProcessor.library_exists()) {
		// set propfile value
		propFile->setValue(PROP_FILE_LIBRARY_LOCATION_KEY, libraryLocation.getFullPathName());
		double& progress_ref = analysis_progress;
		sampleProcessor.analyze_files(progress_ref);
	}
	else {
		AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Error", "Could not find sample library at " + sampleProcessor.get_library_location()[0].getFullPathName() + "\nPlease set the library location and try again.");
	}
}

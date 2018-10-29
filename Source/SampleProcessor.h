/*
  ==============================================================================

    SampleProcessor.h
    Created: 26 Oct 2018 9:38:57pm
    Author:  nachi

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Sample.h"
#include <functional>

class FileAnalyzer : public Thread {
public:
    FileAnalyzer(std::vector<File> dirs,
                 std::vector<std::shared_ptr<Analysis>> *analysis_in, double& p);

    void run() override;

    double &get_progress();

private:
    std::vector<File> library_location;
    std::vector<std::shared_ptr<Analysis>> *analysis_data;
    double &progress;
};

class FileSearcher : public Thread {
public:
    std::vector<std::shared_ptr<Analysis>> results;

    FileSearcher(std::vector<std::shared_ptr<Analysis>> *analysis_in, File similar_to,
                 int num);

    void run() override;

private:
    File similar_to;
    std::vector<std::shared_ptr<Analysis>> *analysis_data;
    int num_results;
};

// SampleProcessor is the class that takes care of all the threading and processing of
// audio samples. MainComponent will interface with this class to search files and get
// results.

class SampleProcessor : public Thread::Listener {
public:
    // CONSTRUCTOR
    SampleProcessor();
    // Analyze sample_libraries

    ~SampleProcessor();
    // Shut down threads and clean up stuff here

    // ================

    // SAMPLE LIBRARY
    std::vector<File> get_library_location();

    void set_library_location(std::vector<std::string>);

    void set_library_location(std::vector<File>);

    void add_library_location(File &in);

    bool library_exists();
    // ================

    // PROCESSING
    void analyze_files(double& progress);

    double &get_search_progress();

    // Start a thread to analyze the files in library_location. If the file is innacessible
    // for any reason, return false. Create .saf data files if they don't already exist.
    void find_similar(File similar_to, int num_results,
                      std::function<void(std::vector<std::shared_ptr<Analysis>>)> callback);

    // Start a thread to search through analysis_data to find the top  num_results similar
    // files.
    void exitSignalSent()
    override;

private:
    std::vector<File> library_location;

    FileAnalyzer *analysisThread = nullptr;
    FileSearcher *searchThread = nullptr;

    std::vector<std::shared_ptr<Analysis>> analysis_data;
    bool analyzed;
    std::function<void(std::vector<std::shared_ptr<Analysis>>)> searchCallback;
};

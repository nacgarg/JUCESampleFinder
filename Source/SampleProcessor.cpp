/*
  ==============================================================================

    SampleProcessor.cpp
    Created: 26 Oct 2018 9:38:57pm
    Author:  Nachiketa Gargi

  ==============================================================================
*/

#include "SampleProcessor.h"

SampleProcessor::SampleProcessor() { std::cout << "Hi" << std::endl; }

SampleProcessor::~SampleProcessor() { std::cout << "Bye" << std::endl; }

std::vector<File> SampleProcessor::get_library_location() {
  return this->library_location;
}

void SampleProcessor::set_library_location(std::vector<std::string> paths_str) {
  this->library_location.clear();
  for (const auto &path_str : paths_str) {
    library_location.emplace_back(File(path_str));
  }
}

void SampleProcessor::set_library_location(std::vector<File> in) {
  this->library_location = in;
}

void SampleProcessor::add_library_location(File& in) {
    this->library_location.push_back(in);
}

bool SampleProcessor::library_exists() {
  if (library_location.empty()) return false;
  bool allExist = true;
  for (const File& f : library_location) {
    if (not f.isDirectory()) allExist = false;
  }
  return allExist;
};

void SampleProcessor::analyze_files() {
  analysisThread = new FileAnalyzer(library_location, &analysis_data);
  analysisThread->addListener(this);
  analysisThread->run();
  delete analysisThread;
}

void SampleProcessor::exitSignalSent() { std::cout << "exitSignalSent!" << std::endl; }

std::vector<std::shared_ptr<Analysis>> SampleProcessor::find_similar(File similar_to,
                                                                     int num_results) {
  searchThread = new FileSearcher(&analysis_data, similar_to, num_results);
  searchThread->run();
  auto res = searchThread->results;
  delete searchThread;
  return res;
}

FileAnalyzer::FileAnalyzer(std::vector<File> dirs,
                           std::vector<std::shared_ptr<Analysis>>* analysis_in)
    : Thread("FileAnalyzer"), library_location(dirs), analysis_data(analysis_in) {}

void FileAnalyzer::run() {
  // Ideally, MonoSamples are only used when loading files and performing analysis.
  // They produce an Analysis which is a compressed combination of FFT and Envelope
  // data. This data is then used for comparison instead of continuing to save the
  // entire MonoSample to save memory.
  for (const File dir : library_location) {
    DirectoryIterator iter(dir, true, "*.wav,*.aif,*.mp3");
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
        MonoSample s(currFile);
        a = s.computeAnalysis();
        auto ofp = analysisFile.createOutputStream();
        if (!ofp->openedOk()) {
          std::cerr << "Couldn't open analysis file for writing" << std::endl;
          continue;
        }
        Analysis::serialize(*ofp, *a);
        delete ofp;
      }

      analysis_data->push_back(a);
    }
  }
}

FileSearcher::FileSearcher(std::vector<std::shared_ptr<Analysis>>* analysis_in,
                           File similar, int num_in)
    : Thread("FileSearcher"),
      analysis_data(analysis_in),
      num_results(num_in),
      similar_to(similar) {}

void FileSearcher::run() {
  auto s = new MonoSample(similar_to);
  std::shared_ptr<Analysis> similarAnalysis = s->computeAnalysis();
  delete s;

  std::cout << "Finding sample most simliar to " << similarAnalysis->filename
            << std::endl;
  Analysis::sortAnalyses(*analysis_data, *similarAnalysis, num_results);
  for (int i = 0; i < num_results; i++) {
    std::cout << "found " << analysis_data->at(i)->filename << " ranked " << i
              << std::endl;
  }
  results = std::vector<std::shared_ptr<Analysis>>(analysis_data->begin(),
                                                   analysis_data->begin() + num_results);
}
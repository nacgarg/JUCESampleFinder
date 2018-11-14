/*
  ==============================================================================

    SampleProcessor.cpp
    Created: 26 Oct 2018 9:38:57pm
    Author:  Nachiketa Gargi

  ==============================================================================
*/

#include "SampleProcessor.h"

SampleProcessor::SampleProcessor() { std::cout << "Hi" << std::endl; }

SampleProcessor::~SampleProcessor() { std::cout << "Bye" << std::endl; delete analysisThread; delete searchThread; }

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

void SampleProcessor::add_library_location(File &in) {
    this->library_location.push_back(in);
}

bool SampleProcessor::library_exists() {
    if (library_location.empty()) return false;
    bool allExist = true;
    for (const File &f : library_location) {
        if (!f.isDirectory()) allExist = false;
    }
    return allExist;
};

void SampleProcessor::analyze_files(double& progress) {
    delete analysisThread;
    analyzed = false;
    analysisThread = new FileAnalyzer(library_location, &analysis_data, progress);
    analysisThread->addListener(this);
    analysisThread->startThread();
	std::cout << "started analysis thread" << std::endl;
}

void SampleProcessor::exitSignalSent() {
    std::cout << "exitSignalSent from " << Thread::getCurrentThread()->getThreadName() << std::endl;
    if (Thread::getCurrentThread()->getThreadId() == analysisThread->getThreadId()) {
        // analysis complete!
        analyzed = true;
		//analysis_data = analysisThread->analysis_data;
    } else if (Thread::getCurrentThread()->getThreadId() == searchThread->getThreadId()) {
        // search complete!
        std::cout << "Calling callback now" << std::endl;
		searchCallback(searchThread->results, callee);
    }
}

void SampleProcessor::find_similar(File similar_to,
                                   int num_results,
                                   std::function<void(std::vector<std::shared_ptr<Analysis>>, void *this_pointer)> callback, void* caller) {
    if (!analyzed) {
        analysisThread->waitForThreadToExit(600000);
    }
    delete searchThread;
    searchCallback = callback;
	callee = caller;
    searchThread = new FileSearcher(&analysis_data, similar_to, num_results);
    searchThread->addListener(this);
    searchThread->startThread();
}

double &SampleProcessor::get_search_progress() {
    return analysisThread->get_progress();
}

FileAnalyzer::FileAnalyzer(std::vector<File> dirs,
                           std::vector<std::shared_ptr<Analysis>> *analysis_in, double& p)
        : Thread("FileAnalyzer"), progress(p), library_location(std::move(dirs)), analysis_data(analysis_in) {}

void FileAnalyzer::run() {
	std::cout << "Running" << std::endl;
    // Ideally, MonoSamples are only used when loading files and performing analysis.
    // They produce an Analysis which is a compressed combination of FFT and Envelope
    // data. This data is then used for comparison instead of continuing to save the
    // entire MonoSample to save memory.
    for (const File &dir : library_location) {
        DirectoryIterator iter(dir, true, "*.wav,*.aif,*.mp3");
        while (iter.next()) {
			std::cout << iter.getEstimatedProgress() << std::endl;
            progress = iter.getEstimatedProgress();
            File* currFile = new File(iter.getFile());
            if (!currFile->existsAsFile()) continue;

            File* analysisFile = new File(currFile->getFullPathName() + ".saf");
            std::shared_ptr<Analysis> a;
			if (analysisFile->existsAsFile()) {
				// read analysis from file
				FileInputStream ifp(*analysisFile);
				if (ifp.failedToOpen()) {
					// TODO actually have exceptions and stuff
					std::cerr << "Couldn't open analysis file for reading" << std::endl;
					AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "File I/O Error", "Couldn't read file", "rip");
					continue;
				}
				a = Analysis::read(ifp);
				if (!a) {
					AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Analysis File Error", "File is incorrectly formatted", "rip");
				}
				else {
					analysis_data->push_back(a);
					continue;
				}
			}
			auto s = new MonoSample(*currFile);
			a = s->computeAnalysis();
			delete s;
			auto ofp = analysisFile->createOutputStream();

			if (!ofp || ofp->failedToOpen()) {
				std::cerr << "Couldn't open analysis file for writing" << std::endl;
				delete analysisFile;
				AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "File I/O Error", "Couldn't open file for writing", "rip");
				continue;
			}
			Analysis::serialize(*ofp, *a);
			delete analysisFile;
			if (a) {
				analysis_data->push_back(a);
			}
        }
    }
    std::cout << "Finished analysis" << std::endl;
    signalThreadShouldExit();
}

double &FileAnalyzer::get_progress() {
    return progress;
}

FileSearcher::FileSearcher(std::vector<std::shared_ptr<Analysis>> *analysis_in,
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
    signalThreadShouldExit();
}
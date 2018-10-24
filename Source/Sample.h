/*
  ==============================================================================

    Sample.h
    Created: 18 Oct 2018 6:15:07pm
    Author:  Nachiketa Gargi

  ==============================================================================
*/
#include <math.h>
#include <algorithm>
#include <tuple>
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"

#pragma once

static const int MAX_LENGTH = 1 << 16;
static const int FFT_ORDER = log2(MAX_LENGTH);
static const int SAMPLE_RATE = 44100;
static const int ANALYSIS_DATA_LENGTH = MAX_LENGTH / 4;

struct Analysis {
  // Simplified struct to only hold minimal data in memory
  std::string filename;
  std::array<float, ANALYSIS_DATA_LENGTH> analysisData;
  // If you need to get the =actual audio data, you can eventually use the
  // MonoSample(Analysis) constructor once I make it
  static const float compareAnalyses(Analysis& a, Analysis& b) {
    float currScore = 0;
    for (int i = 0; i < ANALYSIS_DATA_LENGTH; i++) {
      if (!(a.analysisData[i] == a.analysisData[i] &&
            b.analysisData[i] == b.analysisData[i])) {
        // std::cout << a.analysisData[i] << " - " << b.analysisData[i] << std::endl;
        // std::cout << a.analysisData.size() << " " << b.analysisData.size();
        continue;
      }

      currScore += (a.analysisData.at(i) - b.analysisData.at(i)) *
                   (a.analysisData.at(i) - b.analysisData.at(i));
    }
    return currScore / ANALYSIS_DATA_LENGTH;
  }

  static void sortAnalyses(std::vector<std::shared_ptr<Analysis>>& arr,
                           Analysis& reference, int num) {
    std::vector<std::tuple<float, std::shared_ptr<Analysis>>> scores;
    for (auto el : arr) {
      scores.push_back(std::make_tuple(compareAnalyses(*el, reference), el));
    }
    std::partial_sort(scores.begin(), scores.begin() + num, scores.end());
    for (size_t i = 0; i < arr.size(); i++) {
      arr[i] = std::get<1>(scores[i]);
    }
  }

  static void serialize(OutputStream& os, Analysis& a) {
    auto* gzip = new GZIPCompressorOutputStream(os, 2);
    gzip->writeString(a.filename);
    gzip->write(reinterpret_cast<const char*>(a.analysisData.data()),
                ANALYSIS_DATA_LENGTH * sizeof(float));
    gzip->flush();
    delete gzip;
  }

  static std::shared_ptr<Analysis> read(InputStream& is) {
    auto* gzip = new GZIPDecompressorInputStream(is);
    std::string filename;
    filename = gzip->readString().toStdString();
    Analysis tmp = {filename};
    gzip->read(reinterpret_cast<char*>(tmp.analysisData.data()),
               ANALYSIS_DATA_LENGTH * sizeof(float));
    delete gzip;
    return std::make_shared<Analysis>(tmp);
  }
};

class MonoSample {
 public:
  MonoSample() {}
  MonoSample(File& fileToLoad) : MonoSample() { loadFile(fileToLoad); }

  ~MonoSample() {}

  int getLengthSamples() const;
  int getLengthSeconds() const;
  int getSampleRate() const;

  void loadFile(File fileToLoad) {
    formatManager.registerBasicFormats();
    auto* reader = formatManager.createReaderFor(fileToLoad);
    if (reader == nullptr) {
      return;  // bad file or something
    }
    auto fileBuffer = new AudioBuffer<float>(2, reader->lengthInSamples);
    reader->read(fileBuffer, 0, fileBuffer->getNumSamples(), static_cast<juce::int64>(0),
                 true, false);

    int sampleRate = reader->sampleRate;
    filename = fileToLoad.getFileName().toStdString();
    std::vector<float> temp;
    temp.clear();
    int lengthToRead = MAX_LENGTH;
    if (fileBuffer->getNumSamples() < lengthToRead) {
      lengthToRead = fileBuffer->getNumSamples();
    }
    if (fileBuffer->getNumChannels() == 1) {
      auto* channelDataLeft = fileBuffer->getReadPointer(0);

      for (int i = 0; i < lengthToRead; i++) {
        float mono = channelDataLeft[i];
        temp.push_back(mono);
      }
    } else if (fileBuffer->getNumChannels() == 2) {
      auto* channelDataLeft = fileBuffer->getReadPointer(0);
      auto* channelDataRight = fileBuffer->getReadPointer(1);

      for (int i = 0; i < lengthToRead; i++) {
        float mono = (channelDataLeft[i] + channelDataRight[i]) / 2.0;
        temp.push_back(mono);
      }
    }

    setSamples(temp, sampleRate);
    delete reader;
    delete fileBuffer;
  }
  void setSamples(float* samples, int numSamples, int sampleRate) {
    // convert float array to std::vector and call the other setSamples
    std::vector<float> v(samples, samples + numSamples);
    setSamples(v, sampleRate);
  }

  void setSamples(std::array<float, MAX_LENGTH> samples, int sampleRate) {
    // convert float array to std::vector and call the other setSamples
    std::vector<float> v(samples.begin(), samples.end());
    setSamples(v, sampleRate);
  }

  void setSamples(std::vector<float> samples, int sampleRateIn) {
    if (sampleRateIn != SAMPLE_RATE) {
      // Resample using Lagrange interpolation into SAMPLE_RATE
      // MonoSample::interpolator->reset();
      std::array<float, MAX_LENGTH> resampled;
      MonoSample::interpolator->process(
          static_cast<double>(sampleRateIn) / static_cast<double>(SAMPLE_RATE),
          samples.data(), resampled.begin(), samples.size());
      setSamples(resampled, SAMPLE_RATE);
    }
    sampleRate = sampleRateIn;
    // Ensure that we only copy the data within MAX_LENGTH
    for (int i = 0; i < MAX_LENGTH; i++) {
      if (i < samples.size()) {
        data[i] = samples[i];
      } else {
        data[i] = 0;
      }
    }
  }

  std::string getFilename() const { return this->filename; }

  std::shared_ptr<Analysis> computeAnalysis() {
    // Use a combination of various DSP methods to generate data that encapsulates the
    // gist of a sample that can be compared with simple MSE

    // We take a limited logarithmic sampling of an FFT
    // which reduces the memory needed by more than half (?)

    // We downsample the envelope to fit in the remaning space, because temporal volume
    // envelopes don't really need much precision

    computeFFT();
    computeEnvelope();

    const int fftLength = MAX_LENGTH * 3 / 16;
    // FFT is squashed into 3/4 the space of the Analysis struct.
    std::vector<float> squashedFFT;
    for (double i = 0; i < fftLength; i += 1) {
      int adjustedIndex = static_cast<int>(pow(MAX_LENGTH / 2, i / (fftLength)));
      // TODO ignore frequencies from 0-20hz, because right now they take like half the
      // data and they're useless
      squashedFFT.push_back(fftData[adjustedIndex] / 1500);
    }

    // Envelope is downsampled to 1/4 its original size
    std::vector<float> squashedEnvelope;
    for (int i = 0; i < MAX_LENGTH * 2; i += 32) {
      squashedEnvelope.push_back(envelopeData[i]);
    }
    squashedFFT.insert(squashedFFT.end(), squashedEnvelope.begin(),
                       squashedEnvelope.end());

    Analysis a = {getFilename(), std::array<float, MAX_LENGTH / 4>()};
    std::copy_n(squashedFFT.begin(), MAX_LENGTH / 4, a.analysisData.begin());

    return std::make_shared<Analysis>(a);
  }

 private:
  void computeFFT() {
    // This function computes the FFT and puts it in fftData
    if (fftExists) {
      // If we've already computed it
      return;
    }
    std::copy(data.begin(), data.end(), fftData.begin());
    forwardFFT->performFrequencyOnlyForwardTransform(fftData.begin());
    fftExists = true;
  }

  void computeEnvelope() {
    float m_a = pow(0.01, 1.0 / (50 * SAMPLE_RATE * 0.001));
    float m_r = pow(0.01, 1.0 / (200 * SAMPLE_RATE * 0.001));

    // std::copy(data.begin(), data.end(), envelopeData.begin());

    for (int i = 0; i < MAX_LENGTH; ++i) {
      if (envelopeData[i - 1] > data[i]) {
        envelopeData[i] = (1 - m_r) * std::abs(data[i]) + m_r * envelopeData[i - 1];
      } else {
        envelopeData[i] = (1 - m_a) * std::abs(data[i]) + m_a * envelopeData[i - 1];
      }
    }

    envelopeExists = true;
  }
  const std::unique_ptr<dsp::FFT> forwardFFT =
      std::unique_ptr<dsp::FFT>(new dsp::FFT(FFT_ORDER));
  bool fftExists = false;
  bool envelopeExists = false;
  int sampleRate;
  int length;
  std::array<float, MAX_LENGTH> data;
  const std::unique_ptr<LagrangeInterpolator> interpolator =
      std::unique_ptr<LagrangeInterpolator>(new LagrangeInterpolator());
  std::array<float, MAX_LENGTH * 2> fftData;
  std::array<float, MAX_LENGTH * 2> envelopeData;
  std::string filename;
  AudioFormatManager formatManager;
};

/*
  ==============================================================================

    Sample.h
    Created: 18 Oct 2018 6:15:07pm
    Author:  Nachiketa Gargi

  ==============================================================================
*/
#include <math.h>
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"

#pragma once

static const int MAX_LENGTH = 1 << 16;
static const int FFT_ORDER = log2(MAX_LENGTH);
static const int SAMPLE_RATE = 44100;

struct Analysis {
  // Simplified struct to only hold minimal data in memory
  std::string filename;
  std::array<float, MAX_LENGTH / 4> analysisData;
  // If you need to get the actual audio data, you can eventually use the
  // MonoSample(Analysis) constructor once I make it
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
    reader->read(fileBuffer, 0, MAX_LENGTH, static_cast<juce::int64>(0), true, false);
    // TODO see what happens if you use a mono source file
    auto* channelDataLeft = fileBuffer->getReadPointer(0);
    auto* channelDataRight = fileBuffer->getReadPointer(1);
    std::cout << "Reading file with " << fileBuffer->getNumSamples() << " samples"
              << std::endl;
    int sampleRate = reader->sampleRate;
    filename = fileToLoad.getFileName().toStdString();
    std::vector<float> temp;
    temp.clear();
    for (int i = 0; i < MAX_LENGTH; i++) {
      float mono = (channelDataLeft[i] + channelDataRight[i]) / 2.0;
      temp.push_back(mono);
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
    std::cout << "Received file with sampleRate " << sampleRateIn << std::endl;
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
    std::cout << "original size" << MAX_LENGTH * 2 << " "
              << "compressed size: " << squashedFFT.size() << std::endl;

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
    std::cout << m_a << " - " << m_r << std::endl;

    std::copy(data.begin(), data.end(), envelopeData.begin());

    for (int i = 0; i < MAX_LENGTH; ++i) {
      if (envelopeData[i - 1] > data[i]) {
        envelopeData[i] =
            (1 - m_r) * std::abs(data[i]) + m_r * envelopeData[i - 1];
      } else {
        envelopeData[i] =
            (1 - m_a) * std::abs(data[i]) + m_a * envelopeData[i - 1]; 
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
# SampleFinder

This is yet another iteration of the samplefinder program I previously made in Python. This version is intended to be a standalone crossplatform GUI application written in JUCE that enables sound-similarity-based sample sorting for samples shorter than a couple seconds which could be useful for music producers such as myself. After a somewhat lengthy analysis process, the program creates "sample analysis files" with the extension `.saf` with the same filenames as the samples (similar to `.asd` files that Ableton creates) which it can load at a later time for analysis. Currently, it stores all analyses in memory, but with large sample libraries it is technically possible to load the analyses at search-time to reduce RAM usage.

## Sample Analysis

FFT + Envelope follower

## Comparison

Mean squared error is used currently



#include "AudioProcessor.hpp"
#include <iostream>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>

namespace CAP {
    AudioProcessor::AudioProcessor(std::vector<StreamWriter> sw): streamWriters(sw) {
    }
    
    void AudioProcessor::writeAudioSamples(std::vector<int16_t> samples) {
        for (int16_t sample : samples) {
            for(StreamWriter streamWriter : streamWriters) {
                streamWriter.write(sample);
            }
        }
    }
    void AudioProcessor::close() {
        for(StreamWriter streamWriter : streamWriters) {
            streamWriter.close();
        }
    }
}

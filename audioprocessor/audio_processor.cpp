

#include "audio_processor.hpp"
#include <iostream>


namespace CAP {
    AudioProcessor::AudioProcessor(std::vector<StreamWriter> sw): streamWriters(sw) {
    }
    
    void AudioProcessor::writeAudioSamples(std::vector<int16_t> samples) {
        for(StreamWriter streamWriter : streamWriters) {
            streamWriter.write(samples.data(), samples.size());
        }
    }
    void AudioProcessor::close() {
        for(StreamWriter streamWriter : streamWriters) {
            streamWriter.close();
        }
    }
}

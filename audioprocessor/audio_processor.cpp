
#include "audio_processor.hpp"
#include <iostream>

CAP::AudioProcessor::AudioProcessor(std::vector<StreamWriter> sw): streamWriters(sw) {
}

void CAP::AudioProcessor::writeAudioSamples(std::vector<int16_t>& samples) {
    for(StreamWriter streamWriter : streamWriters) {
        streamWriter.write(samples.data(), samples.size());
    }
}
void CAP::AudioProcessor::close() {
    for(StreamWriter streamWriter : streamWriters) {
        streamWriter.close();
    }
}

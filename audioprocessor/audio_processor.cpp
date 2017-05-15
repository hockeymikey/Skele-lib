
#include "audio_processor.hpp"
#include <iostream>

CAP::AudioProcessor::AudioProcessor(std::vector<StreamWriter> sw): streamWriters(sw) {
    for(auto& streamWriter : streamWriters) {
        streamWriter.start();
    }
}

void CAP::AudioProcessor::writeAudioSamples(std::vector<int16_t>& samples) {
    for(auto& streamWriter : streamWriters) {
        streamWriter.enqueue(samples);
    }
}
void CAP::AudioProcessor::close() {
//    for(StreamWriter streamWriter : streamWriters) {
//        streamWriter.close();
//    }
}

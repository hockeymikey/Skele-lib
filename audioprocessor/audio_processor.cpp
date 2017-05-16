
#include "audio_processor.hpp"
#include <iostream>

CAP::AudioProcessor::AudioProcessor(std::vector<std::shared_ptr<StreamWriter>> sw): streamWriters(sw) {
    for(auto& streamWriter : streamWriters) {
        streamWriter->start();
    }
}

void CAP::AudioProcessor::writeAudioSamples(std::vector<int16_t>& samples) {
    for(auto& streamWriter : streamWriters) {
        streamWriter->enqueue(samples);
    }
}
void CAP::AudioProcessor::stop() {
    for(auto& streamWriter : streamWriters) {
        streamWriter->stop().get();
    }
}


#include "audio_processor.hpp"
#include <iostream>

CAP::AudioProcessor::AudioProcessor(std::vector<std::shared_ptr<StreamWriter>> sw): streamWriters(sw) {
    if (streamWriters.empty()) {
        throw std::runtime_error("At least one stream writer is expected");
    }
    if (streamWriters[0]->hasCompressor()) {
        throw std::runtime_error("Stream writer with highest priority cannot have compressor atached to it");
    }
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

void CAP::AudioProcessor::scheduleAudioPostProcessing(std::function<void ()> callback) {
    
    std::async(std::launch::async, [](){
        
    });
//    callback();
}


#include "audio_processor.hpp"


CAP::AudioProcessor::AudioProcessor(std::vector<StreamWriter> &streamWriters): streamWriters(streamWriters) {
    if (streamWriters.size()) {
        throw std::runtime_error("At least one stream writer is expected");
    }
    for (auto &streamWriter: streamWriters) {
        streamWriter.start();
    }
}

void CAP::AudioProcessor::process(const AudioBuffer& audioBuffer) {
    for (auto &streamWriter: streamWriters) {
        streamWriter.enqueue(audioBuffer);
    }
}

void CAP::AudioProcessor::stop() {
    for (auto &streamWriter: streamWriters) {
        streamWriter.stop().get();
    }
}

CAP::AudioProcessor::~AudioProcessor() {
    
}

void CAP::AudioProcessor::scheduleAudioPostProcessing(std::function<void ()> callback) {
//    auto priorityWrite = streamWriters[0]->numberOfBuffersWritten();
//    for(auto& streamWriter : streamWriters) {
//        if (streamWriter == streamWriters[0]) {
//            //skip priority writer
//            continue;
//        }
//        streamWriter->start();
//    }
//    std::async(std::launch::async, [](){
//        
//    });
//    callback();
}

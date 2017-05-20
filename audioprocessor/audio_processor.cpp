
#include "audio_processor.hpp"
#include <array>

CAP::AudioProcessor::AudioProcessor(std::vector<StreamWriter> &sw): streamWriters(std::move(sw)) {
    if (streamWriters.size() == 0) {
        throw std::runtime_error("At least one stream writer is expected");
    }
    for (auto &streamWriter: streamWriters) {
        streamWriter.start();
    }
}

void CAP::AudioProcessor::process(std::int16_t *samples, std::size_t nsamples) {
    for (auto &streamWriter: streamWriters) {
        streamWriter.enqueue(AudioBuffer(samples, nsamples));
    }
}

void CAP::AudioProcessor::stop() {
    for (auto &streamWriter: streamWriters) {
        streamWriter.stop().get();
    }
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

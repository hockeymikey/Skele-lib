
#include "audio_processor.hpp"
#include <array>
#include <iostream>

CAP::AudioProcessor::AudioProcessor(StreamWriter * const streamWriters_, std::int8_t streamWriterCount_): streamWriters(streamWriters_), streamWriterCount(streamWriterCount_) {
    if (streamWriterCount == 0 || streamWriterCount > StreamWriterCapacity) {
        throw std::runtime_error("At least one stream writer, but no more than 5 writers is expected");
    }
    for (int i = 0; i < streamWriterCount; i++) {
        streamWriters[i].start();
    }
}

CAP::AudioProcessor::ProcessResult CAP::AudioProcessor::processBuffer(std::int16_t *samples, std::size_t nsamples) {
    
    auto result = ProcessResult::Success;

    
    for (int i = 0; i < streamWriterCount; i++) {
        auto qSize = streamWriters[i].queueSize();
        if (i == 0) {
            if (qSize >= streamWriterKillThreshold) {
                //priority writer has issues, kill all
                result = ProcessResult::PriorityWriterError;
                streamWriters[i].kill().get();
            } else {
                if (streamWriters[i].isWriteable()) {
                    streamWriters[i].enqueue(AudioBuffer(samples, nsamples));
                }
            }
        } else {
            //non-priority writer has issues, kill it
            if (qSize >= streamWriterKillThreshold || result == ProcessResult::PriorityWriterError) {
                streamWriters[i].kill().get();
                if (result != ProcessResult::PriorityWriterError) {
                    result = ProcessResult::NonPriorityWriterError;
                }
            } else {
                if (streamWriters[i].isWriteable()) {
                    streamWriters[i].enqueue(AudioBuffer(samples, nsamples));
                }
            }
        }
    }
    
    return result;
}

void CAP::AudioProcessor::stop() {
    for (int i = 0; i < streamWriterCount; i++) {
        streamWriters[i].stop().get();
    }
}


void CAP::AudioProcessor::scheduleAudioPostProcessing(std::function<void ()> callback) {
}

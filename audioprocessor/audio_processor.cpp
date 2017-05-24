
#include "audio_processor.hpp"
#include <array>
#include <iostream>

CAP::AudioProcessor::AudioProcessor(std::vector<StreamWriter> &sw): streamWriters(std::move(sw)) {
    if (streamWriters.size() == 0) {
        throw std::runtime_error("At least one stream writer is expected");
    }
    for (auto &streamWriter: streamWriters) {
        streamWriter.start();
    }
}

CAP::AudioProcessor::ProcessResult CAP::AudioProcessor::process(std::int16_t *samples, std::size_t nsamples) {
    int priority = 0;
    bool priorityWriterHasIssues = false;
    bool nonPriorityWriterHasIssues = false;
    int streamWritersToKillCount= 0;
    std::array<std::int8_t, 10> streamWritersTokill;
    
    for (auto &streamWriter: streamWriters) {
        if (priority == 0) {
            if (streamWriter.queueSize() >= streamWriterKillThreshold) {
                //priority writer has issues, kill all
                priorityWriterHasIssues = true;
                streamWritersTokill[streamWritersToKillCount] = priority;
                streamWritersToKillCount += 1;
            } else {
                streamWriter.enqueue(AudioBuffer(samples, nsamples));
            }
        } else {
            //non-priority writer has issues, kill it and throw exception
            if (streamWriter.queueSize() >= streamWriterKillThreshold || priorityWriterHasIssues) {
                streamWritersTokill[streamWritersToKillCount] = priority;
                streamWritersToKillCount += 1;
                nonPriorityWriterHasIssues = true;
            } else {
                streamWriter.enqueue(AudioBuffer(samples, nsamples));
            }
        }
        
        std::cout << priority << ":" << streamWriter.numberOfBuffersWritten() << " q:" << streamWriter.queueSize() << std::endl;
        priority += 1;
    }
    
    for (int i = 0; i < streamWritersToKillCount; i++) {
        int priority = streamWritersTokill[i];        
        streamWriters.at(priority).kill().get();
        streamWriters.erase(streamWriters.begin() + priority);
    }
    if (priorityWriterHasIssues) {
        return ProcessResult::PriorityWriterError;
    } else if (nonPriorityWriterHasIssues) {
        return ProcessResult::NonPriorityWriterError;
    }
    return ProcessResult::Success;
}

void CAP::AudioProcessor::stop() {
    for (auto &streamWriter: streamWriters) {
        streamWriter.stop().get();
    }
//    while (true) {
//        int i = 0;
//        for (auto &streamWriter: streamWriters) {
//            std::cout << i << "+:" << streamWriter.numberOfBuffersWritten() << " q:" << streamWriter.queueSize() << std::endl;
//            i += 1;
//        }
//        
//        
//        std::this_thread::sleep_for(std::chrono::milliseconds(10));
//        
//    }
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


#include "audio_processor.hpp"
#include <array>
#include <iostream>

CAP::AudioProcessor::AudioProcessor(std::vector<StreamWriter> &sw) {
    if (sw.size() == 0 || sw.size() > 5) {
        throw std::runtime_error("At least one stream writer, but no more than 5 writers is expected");
    }
    for (int i = 0; i < sw.size(); i++) {
        PrioritizedStreamWriter ps(sw.at(i), i);
        prioritizedStreamWriters.push_back(std::move(ps));
    }
    
    for(auto &psw: prioritizedStreamWriters) {
        psw.getStreamWriter().start();
    }
}

CAP::AudioProcessor::ProcessResult CAP::AudioProcessor::process(std::int16_t *samples, std::size_t nsamples) {
    
    auto result = ProcessResult::Success;
    int streamWritersToKillCount= 0;
    std::array<std::int8_t, 5> streamWritersTokill;
    
    for (auto &pStreamWriter: prioritizedStreamWriters) {
        if (pStreamWriter.getPriority() == 0) {
            if (pStreamWriter.getStreamWriter().queueSize() >= streamWriterKillThreshold) {
                //priority writer has issues, kill all
                result = ProcessResult::PriorityWriterError;
                streamWritersTokill[streamWritersToKillCount] = pStreamWriter.getPriority();
                streamWritersToKillCount += 1;
            } else {
                pStreamWriter.getStreamWriter().enqueue(AudioBuffer(samples, nsamples));
            }
        } else {
            //non-priority writer has issues, kill it and throw exception
            if (pStreamWriter.getStreamWriter().queueSize() >= streamWriterKillThreshold || result == ProcessResult::PriorityWriterError) {
                streamWritersTokill[streamWritersToKillCount] = pStreamWriter.getPriority();
                streamWritersToKillCount += 1;
                if (result != ProcessResult::PriorityWriterError) {
                    result = ProcessResult::NonPriorityWriterError;
                }
            } else {
                pStreamWriter.getStreamWriter().enqueue(AudioBuffer(samples, nsamples));
            }
        }
        
        std::cout << pStreamWriter.getPriority() << ":" << pStreamWriter.getStreamWriter().numberOfBuffersWritten() << " q:" << pStreamWriter.getStreamWriter().queueSize() << std::endl;
        
    }
    
    for (int i = 0; i < streamWritersToKillCount; i++) {
        int priority = streamWritersTokill[i];
        for (int g = 0; g < prioritizedStreamWriters.size(); g++) {
            if (prioritizedStreamWriters[g].getPriority() == priority) {
                prioritizedStreamWriters.at(g).getStreamWriter().kill().get();
                prioritizedStreamWriters.erase(prioritizedStreamWriters.begin() + g);
                break;
            }
        }
        
        
        
    }
    
    return result;
}

void CAP::AudioProcessor::stop() {
    for (auto &pstreamWriter: prioritizedStreamWriters) {
        pstreamWriter.getStreamWriter().stop().get();
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

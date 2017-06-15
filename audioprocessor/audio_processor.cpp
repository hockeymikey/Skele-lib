
#include "audio_processor.hpp"
#include <array>
#include <iostream>

CAP::AudioProcessor::AudioProcessor(StreamWriter * const streamWriters, std::uint8_t streamWriterCount) {
            
    streamWriterBundles[bundleCount].streamWriters = streamWriters;
    streamWriterBundles[bundleCount].streamWriterCount = streamWriterCount;
    streamWriterBundles[bundleCount].isPostProcessing = false;
    
    for (std::uint8_t i = 0; i < streamWriterCount; i++) {
        streamWriters[i].start();
    }
    bundleCount++;
}

CAP::AudioProcessor::ProcessResult CAP::AudioProcessor::processBuffer(std::int16_t *samples, std::size_t nsamples) {
    
    auto result = ProcessResult::Success;

    for (std::uint8_t g = 0; g < bundleCount; g++) {
        if (streamWriterBundles[g].isPostProcessing) {
            continue;
        }
        
        for (std::uint8_t i = 0; i < streamWriterBundles[g].streamWriterCount; i++) {
            auto qSize = streamWriterBundles[g].streamWriters[i].queueSize();
            if (i == 0) {
                if (qSize >= streamWriterKillThreshold) {
                    //priority writer has issues, kill all
                    result = ProcessResult::PriorityWriterError;
                    streamWriterBundles[g].streamWriters[i].kill().get();
                } else {
                    if (streamWriterBundles[g].streamWriters[i].isWriteable()) {
                        streamWriterBundles[g].streamWriters[i].enqueue(AudioBuffer(samples, nsamples));
                    }
                }
            } else {
                //non-priority writer has issues, kill it
                if (qSize >= streamWriterKillThreshold || result == ProcessResult::PriorityWriterError) {
                    streamWriterBundles[g].streamWriters[i].kill().get();
                    if (result != ProcessResult::PriorityWriterError) {
                        result = ProcessResult::NonPriorityWriterError;
                    }
                } else {
                    if (streamWriterBundles[g].streamWriters[i].isWriteable()) {
                        streamWriterBundles[g].streamWriters[i].enqueue(AudioBuffer(samples, nsamples));
                    }
                }
            }
        }
        
    }
    
    
    
    return result;
}

std::future<void> CAP::AudioProcessor::stop(std::function<void ()> callback) {
    
    return std::async(std::launch::async, [this, callback] {
        for (std::uint8_t g = 0; g < bundleCount; g++) {
            if (streamWriterBundles[g].isPostProcessing) {
                continue;
            }
            for (std::uint8_t i = 0; i < streamWriterBundles[g].streamWriterCount; i++) {
                streamWriterBundles[g].streamWriters[i].stop().get();
            }
        }
        callback();
    });
}


void CAP::AudioProcessor::schedulePostProcess(StreamWriter * const streamWriters, std::uint8_t streamWriterCount, std::function<void ()> callback) {
    
    //retire previous stream writers
    streamWriterBundles[bundleCount - 1].isPostProcessing = true;
    auto oldStreamWriters = streamWriterBundles[bundleCount - 1].streamWriters;
    auto oldStreamWriterCount = streamWriterBundles[bundleCount - 1].streamWriterCount;
    
    auto future = std::async(std::launch::async, [oldStreamWriters, oldStreamWriterCount, callback] {
        for (auto i = 0; i < oldStreamWriterCount; i++) {
            oldStreamWriters[i].stop().get();
        }
        callback();
    });
    
    streamWriterBundles[bundleCount].streamWriters = streamWriters;
    streamWriterBundles[bundleCount].streamWriterCount = streamWriterCount;
    streamWriterBundles[bundleCount].isPostProcessing = false;    
    
    for (std::uint8_t i = 0; i < streamWriterCount; i++) {
        streamWriters[i].start();
    }
    bundleCount++;
}

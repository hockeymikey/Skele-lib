
#include "audio_processor.hpp"
#include <array>
#include <iostream>

CAP::AudioProcessor::AudioProcessor(std::vector<std::shared_ptr<StreamWriter>> sws) {
    
    for(auto &sw: sws) {
        sw->start();
    }
    
    auto bundle = std::make_shared<StreamWriterBundle>(sws);
    streamWriterBundles.push_back(bundle);
    
    
    
}

CAP::AudioProcessor::ProcessResult CAP::AudioProcessor::processBuffer(std::int16_t *samples, std::size_t nsamples) {
    
    auto result = ProcessResult::Success;

    for (auto &bundle: streamWriterBundles) {
        if (bundle->isPostProcessing) {
            continue;
        }
        
        for(auto it = bundle->streamWriters.begin(); it != bundle->streamWriters.end(); ++it) {
            auto sw = *it;
            auto qSize = sw->queueSize();
        
            if (it == bundle->streamWriters.begin()) {
                if (qSize >= streamWriterKillThreshold) {
                    //priority writer has issues, kill all
                    result = ProcessResult::PriorityWriterError;
                    sw->kill().get();
                } else {
                    if (sw->isWriteable()) {
                        sw->enqueue(AudioBuffer(samples, nsamples));
                    }
                }
            } else {
                //non-priority writer has issues, kill it
                if (qSize >= streamWriterKillThreshold || result == ProcessResult::PriorityWriterError) {
                    sw->kill().get();
                    if (result != ProcessResult::PriorityWriterError) {
                        result = ProcessResult::NonPriorityWriterError;
                    }
                } else {
                    if (sw->isWriteable()) {
                        sw->enqueue(AudioBuffer(samples, nsamples));
                    }
                }
            }
        }
        
    }
    
    
    
    return result;
}

std::future<void> CAP::AudioProcessor::stop(std::function<void ()> callback) {
    
    return std::async(std::launch::async, [this, callback] {
        for (auto &bundle: streamWriterBundles) {
            if (bundle->isPostProcessing) {
                continue;
            }
            for (auto &sw: bundle->streamWriters) {
                sw->stop().get();
            }
        }
        callback();
    });
}

std::shared_ptr<CAP::StreamWriter> CAP::AudioProcessor::getCurrentPriorityStreamWriter() {
    auto it = streamWriterBundles.end() - 1;
    return (*it)->streamWriters.front();
}

void CAP::AudioProcessor::schedulePostProcess(std::vector<std::shared_ptr<StreamWriter>> sws, std::function<void ()> callback) {
    
    schedulePostProcess(callback);
    
    auto bundle = std::make_shared<StreamWriterBundle>(sws);
    
    for(auto &sw: bundle->streamWriters) {
        sw->start();
    }
    
    streamWriterBundles.push_back(bundle);
    
    
}

void CAP::AudioProcessor::schedulePostProcess(std::function<void ()> callback) {
    
    //retire previous stream writers
    streamWriterBundles.back()->isPostProcessing = true;
    
    auto currentBundle = streamWriterBundles.back();

    auto future = std::async(std::launch::async, [currentBundle, callback] {
        for( auto &sw: currentBundle->streamWriters) {
            sw->stop().get();
        }
        callback();
    });
    
    
}

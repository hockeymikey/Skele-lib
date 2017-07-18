
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

//CAP::AudioProcessor::AudioProcessor(StreamWriter streamWriter, StreamWriter other) {
//    
//    TemplatedStreamWriterBundle<2> bundle(std::move(streamWriter), std::move(other));
//    
////    streamWriterBundles[bundleCount] = std::move(bundle);
////    
////    StreamWriter* sw = getPriorityStreamWriterFromBundle(&(streamWriterBundles[bundleCount]));
//
//    StreamWriterBundle b = std::move(bundle);
//    
//    auto b2 = std::move(static_cast<TemplatedStreamWriterBundle<2>&>(b));
//    b2.getPriorityStreamWriter()->start();
////    StreamWriter* sw = getPriorityStreamWriterFromBundle(&b);
//    
////    sw->start();
////    (sw + 1)->start();
//
//    bundleCount++;
//}


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

//CAP::StreamWriter* CAP::AudioProcessor::getPriorityStreamWriterFromBundle(StreamWriterBundle* swb) {
//    StreamWriter* sw = nullptr;
//    if (swb->streamWriterCount == 1) {
//        sw = static_cast<TemplatedStreamWriterBundle<1>*>(swb)->getPriorityStreamWriter();
//    } else if (swb->streamWriterCount == 2) {
//        sw = static_cast<TemplatedStreamWriterBundle<2>*>(swb)->getPriorityStreamWriter();
//    }
//    
//    return sw;
//}

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

//std::shared_ptr<CAP::StreamWriter> CAP::AudioProcessor::getCurrentPriorityStreamWriter() {
//    auto it = streamWriterBundles.end() - 1;
//    return (*it)->streamWriters.front();
//}

void CAP::AudioProcessor::schedulePostProcess(std::vector<std::shared_ptr<StreamWriter>> sws, std::function<void ()> callback) {
    
    schedulePostProcess(callback);
    
    auto bundle = std::make_shared<StreamWriterBundle>(sws);
    
    for(auto &sw: bundle->streamWriters) {
        sw->start();
    }
    
    streamWriterBundles.push_back(bundle);
    
    
}

//void CAP::AudioProcessor::schedulePostProcess(CAP::StreamWriter priority, std::function<void ()> callback) {
//    schedulePostProcess(callback);
//    
//    priority.start();
//    
//    static TemplatedStreamWriterBundle<1> bundle(priority);
//    
//    streamWriterBundles[bundleCount] = std::move(bundle);
//    
////    streamWriterBundles[bundleCount].getPriorityStreamWriter()->start();
//    
//    
//    bundleCount++;
//}

//CAP::StreamWriter * CAP::AudioProcessor::getPriorityStreamWriter(StreamWriterBundle * swb) {
//    return swb->getPriorityStreamWriter();
//}
//std::uint8_t CAP::AudioProcessor::getStreamWriterCount(StreamWriterBundle * swb) {
//    return swb->getStreamWriterCount();
//}

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

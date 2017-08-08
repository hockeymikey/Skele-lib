
#include "audio_processor.hpp"

CAP::AudioProcessor::AudioProcessor(std::unique_ptr<AbstractCircularQueue> circularQueue_): circularQueue(std::move(circularQueue_)) {
    
}

void CAP::AudioProcessor::startHighlight(std::vector<std::shared_ptr<StreamWriter>> streamWriters, std::uint8_t recommendedDelayInSeconds) {
    for(auto &sw: streamWriters) {
        sw->start();
    }
    auto bundle = std::make_shared<StreamWriterBundle>(streamWriters, recommendedDelayInSeconds);
    streamWriterBundles.push_back(bundle);

}

CAP::AudioProcessor::Status CAP::AudioProcessor::processSamples(std::int16_t *samples, std::size_t nsamples) {
    
    auto bundle = streamWriterBundles.back();
    
    //bypass circular queue if no delay
    if (bundle->recommendedDelayInSeconds == 0) {
        return enqueueSamples(samples, nsamples);
    }
    
    for (std::size_t i = 0; i < nsamples; i++) {
        circularQueue->enqueue(samples[i]);
    }
    
    if (circularQueue->isFull()) {
        return Status::FullBuffer;
    }
    
    auto delayInSamples = bundle->recommendedDelayInSeconds * 44100;
    
    std::size_t dequeueCount = 0;
    auto qSize = circularQueue->size();
    if (qSize > delayInSamples) {
        ;
    }
    while (qSize > delayInSamples && dequeueCount < nsamples) {
        auto oldestSample = circularQueue->front();
        circularQueue->dequeue();
        samples[dequeueCount] = oldestSample;
        dequeueCount++;
    }
    
    if (dequeueCount > 0) {
        return enqueueSamples(samples, dequeueCount);
    }
    
    return Status::Success;
}



CAP::AudioProcessor::Status CAP::AudioProcessor::enqueueSamples(std::int16_t *samples, std::size_t nsamples) {
    
    auto result = Status::Success;
    auto bundle = streamWriterBundles.back();
    for(auto it = bundle->streamWriters.begin(); it != bundle->streamWriters.end(); ++it) {
        auto sw = *it;
        auto qSize = sw->queueSize();
        
        if (it == bundle->streamWriters.begin()) {
            if (qSize >= streamWriterKillThreshold) {
                //priority writer has issues, kill all
                result = Status::PriorityWriterError;
                sw->kill();
            } else {
                if (sw->isWriteable()) {
                    sw->enqueue(AudioBuffer(samples, nsamples));
                }
            }
        } else {
            //non-priority writer has issues, kill it
            if (qSize >= streamWriterKillThreshold || result == Status::PriorityWriterError) {
                sw->kill();
                if (result != Status::PriorityWriterError) {
                    result = Status::NonPriorityWriterError;
                }
            } else {
                if (sw->isWriteable()) {
                    sw->enqueue(AudioBuffer(samples, nsamples));
                }
            }
        }
    }
    
    return result;
}

void CAP::AudioProcessor::stopHighlight() {
    for (auto &sw: streamWriterBundles.back()->streamWriters) {
        sw->stop();
    }
}


//void CAP::AudioProcessor::schedulePostProcess(std::vector<std::shared_ptr<StreamWriter>> sws, std::function<void ()> callback) {

//    schedulePostProcess(callback);
//    
//    auto bundle = std::make_shared<StreamWriterBundle>(sws);
//    
//    for(auto &sw: bundle->streamWriters) {
//        sw->start();
//    }
//    
//    streamWriterBundles.push_back(bundle);
    
    
//}

//void CAP::AudioProcessor::schedulePostProcess(std::function<void ()> callback) {
//    
//    //retire previous stream writers
//    streamWriterBundles.back()->isPostProcessing = true;
//    
//    auto currentBundle = streamWriterBundles.back();
//
//    auto future = std::async(std::launch::async, [currentBundle, callback] {
//        for( auto &sw: currentBundle->streamWriters) {
//            sw->stop().get();
//        }
//        callback();
//    });
//    
//    
//}

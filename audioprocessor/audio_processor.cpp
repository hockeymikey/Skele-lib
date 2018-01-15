
#include "audio_processor.hpp"
#include <algorithm>

CAP::AudioProcessor::AudioProcessor(std::unique_ptr<AbstractCircularQueue> circularQueue_): circularQueue(std::move(circularQueue_)) {
    
}

CAP::AudioProcessor::AudioProcessor(std::unique_ptr<AbstractCircularQueue> circularQueue_, std::uint32_t streamWriterKillThresholdInSamples_): circularQueue(std::move(circularQueue_)), streamWriterKillThreshold(streamWriterKillThresholdInSamples_ / AudioBuffer::AudioBufferCapacity) {

}

double CAP::AudioProcessor::startHighlight(std::vector<std::shared_ptr<StreamWriter>> streamWriters, std::uint32_t recommendedDelayInSamples) {
    double bufferedAudioInSeconds = 0.0;
    std::shared_ptr<StreamWriterBundle> oldBundle = nullptr;
    auto newBundle = std::make_shared<StreamWriterBundle>(streamWriters, recommendedDelayInSamples);
    
    for(auto &sw: streamWriters) {
        sw->start();
    }
    
    std::unique_lock<std::mutex> bundlesLock(bundlesMutex);
    if (!streamWriterBundles.empty()) {
        oldBundle = streamWriterBundles.back();
    }
    streamWriterBundles.push_back(newBundle);
    bundlesLock.unlock();
    
    if (oldBundle != nullptr) {
        if (oldBundle->recommendedDelayInSamples > 0) {
            auto qSize = circularQueueSize();
            if (qSize > 0) {
                if (qSize <= oldBundle->recommendedDelayInSamples) {
//                    bufferedAudioInSeconds = qSize / 44100.0;
                    flushCircularQueue(qSize, newBundle);
                } else {
//                    bufferedAudioInSeconds = oldBundle->recommendedDelayInSamples / 44100.0;
                    flushCircularQueue(oldBundle->recommendedDelayInSamples, newBundle);
                }
            }
            
            auto timeSpan = std::chrono::system_clock::now() - oldBundle->startTime;
            auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count();
            auto recommendedDelayInSeconds = oldBundle->recommendedDelayInSamples / 44100.0;
            if (millis > recommendedDelayInSeconds * 1000.0) {
                bufferedAudioInSeconds = recommendedDelayInSeconds;
            } else {
                bufferedAudioInSeconds = millis / 1000.0;
            }
        }
        
    }
    
    return bufferedAudioInSeconds;
}

std::size_t CAP::AudioProcessor::circularQueueSize() {
    std::lock_guard<std::mutex> queueLock(circularQueueMutex);
    return circularQueue->size();
}

CAP::AudioProcessor::Status CAP::AudioProcessor::processSamples(std::int16_t *samples, std::size_t nsamples) {
    std::shared_ptr<StreamWriterBundle> currentBundle = nullptr;
    std::unique_lock<std::mutex> queueLock(circularQueueMutex, std::defer_lock);
    
    std::unique_lock<std::mutex> bundlesLock(bundlesMutex);
    if (!streamWriterBundles.empty()) {
        currentBundle = streamWriterBundles.back();
    }
    bundlesLock.unlock();
    
    queueLock.lock();
    for (std::size_t i = 0; i < nsamples; i++) {
        circularQueue->enqueue(samples[i]);
    }
    auto qIsFull = circularQueue->isFull();
    auto qSize = circularQueue->size();
    queueLock.unlock();
    
    if (qIsFull) {
        return Status::FullBuffer;
    }
    
    if (currentBundle == nullptr) {
        return Status::Success;
    }
    
    auto delayInSamples = currentBundle->recommendedDelayInSamples;
    
    std::size_t dequeueCount = 0;
    
    while (qSize > delayInSamples && dequeueCount < nsamples) {
        queueLock.lock();
        auto oldestSample = circularQueue->front();
        circularQueue->dequeue();
        qSize = circularQueue->size();
        queueLock.unlock();
        samples[dequeueCount] = oldestSample;
        dequeueCount++;
    }
    
    if (dequeueCount > 0) {
        return enqueueSamples(samples, dequeueCount, currentBundle);
    }
    
    return Status::Success;
}



CAP::AudioProcessor::Status CAP::AudioProcessor::enqueueSamples(std::int16_t *samples, std::size_t nsamples, std::shared_ptr<StreamWriterBundle> currentBundle) {
    
    auto result = Status::Success;
    
    for(auto it = currentBundle->streamWriters.begin(); it != currentBundle->streamWriters.end(); ++it) {
        auto sw = *it;
        auto qSize = sw->queueSize();
        
        if (it == currentBundle->streamWriters.begin()) { //find priority writer
            if (qSize >= streamWriterKillThreshold) {
                //priority writer has issues, kill all
                result = Status::PriorityStreamWriterKilledDueToOverflow;
                sw->kill();
            } else {
                if (sw->isWriteable()) {
                    sw->enqueue(AudioBuffer(samples, nsamples));
                } else {
                    result = Status::StreamWritersNotRunning;
                }
            }
        } else {
            if (result == Status::StreamWritersNotRunning) {
                //noop
            } else if (result == Status::PriorityStreamWriterKilledDueToOverflow) {
                // priority writer has issues, therefore kill non priority writers
                sw->kill();
            } else if (qSize >= streamWriterKillThreshold) {
                //non-priority writer has issues, kill it
                sw->kill();
                result = Status::NonPriorityStreamWriterKilledDueToOverflow;
            } else {
                if (sw->isWriteable()) {
                    sw->enqueue(AudioBuffer(samples, nsamples));
                } else {
                    result = Status::NonPriorityStreamWriterNotRunning;
                }
            }
        }
    }
    
    
    return result;
}

void CAP::AudioProcessor::flushCircularQueue(std::size_t flushLimitInSamples, std::shared_ptr<StreamWriterBundle> bundle) {
    std::int16_t samples[AudioBuffer::AudioBufferCapacity];
    std::size_t sampleCount = 0;
    std::unique_lock<std::mutex> queueLock(circularQueueMutex, std::defer_lock);
    
    while (flushLimitInSamples > 0) {
        
        queueLock.lock();
        samples[sampleCount] = circularQueue->front();
        circularQueue->dequeue();
        queueLock.unlock();
        sampleCount++;
        
        
        if (sampleCount == AudioBuffer::AudioBufferCapacity || flushLimitInSamples - 1 == 0) {
            enqueueSamples(samples, sampleCount, bundle);
            sampleCount = 0;
        }
        
        flushLimitInSamples--;
        
    }
    
}

bool CAP::AudioProcessor::isFileBeingProcessedAtFilepath(std::string filepath) {
    
    std::lock_guard<std::mutex> bundlesLock(bundlesMutex);
    if (streamWriterBundles.empty()) {
        return false;
    }
    for (auto &bundle: streamWriterBundles) {
        for(auto &sw: bundle->streamWriters) {
            if (sw->getFilePath() == filepath && sw->isWriteable()) {
                return true;
            }
        }
    }
    
    return false;
}

void CAP::AudioProcessor::stopHighlight(bool flushQueue) {
    
    std::shared_ptr<StreamWriterBundle> currentBundle = nullptr;
    std::unique_lock<std::mutex> bundlesLock(bundlesMutex);
    if (!streamWriterBundles.empty()) {
        currentBundle = streamWriterBundles.back();
    }
    bundlesLock.unlock();
    
    auto qSize = circularQueueSize();
    
    if (flushQueue && qSize > 0) {
        auto currentRecommendedDelayInSamples = currentBundle->recommendedDelayInSamples;
        if (qSize <= currentRecommendedDelayInSamples) {
            flushCircularQueue(qSize, currentBundle);
        } else {
            flushCircularQueue(currentRecommendedDelayInSamples, currentBundle);
        }
    }
    
    
    for (auto &sw: currentBundle->streamWriters) {
        sw->stop();
    }
}

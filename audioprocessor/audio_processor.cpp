
#include "audio_processor.hpp"
#include <iostream>

CAP::AudioProcessor::AudioProcessor(std::unique_ptr<AbstractCircularQueue> circularQueue_): circularQueue(std::move(circularQueue_)) {
    
}

void CAP::AudioProcessor::startHighlight(std::vector<std::shared_ptr<StreamWriter>> streamWriters, std::uint32_t recommendedDelayInSamples) {
    
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
                    flushCircularQueue(qSize);
                } else {
                    flushCircularQueue(oldBundle->recommendedDelayInSamples);
                }
            }
            
        }
        
    }
    
    
    
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
        return enqueueSamples(samples, dequeueCount);
    }
    
    return Status::Success;
}



CAP::AudioProcessor::Status CAP::AudioProcessor::enqueueSamples(std::int16_t *samples, std::size_t nsamples) {
    
    auto result = Status::Success;
    std::unique_lock<std::mutex> bundlesLock(bundlesMutex);
    auto bundle = streamWriterBundles.back();
    bundlesLock.unlock();
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
                } else {
                    result = Status::PriorityWriterError;
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
                } else {
                    result = Status::NonPriorityWriterError;
                }
            }
        }
    }
    
    return result;
}

void CAP::AudioProcessor::flushCircularQueue(std::size_t flushLimitInSamples) {
    std::int16_t samples[AudioBuffer::AudioBufferCapacity];
    std::size_t sampleCount = 0;
    std::size_t flushCount = 0;
    std::unique_lock<std::mutex> bundlesLock(bundlesMutex);
    auto hasBundles = !streamWriterBundles.empty();
    bundlesLock.unlock();
    std::unique_lock<std::mutex> queueLock(circularQueueMutex, std::defer_lock);
    
    while (flushCount <= flushLimitInSamples) {
        if (sampleCount == AudioBuffer::AudioBufferCapacity) {
            if (hasBundles) {
                auto status = enqueueSamples(samples, sampleCount);
                switch (status) {
                    case Status::NonPriorityWriterError:
                        break;
                    case Status::PriorityWriterError:
                        break;
                    case Status::Success:
                        break;
                    default:
                        break;
                }
            }
            
            sampleCount = 0;
        } else {
            queueLock.lock();
            auto qSize = circularQueue->size();
            samples[sampleCount] = circularQueue->front();
            queueLock.unlock();
            if (qSize == 0) {
                if (hasBundles) {
                    auto status = enqueueSamples(samples, sampleCount);
                    switch (status) {
                        case Status::NonPriorityWriterError:
                            break;
                        case Status::PriorityWriterError:
                            break;
                        case Status::Success:
                            break;
                        default:
                            break;
                    }
                }
            } else {
                sampleCount++;
            }
        }
        
        queueLock.lock();
        circularQueue->dequeue();
        queueLock.unlock();
        flushCount++;
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
    
    if (currentBundle != nullptr) {
        if (flushQueue && qSize > 0) {
            if (qSize <= currentBundle->recommendedDelayInSamples) {
                flushCircularQueue(qSize);
            } else {
                flushCircularQueue(currentBundle->recommendedDelayInSamples);
            }
        }
        
        
        for (auto &sw: currentBundle->streamWriters) {
            sw->stop();
        }
    } else {
        if (flushQueue && qSize > 0) {
            flushCircularQueue(qSize);
        }
    }
}

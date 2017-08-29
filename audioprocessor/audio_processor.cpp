
#include "audio_processor.hpp"

CAP::AudioProcessor::AudioProcessor(std::unique_ptr<AbstractCircularQueue> circularQueue_): circularQueue(std::move(circularQueue_)) {
    
}

void CAP::AudioProcessor::startHighlight(std::vector<std::shared_ptr<StreamWriter>> streamWriters, std::uint8_t recommendedDelayInSeconds) {
    std::unique_lock<std::mutex> bundlesLock(bundlesMutex);
    bundlesLock.unlock();
    for(auto &sw: streamWriters) {
        sw->start();
    }
    auto oldRecommmendedDelay = 0;
    
    bundlesLock.lock();
    if (streamWriterBundles.size() > 0) {
        oldRecommmendedDelay = streamWriterBundles.back()->recommendedDelayInSeconds;
    }
    auto bundle = std::make_shared<StreamWriterBundle>(streamWriters, recommendedDelayInSeconds);
    streamWriterBundles.push_back(bundle);
    bundlesLock.unlock();
    
    auto delayInSamples = oldRecommmendedDelay * 44100;
    
    flushCircularQueue(delayInSamples);
}

CAP::AudioProcessor::Status CAP::AudioProcessor::processSamples(std::int16_t *samples, std::size_t nsamples) {
    
    std::unique_lock<std::mutex> queueLock(circularQueueMutex);
    queueLock.unlock();
    std::unique_lock<std::mutex> bundlesLock(bundlesMutex);
    auto bundle = streamWriterBundles.back();
    bundlesLock.unlock();
    
    
    //bypass circular queue if no delay
    if (bundle->recommendedDelayInSeconds == 0) {
        return enqueueSamples(samples, nsamples);
    }
    
    for (std::size_t i = 0; i < nsamples; i++) {
        queueLock.lock();
        circularQueue->enqueue(samples[i]);
        queueLock.unlock();
    }
    
    queueLock.lock();
    auto qIsFull = circularQueue->isFull();
    queueLock.unlock();
    
    if (qIsFull) {
        return Status::FullBuffer;
    }
    
    auto delayInSamples = bundle->recommendedDelayInSeconds * 44100;
    
    std::size_t dequeueCount = 0;
    queueLock.lock();
    auto qSize = circularQueue->size();
    queueLock.unlock();
    if (qSize > delayInSamples) {
        ;
    }
    while (qSize > delayInSamples && dequeueCount < nsamples) {
        queueLock.lock();
        auto oldestSample = circularQueue->front();
        circularQueue->dequeue();
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
    std::unique_lock<std::mutex> queueLock(circularQueueMutex);
    queueLock.unlock();
    while (flushCount <= flushLimitInSamples) {
        if (sampleCount == AudioBuffer::AudioBufferCapacity) {
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
            sampleCount = 0;
        } else {
            queueLock.lock();
            auto qSize = circularQueue->size();
            samples[sampleCount] = circularQueue->front();
            queueLock.unlock();
            if (qSize == 1) {
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
            } else if (qSize == 0) {
                break;
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

void CAP::AudioProcessor::stopHighlight(bool flushQueue) {
    if (flushQueue) {
        std::unique_lock<std::mutex> queueLock(circularQueueMutex);
        auto size = circularQueue->size();
        queueLock.unlock();
        flushCircularQueue(size);
    }
    std::unique_lock<std::mutex> bundlesLock(bundlesMutex);
    auto bundle = streamWriterBundles.back();
    bundlesLock.unlock();
    for (auto &sw: bundle->streamWriters) {
        sw->stop();
    }
}

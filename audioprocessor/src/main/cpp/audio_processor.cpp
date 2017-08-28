
#include "audio_processor.hpp"

CAP::AudioProcessor::AudioProcessor(std::unique_ptr<AbstractCircularQueue> circularQueue_): circularQueue(std::move(circularQueue_)) {
    
}

void CAP::AudioProcessor::startHighlight(std::vector<std::shared_ptr<StreamWriter>> streamWriters, std::uint8_t recommendedDelayInSeconds) {
    for(auto &sw: streamWriters) {
        sw->start();
    }
    auto oldRecommmendedDelay = 0;
    if (streamWriterBundles.size() > 0) {
        oldRecommmendedDelay = streamWriterBundles.back()->recommendedDelayInSeconds;
    }
    auto bundle = std::make_shared<StreamWriterBundle>(streamWriters, recommendedDelayInSeconds);
    streamWriterBundles.push_back(bundle);
    
    auto delayInSamples = oldRecommmendedDelay * 44100;
    
    flushCircularQueue(delayInSamples);
}

CAP::AudioProcessor::Status CAP::AudioProcessor::processSamples(std::int16_t *samples, std::size_t nsamples) {
    
    auto bundle = streamWriterBundles.back();
    
    //bypass circular queue if no delay
    if (bundle->recommendedDelayInSeconds == 0) {
        return enqueueSamples(samples, nsamples);
    }
    
    for (std::size_t i = 0; i < nsamples; i++) {
        std::unique_lock<std::mutex> queueLock(circularQueueMutex);
        circularQueue->enqueue(samples[i]);
        queueLock.unlock();
    }
    
    std::unique_lock<std::mutex> queueLock(circularQueueMutex);
    auto qIsFull = circularQueue->isFull();
    queueLock.unlock();
    
    if (qIsFull) {
        return Status::FullBuffer;
    }
    
    auto delayInSamples = bundle->recommendedDelayInSeconds * 44100;
    
    std::size_t dequeueCount = 0;
    std::unique_lock<std::mutex> queueLock2(circularQueueMutex);
    auto qSize = circularQueue->size();
    queueLock2.unlock();
    if (qSize > delayInSamples) {
        ;
    }
    while (qSize > delayInSamples && dequeueCount < nsamples) {
        std::unique_lock<std::mutex> queueLock(circularQueueMutex);
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
            std::unique_lock<std::mutex> queueLock(circularQueueMutex);
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
        
        std::unique_lock<std::mutex> queueLock(circularQueueMutex);
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
    for (auto &sw: streamWriterBundles.back()->streamWriters) {
        sw->stop();
    }
}
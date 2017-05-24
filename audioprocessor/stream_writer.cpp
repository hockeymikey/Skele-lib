//
//  StreamWriter.cpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/2/17.
//
//

#include "stream_writer.hpp"
#include <thread>
#include <cstdlib>
#include <iostream>
#include <memory>

CAP::StreamWriter::StreamWriter(std::string filepath): filepath(filepath), waitMutex(new std::mutex()), queueMutex(new std::mutex()), buffersWrittenMutex(new std::mutex()), queueConditionVariable(new std::condition_variable()) {
}
CAP::StreamWriter::StreamWriter(std::string filepath, std::unique_ptr<SignalProcessor> cmp): filepath(filepath), signalProcessor(std::move(cmp)), waitMutex(new std::mutex()), queueMutex(new std::mutex()), buffersWrittenMutex(new std::mutex()), queueConditionVariable(new std::condition_variable()) {
}

void CAP::StreamWriter::enqueue(CAP::AudioBuffer audioBuffer) {
    // 1. Secure our queue access.
    std::lock_guard<std::mutex> bufferLock(*queueMutex);
    
    
    // 2. Insert new buffer.
    bufferQueue.push(std::move(audioBuffer));
    
    // 3. Notify the loop thread.
    queueConditionVariable->notify_one();
}

std::future<void> CAP::StreamWriter::stop() {
    stopLoop = true;
    queueConditionVariable->notify_one();
    return stopPromise.get_future();
}

std::future<void> CAP::StreamWriter::kill() {
    killLoop = true;
    queueConditionVariable->notify_one();
    auto future = killPromise.get_future();
    if (hasError) {
        killPromise.set_value();
    }
    return future;
}

std::size_t CAP::StreamWriter::numberOfBuffersWritten() {
    std::lock_guard<std::mutex> buffersWrittenLock(*buffersWrittenMutex);
    return buffersWritten;
}

void CAP::StreamWriter::start() {
    stopLoop = false;
    killLoop = false;
    hasError = false;
    stopPromise = std::promise<void>();
    killPromise = std::promise<void>();
    std::thread(&CAP::StreamWriter::runLoop, this).detach();
}

bool CAP::StreamWriter::hasSignalProcessor() const {
    return signalProcessor != nullptr;
}
CAP::StreamWriter::~StreamWriter() {
    
}

std::size_t CAP::StreamWriter::queueSize() {
    std::lock_guard<std::mutex> bufferLock(*queueMutex);
    return bufferQueue.size();
}

void CAP::StreamWriter::runLoop() {
    std::ofstream fileStream(filepath, std::ofstream::binary | std::ofstream::app);
    std::unique_lock<std::mutex> loopLock(*waitMutex);
    while (true) {
        // 1. Wait for an event if the queue is empty.
        std::unique_lock<std::mutex> queueLock(*queueMutex);
        auto isEmpty = bufferQueue.empty();
        queueLock.unlock();
        
        if (stopLoop) {
            if (isEmpty) {
                stopPromise.set_value();
                return;
            }
        } else if (killLoop) {
            killPromise.set_value();
            return;
        }
        
        if (isEmpty) {
            queueConditionVariable->wait(loopLock);
            continue;
        }
        
        
        // 3. Get the next buffer.
        queueLock.lock();
        auto audioBuffer = std::move(bufferQueue.front());
        bufferQueue.pop();
        queueLock.unlock();
        
        if (signalProcessor != nullptr) {
            AudioBuffer compressedBuffer;
            if (!signalProcessor->process(audioBuffer, compressedBuffer)) {
                hasError = true;
                return;
            }
            writeAudioBufferToFileStream(compressedBuffer, fileStream);
            
        } else {
           writeAudioBufferToFileStream(audioBuffer, fileStream);
        }
        
    }
}
void CAP::StreamWriter::writeAudioBufferToFileStream(const AudioBuffer &audioBuffer, std::ofstream& stream) {
    auto b = reinterpret_cast<const char *>(audioBuffer.getBuffer());
    stream.write(b, audioBuffer.size() * sizeof(*audioBuffer.getBuffer()));
    std::lock_guard<std::mutex> buffersWrittenLock(*buffersWrittenMutex);
    buffersWritten = buffersWritten + 1;
}


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

CAP::StreamWriter::StreamWriter(std::shared_ptr<File> file_, std::shared_ptr<SignalProcessor> cmp):
    file(std::move(file_)),
    signalProcessor(std::move(cmp)),
    waitMutex(new std::mutex()),
    queueMutex(new std::mutex()),
    queueConditionVariable(new std::condition_variable()),
    stopLoop(new std::atomic_bool()),
    killLoop(new std::atomic_bool()),
    hasError(new std::atomic_bool()),
    buffersWritten(new std::atomic_size_t()) {
        *stopLoop = false;
        *killLoop = false;
        *hasError = false;
        *buffersWritten = 0;
        
}

bool CAP::StreamWriter::isWriteable() const {
    return !(*killLoop || *stopLoop);
}

void CAP::StreamWriter::enqueue(CAP::AudioBuffer audioBuffer) {
    // 1. Secure our queue access.
    std::lock_guard<std::mutex> bufferLock(*queueMutex);
    
    
    // 2. Insert new buffer.
    bufferQueue.push(std::move(audioBuffer));
    
    // 3. Notify the loop thread.
    queueConditionVariable->notify_one();
}

void CAP::StreamWriter::stop() {
    *stopLoop = true;
    queueConditionVariable->notify_one();
}

void CAP::StreamWriter::kill() {
    *killLoop = true;
    std::unique_lock<std::mutex> queueLock(*queueMutex);
    bufferQueue = {};
    queueLock.unlock();
    queueConditionVariable->notify_one();
    
}

std::shared_ptr<std::atomic_size_t> CAP::StreamWriter::numberOfBuffersWritten() const {
    return buffersWritten;
}

std::size_t CAP::StreamWriter::numberOfSamplesWritten() {
    return samplesWritten;
}

void CAP::StreamWriter::start() {
    auto po = file->path();
    std::thread(&CAP::StreamWriter::runLoop, this).detach();
}

CAP::StreamWriter::~StreamWriter() {
}

std::size_t CAP::StreamWriter::queueSize() const {
    std::lock_guard<std::mutex> bufferLock(*queueMutex);
    return bufferQueue.size();
}

void CAP::StreamWriter::runLoop() {
    file->open();
    if (!file->isOpen()) {
        std::cerr << "file cannot be opened: " << file->path() << std::endl;
        *hasError = true;
    }
    std::unique_lock<std::mutex> loopLock(*waitMutex);
    while (true) {
        // 1. Wait for an event if the queue is empty.
        std::unique_lock<std::mutex> queueLock(*queueMutex);
        auto isEmpty = bufferQueue.empty();
        queueLock.unlock();
        
        if (*stopLoop) {
            if (isEmpty) {
                if (file->isOpen()) {
                    file->close();
                    signalProcessor->finalizeFileAtPath(file->path());
                }
                if (eventHandler != nullptr) {
                    eventHandler->streamWriterStopped();
                }
                return;
            }
        } else if (*killLoop) {
            if (file->isOpen()) {
                file->close();
            }
            if (eventHandler != nullptr) {
                eventHandler->streamWriterKilled();
            }
            return;
        }
        
        if (isEmpty || *hasError) {
            queueConditionVariable->wait(loopLock);
            continue;
        }
        
        
        // 3. Get the next buffer.
        queueLock.lock();
        auto audioBuffer = std::move(bufferQueue.front());
        bufferQueue.pop();
        queueLock.unlock();
        
        AudioBuffer compressedBuffer;
        if (!signalProcessor->process(audioBuffer, compressedBuffer) || !writeAudioBufferToFileStream(compressedBuffer)) {
            *hasError = true;
        }
    }
}
bool CAP::StreamWriter::writeAudioBufferToFileStream(const AudioBuffer &audioBuffer) {
    if (file->write(audioBuffer)) {
        *buffersWritten = *buffersWritten + 1;
        samplesWritten += audioBuffer.size();
        return true;
    }
    std::cerr << "error writing to file:" << file->path() << std::endl;
    return false;
}


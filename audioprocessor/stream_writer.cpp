//
//  StreamWriter.cpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/2/17.
//
//

#include "stream_writer.hpp"
#include <thread>
#include <iostream>

CAP::StreamWriter::StreamWriter(std::string filepath): filepath(filepath) {
}
CAP::StreamWriter::StreamWriter(std::string filepath, std::shared_ptr<Compressor> cmp): filepath(filepath), compressor(cmp) {
    
}


void CAP::StreamWriter::enqueue(std::vector<int16_t> buffer) {
    // 1. Secure our queue access.
    std::lock_guard<std::mutex> bufferLock(queueMutex);
    
    // 2. Insert new buffer.
    bufferQueue.push(buffer);
    
    // 3. Notify the loop thread.
    queueConditionVariable.notify_one();
}

std::future<void> CAP::StreamWriter::stop() {
    stopLoop = true;
    queueConditionVariable.notify_one();
    return stopPromise.get_future();
}

std::size_t CAP::StreamWriter::numberOfBuffersWritten() {
    std::lock_guard<std::mutex> buffersWrittenLock(buffersWrittenMutex);
    return buffersWritten;
}

void CAP::StreamWriter::start() {
    stopLoop = false;
    std::thread(&CAP::StreamWriter::runLoop, this).detach();
}

bool CAP::StreamWriter::hasCompressor() const {
    return compressor != nullptr;
}

void CAP::StreamWriter::runLoop() {
    std::ofstream fileStream(filepath, std::ofstream::binary | std::ofstream::app);
    std::unique_lock<std::mutex> loopLock(waitMutex);
    while (true) {
        // 1. Wait for an event if the queue is empty.
        std::unique_lock<std::mutex> queueLock(queueMutex);
        auto isEmpty = bufferQueue.empty();
        queueLock.unlock();
        
        if (isEmpty) {
            if (stopLoop) {
                stopPromise.set_value();
                return;
            } else {
                queueConditionVariable.wait(loopLock);
                continue;
            }
        }
        
        // 3. Get the next buffer.
        queueLock.lock();
        auto bufferVector = bufferQueue.front();
        bufferQueue.pop();
        queueLock.unlock();
        
        if (bufferVector.size() == 0) {
            continue;
        }
        
        if (compressor != nullptr) {
            try {
                bufferVector = compressor->compress(bufferVector);
                writeBufferToFileStream(bufferVector, fileStream);
            } catch (std::runtime_error re) {
                std::cerr << re.what() << std::endl;
            }
            
        } else {
            writeBufferToFileStream(bufferVector, fileStream);            
        }
        
    }
}
void CAP::StreamWriter::writeBufferToFileStream(std::vector<int16_t> &buffer, std::ofstream &stream) {
    stream.write(reinterpret_cast<char *>(buffer.data()), buffer.size() * sizeof(buffer[0]));
    std::lock_guard<std::mutex> buffersWrittenLock(buffersWrittenMutex);
    buffersWritten = buffersWritten + 1;
}


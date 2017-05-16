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
void CAP::StreamWriter::start() {
    stopLoop = false;
    std::thread(&CAP::StreamWriter::runLoop, this).detach();
}

void CAP::StreamWriter::runLoop() {
    std::ofstream fileStream(filepath, std::ofstream::binary | std::ofstream::app);
    std::unique_lock<std::mutex> loopLock(waitMutex);
    while (true) {
        // 1. Wait for an event if the queue is empty.
        std::unique_lock<std::mutex> bufferLock(queueMutex);
        auto isEmpty = bufferQueue.empty();
        bufferLock.unlock();
        
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
        bufferLock.lock();
        auto bufferVector = bufferQueue.front();
        bufferQueue.pop();
        bufferLock.unlock();
        
        if (bufferVector.size() == 0) {
            continue;
        }
        
        if (compressor != nullptr) {
            try {
                bufferVector = compressor->compress(bufferVector);
                fileStream.write(reinterpret_cast<char *>(bufferVector.data()), bufferVector.size() * sizeof(bufferVector[0]));
            } catch (std::runtime_error re) {
                std::cerr << re.what() << std::endl;
            }
            
        } else {
            fileStream.write(reinterpret_cast<char *>(bufferVector.data()), bufferVector.size() * sizeof(bufferVector[0]));
        }
        
    }
}

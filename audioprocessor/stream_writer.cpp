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
CAP::StreamWriter::StreamWriter(const StreamWriter& other) {
    
}


void CAP::StreamWriter::enqueue(std::vector<int16_t>& buffer) {
    // 1. Secure our queue access.
    std::lock_guard<std::mutex> taskLock(queueMutex);
    
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
    std::ofstream fileStream(filepath, std::ios_base::binary | std::ios_base::out);
    std::unique_lock<std::mutex> loopLock(waitMutex);
    while (true) {
        // 1. Wait for an event if the queue is empty.
        std::unique_lock<std::mutex> bufferLock(queueMutex);
        auto queueSize = bufferQueue.size();
        bufferLock.unlock();
        
        if (queueSize == 0 && !stopLoop) {
            queueConditionVariable.wait(loopLock);
        } else if (queueSize == 0 && stopLoop) {
            stopPromise.set_value();
            return;
        }        
        
        // 3. Get the next buffer.
        bufferLock.lock();
        auto buffer = bufferQueue.front();
        bufferQueue.pop();
        bufferLock.unlock();
        
        if (compressor != nullptr) {
            try {
                buffer = compressor->compress(buffer);
            } catch (std::runtime_error re) {
                std::cerr << re.what() << std::endl;
            }
            
        }
        
        // 4. write
        fileStream.write(reinterpret_cast<char *>(buffer.data()), buffer.size() * sizeof(int16_t));
    }
}

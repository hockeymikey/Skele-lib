//
//  StreamWriter.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/2/17.
//
//

#ifndef StreamWriter_hpp
#define StreamWriter_hpp


#include "compressor.hpp"
#include <string>
#include <fstream>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <memory>
#include <future>
namespace CAP {
    /**
     Base class for writing raw audio samples to the file specified by the client.
     **/
    class StreamWriter {
    public:
        /**
         Constructor
         
         @param filepath
            The file path where this object is gonna write samples to.
         **/
        StreamWriter(std::string filepath);
        
        /**
         Constructor
         
         @param filepath
            The file path where this object is gonna write samples to.
         @param compressor
            Compressor to use before writing to file
         **/
        StreamWriter(std::string filepath, std::shared_ptr<Compressor> compressor);
        
        /**
         Copy constructor
         @param other
            Other stream writer object
         **/
        
        
        
        /**
         Places buffer in the queue for asynchronous processing
         
         @param     buffer
            Array of samples, makes copy internally
         **/
        void enqueue(std::vector<int16_t> buffer);
        
        /**
         Stops asynchronous processing.
         **/
        std::future<void> stop();
        
        /**
         Starts asynchronous processing
         **/
        void start();
        
        /**
         Compressor is attached or not
         @return  boolean
         **/
        bool hasCompressor() const;
        
        std::size_t numberOfBuffersWritten();
        
    protected:
    private:
        StreamWriter(const StreamWriter& other);
        std::string filepath;
        std::promise<void> stopPromise;
        std::queue<std::vector<int16_t>> bufferQueue;
        StreamWriter();
        StreamWriter operator=(const StreamWriter&);
        std::mutex waitMutex;
        std::mutex queueMutex;
        std::condition_variable queueConditionVariable;
        bool stopLoop = false;
        void runLoop();
        std::shared_ptr<Compressor> compressor;
        std::mutex buffersWrittenMutex;
        std::size_t buffersWritten = 0;
        void writeBufferToFileStream(std::vector<int16_t>&, std::ofstream&);
    };
}

#endif /* StreamWriter_hpp */

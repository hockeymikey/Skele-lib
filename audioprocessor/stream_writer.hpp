//
//  StreamWriter.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/2/17.
//
//

#ifndef StreamWriter_hpp
#define StreamWriter_hpp

#include "audio_buffer.hpp"
#include "signal_processor.hpp"
#include <string>
#include <fstream>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <memory>
#include <future>
#include <array>
namespace CAP {
    
    /**
     Base class for writing raw audio samples to the file specified by the client.
     **/
    class StreamWriter {
    public:
        
        /**
         Let compiler generate move assignment operator
         **/
        StreamWriter& operator=(StreamWriter&& other) = default;
        
        /**
         Move constructor: use compiler generated one (it knows what it's doing)
         **/
        StreamWriter(StreamWriter&&) = default;
        
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
        StreamWriter(std::string filepath, std::unique_ptr<SignalProcessor> signalProcessor);
        
        /**
         Copy constructor
         @param other
            Other stream writer object
         **/
        
        
        
        /**
         Places buffer in the queue for asynchronous processing, makes copy internally
         
         @param     buffer
            Array of samples
         **/
        void enqueue(AudioBuffer audioBuffer);
        
        /**
         Stops processing. Based on the flag supplied, will wait till queue is empty or not.
         **/
        std::future<void> stopGracefully(bool gracefully);
        
        
        /**
         Starts asynchronous processing
         **/
        void start();
        
        /**
         Signal processor is attached or not
         @return  boolean
         **/
        bool hasSignalProcessor() const;
        
        /**
         Destructor
         **/
        ~StreamWriter();
        
        std::size_t queueSize();
        
        std::size_t numberOfBuffersWritten();
        /**
         Dont allow copy since the object uses mutexes
         **/
        StreamWriter(const StreamWriter& other) = delete;
        
        /**
         Don't allow object with default field values
         **/
        StreamWriter() = delete;
        
        /**
         Don't allow copy by assignment since the object uses mutexes
         **/
        StreamWriter operator=(const StreamWriter&) = delete;
        
    protected:
    private:        
        std::string filepath;
        std::promise<void> stopPromise;
        std::queue<AudioBuffer> bufferQueue;
        
        bool stopLoop = false;
        std::unique_ptr<SignalProcessor> signalProcessor; //pointer used to allow polymorphism
        std::size_t buffersWritten = 0;
        
        //pointer to synchronisation infrastructure to allow compiler generate move constructor
        std::unique_ptr<std::mutex> waitMutex;
        std::unique_ptr<std::mutex> queueMutex;
        std::unique_ptr<std::mutex> buffersWrittenMutex;
        std::unique_ptr<std::condition_variable> queueConditionVariable;
        
        //private methods
        void writeAudioBufferToFileStream(const AudioBuffer &audioBuffer, std::ofstream&);
        void runLoop();
    };
}

#endif /* StreamWriter_hpp */

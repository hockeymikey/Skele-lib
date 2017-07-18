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
#include "system_file.hpp"

#include <string>
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
         
         @param file
            Reference to file object
         @param compressor
            Compressor to use before writing to file
         **/
        StreamWriter(std::shared_ptr<File> file, std::shared_ptr<SignalProcessor> signalProcessor);
        
        
        /**
         Places buffer in the queue for asynchronous processing, makes copy internally
         
         @param     buffer
            Array of samples
         **/
        void enqueue(AudioBuffer audioBuffer);
        
        /**
         Stops processing.
         
         @return future
         **/
        std::future<void> stop();
        
        /**
         Ignores queued buffers, and returns from the thread
         
         @return future
         **/
        std::future<void> kill();
        
        
        /**
         Starts asynchronous processing
         **/
        void start();
                
        
        /**
         Destructor
         **/
        ~StreamWriter();
        
        /**
         The size of queue of audio buffers
         
         @return size_t
         **/
        std::size_t queueSize() const;
        
        /**
         Keeps track of number of audio buffers that have been written successfully
         
         @return atomic_size_t
            pointer to atomic size_t
         **/
        std::shared_ptr<std::atomic_size_t> numberOfBuffersWritten() const;
        
        /**
         Returns number of samlpes stream writer wrote. Doesnt need to be atomic
         **/
        std::size_t numberOfSamplesWritten();
        
        /**
         Informs whether writer has been scheduled for stop/kill.
         **/
        bool isWriteable() const;
        
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
        
        std::shared_ptr<File> file;
        
        std::promise<void> stopPromise;
        std::promise<void> killPromise;
        std::queue<AudioBuffer> bufferQueue;
        
        std::shared_ptr<SignalProcessor> signalProcessor; //pointer used to allow polymorphism
        
        
        //pointer to synchronisation infrastructure to allow compiler generate move constructor
        std::unique_ptr<std::mutex> waitMutex;
        std::unique_ptr<std::mutex> queueMutex;
        std::unique_ptr<std::condition_variable> queueConditionVariable;
        std::unique_ptr<std::atomic_bool> stopLoop;
        std::unique_ptr<std::atomic_bool> killLoop;
        std::unique_ptr<std::atomic_bool> hasError;
        std::shared_ptr<std::atomic_size_t> buffersWritten;
        
        std::size_t samplesWritten = 0;
        
        //private methods
        bool writeAudioBufferToFileStream(const AudioBuffer &audioBuffer);
        void runLoop();
    };
}

#endif /* StreamWriter_hpp */

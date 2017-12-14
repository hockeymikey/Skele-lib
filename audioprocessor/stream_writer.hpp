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
    
    class AbstractStreamWriterObserver {
    public:
        AbstractStreamWriterObserver(const AbstractStreamWriterObserver& other) = delete;
        AbstractStreamWriterObserver() = default;
        AbstractStreamWriterObserver(AbstractStreamWriterObserver&&) = default;
        
        virtual void streamWriterStopped() = 0;
        virtual void streamWriterKilled() = 0;
        virtual ~AbstractStreamWriterObserver() {
            
        };

        //            virtual void operator()() final {
        //            for(int i = 0; i < 10000; i++)
        //                std::cout<<"Display Thread Executing"<<std::endl;
        //            }
    };
    
    /**
     Base class for writing raw audio samples to the file specified by the client.
     **/
    class StreamWriter {
    public:
        
        std::shared_ptr<AbstractStreamWriterObserver> streamWriterObserver;        
        
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
         Gets filepath of File that was passed to this object during construction
         
         @return string
         **/
        std::string getFilePath() const;
        
        
        /**
         Places buffer in the queue for asynchronous processing, makes copy internally
         
         @param     buffer
            Array of samples
         **/
        void enqueue(AudioBuffer audioBuffer);
        
        /**
         Stops processing (async call).
         
         @return future
         **/
        void stop();
        
        /**
         Ignores queued buffers, and returns from the thread (async call)
         
         @return future
         **/
        void kill();
        
        
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
        std::shared_ptr<std::atomic_size_t> numberOfSamplesWritten() const;
        
        
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
        
        const std::shared_ptr<File> file;
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
        
        std::shared_ptr<std::atomic_size_t> samplesWritten;
        
        //private methods
        bool writeAudioBufferToFileStream(const AudioBuffer &audioBuffer);
        bool writeRawBufferToFileStream(unsigned char *rawBuffer, int nbytes);
        void runLoop();
    };
}

#endif /* StreamWriter_hpp */

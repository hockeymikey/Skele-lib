
#ifndef audioprocessor_hpp
#define audioprocessor_hpp

#include "stream_writer.hpp"
#include "audio_buffer.hpp"
#include <fstream>
#include <vector>

namespace CAP {
    
    
    
    /**
     Orchestrates audio sample writing
     **/
    class AudioProcessor {
        
    public:
        
        static const std::int8_t StreamWriterCapacity = 5;
        
        enum class ProcessResult {
            Success,
            PriorityWriterError,
            NonPriorityWriterError
        };
        
        /**
         Constructor. Stream writer at index 0 has highest priority. AudioProcessor takes ownership of the stream writers. Outside access not possible
         
         @param streamWriters
            An array of stream writers audio processor will be delegating write operation to.
         **/
        AudioProcessor(StreamWriter * const streamWriters, std::int8_t streamWriterCount);
        
        /**
         Passes audio buffer to stream writers to process.
                  
         @param samples
            Pointer to array of samples
         @param nsamples
            Number of samples in array
         @return ProcessResult      
            If result is priority writer error, the method will discard the samples.
            If the result is non-priority writer error, priority will keep writing non-priority samples will be discarded
         **/
        ProcessResult processBuffer(std::int16_t *samples, std::size_t nsamples);
        
        
        /**
         Stops listening for incoming samples. Blocks while waiting for the queues to be emptied.
         **/
        void stop();
        
        
        /**
         No default constructor allowed
         **/
        AudioProcessor() = delete;
        
        /**
         No copy constructor allowed
         **/
        AudioProcessor(const AudioProcessor&) = delete;
        
        /**
         No copy by assignment allowed
         **/
        AudioProcessor operator=(const AudioProcessor&) = delete;
        
        /**
         Must be run on separate thread
         **/
        void scheduleAudioPostProcessing(std::function<void ()>);
        
    protected:
    
    private:
        
        StreamWriter * const streamWriters;
        std::int8_t streamWriterCount = 0;
        std::int8_t streamWriterKillThreshold = 100; //number of buffers
        
    };
}

#endif /* audioprocessor_hpp */

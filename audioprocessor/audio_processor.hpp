
#ifndef audioprocessor_hpp
#define audioprocessor_hpp

#include "stream_writer.hpp"
#include "audio_buffer.hpp"
#include "circular_queue.hpp"
#include <array>

namespace CAP {
    
    /**
     Orchestrates audio sample writing
     **/
    class AudioProcessor {
        
    public:
        
        enum class Status {
            Success,
            PriorityWriterError,
            NonPriorityWriterError,
            FullBuffer
        };
        
        
        /**
         Constructor.
         
         @param circularQueue
            Pointer to cicrular queue
         @param recommendedDelay
            Processing delay
         **/
        AudioProcessor(std::unique_ptr<AbstractCircularQueue> circularQueue);
        
        
        void startHighlight(std::vector<std::shared_ptr<StreamWriter>> streamWriters, std::uint8_t recommendedDelayInSeconds);
        
        /**
         @param samples
            Pointer to array of samples
         @param nsamples
            Number of samples in array
         
         @return ProcessResult
            If result is priority writer error, the method will discard the samples.
            If the result is non-priority writer error, priority will keep writing non-priority samples will be discarded
         **/
        CAP::AudioProcessor::Status processSamples(std::int16_t *samples, std::size_t nsamples);
        
        
        /**
         Stops listening for incoming samples. Blocks while waiting for the queues to be emptied.
         
         @param callback
            Callback function to call when all stream writers are done
         @return future
         **/
        void stopHighlight();
        
        
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
         Redirects audio processing to newly-provided stream writers, calls callback when old streamwriters are caught up
         
         @param priority
            Vector of pointers to stream writers audio processor will be delegating write operation to.
         
         @param callback
            Callback function to call when all stream writers are done
         **/
//        void schedulePostProcess(std::vector<std::shared_ptr<StreamWriter>> sw, std::function<void ()> callback);
        
    
        
    protected:
    
    private:
        
        struct StreamWriterBundle {
            bool isPostProcessing = false;
            StreamWriterBundle(StreamWriterBundle&&) = default;
            StreamWriterBundle operator=(const StreamWriterBundle&) = delete;
            StreamWriterBundle& operator=(StreamWriterBundle&& other) = default;
            StreamWriterBundle(const StreamWriterBundle& other) = delete;
            StreamWriterBundle() = delete;
            StreamWriterBundle(std::vector<std::shared_ptr<StreamWriter>> sw, std::uint8_t recommendedDelay_): streamWriters(sw), recommendedDelayInSeconds(recommendedDelay_) {}
            std::vector<std::shared_ptr<StreamWriter>> streamWriters;
            std::uint8_t recommendedDelayInSeconds;
        };
        
        std::vector<std::shared_ptr<StreamWriterBundle>> streamWriterBundles;
        
        
        std::unique_ptr<AbstractCircularQueue> circularQueue;
        
        std::int8_t streamWriterKillThreshold = 100; //number of buffers
        
//        void schedulePostProcess(std::function<void ()> callback);
        CAP::AudioProcessor::Status enqueueSamples(std::int16_t *samples, std::size_t nsamples);
        
    };
}

#endif /* audioprocessor_hpp */

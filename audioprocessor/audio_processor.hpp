
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
        
        /**
         Offloads samples that are in circular queue into streamwriters. ]
         
         @param streamWriters
            List of stream writers
         @param recommendedDelayInSeconds
            Wait to write to stream writers until circular queue reaches this paramater
         **/
        void startHighlight(std::vector<std::shared_ptr<StreamWriter>> streamWriters, std::uint8_t recommendedDelayInSeconds);
        
        /**
         Since audio processor owns circular queue, publish queue size to outside world
         **/
        std::size_t circularQueueSize();
        
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
        void stopHighlight(bool flushCircularQueue);
        
        
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
        
       
        
    
        
    protected:
    
    private:
        
        struct StreamWriterBundle {
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
        
        std::mutex circularQueueMutex;
        std::mutex bundlesMutex;
        
//        void schedulePostProcess(std::function<void ()> callback);
        CAP::AudioProcessor::Status enqueueSamples(std::int16_t *samples, std::size_t nsamples);
        void flushCircularQueue(std::size_t flushLimitInSamples);
        
    };
}

#endif /* audioprocessor_hpp */

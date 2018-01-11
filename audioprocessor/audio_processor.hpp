
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
            Success, //audio processor was able to process samples with no issues
            FullBuffer, //audio processor buffer is full, can't keep up taking off samples off of buffer
            PriorityStreamWriterKilledDueToOverflow, //priority writer has issues (can't open file, cant write to file, slow I/O), processor kills it
            NonPriorityStreamWriterKilledDueToOverflow, //non-priority writer has issues (can't open file, cant write to file, slow I/O, priority writer killed), processor kills it
            StreamWritersNotRunning, //all writers cant accept new samples because that have been killed
            NonPriorityStreamWriterNotRunning //can't accept new samples, cause it has been killed
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
         Offloads samples that are in circular queue into streamwriters.
         
         @param streamWriters
            List of stream writers
         @param recommendedDelayInSamples
            Wait to write to stream writers until circular queue size reaches recommendedDelayInSamples
         @return
             Number of seconds of buffered audio that will be part of this highlight
         **/
        double startHighlight(std::vector<std::shared_ptr<StreamWriter>> streamWriters, std::uint32_t recommendedDelayInSamples);
        
        
        
        /**
         Since audio processor owns circular queue, publish queue size in samples to outside world.
         **/
        std::size_t circularQueueSize();
                
        
        /**
         @param samples
            Pointer to array of samples
         @param nsamples
            Number of samples in array
         
         @return Status

         **/
        CAP::AudioProcessor::Status processSamples(std::int16_t *samples, std::size_t nsamples);
        
        
        /**
         Stops consuming new samples and passing to the stream writers
                  
         @param flushCircularQueue
            Instructs this object to pass buffered samples to current stream writer bundle
         **/
        void stopHighlight(bool flushCircularQueue);
        
        /**
         Needed to see whether post processing can be started on a file with path specified.
         The file that is open is being processed, caller shoudn't mess with it yet.
         
         @param filepath
             Path to the file of interest
         **/
        bool isFileBeingProcessedAtFilepath(std::string filepath);
        
        
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
            StreamWriterBundle(std::vector<std::shared_ptr<StreamWriter>> sw, std::uint32_t recommendedDelay_): streamWriters(sw), recommendedDelayInSamples(recommendedDelay_) {}
            std::vector<std::shared_ptr<StreamWriter>> streamWriters;
            std::uint32_t recommendedDelayInSamples;
            std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
        };
        
        std::vector<std::shared_ptr<StreamWriterBundle>> streamWriterBundles;
        
        
        std::unique_ptr<AbstractCircularQueue> circularQueue;
        
        std::int8_t streamWriterKillThreshold = 100; //number of buffers
        
        std::mutex circularQueueMutex;
        std::mutex bundlesMutex;
        
        CAP::AudioProcessor::Status enqueueSamples(std::int16_t *samples, std::size_t nsamples);
        void flushCircularQueue(std::size_t flushLimitInSamples);
    };
}

#endif /* audioprocessor_hpp */

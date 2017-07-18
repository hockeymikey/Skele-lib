
#ifndef audioprocessor_hpp
#define audioprocessor_hpp

#include "stream_writer.hpp"
#include "audio_buffer.hpp"
#include <array>

namespace CAP {
    
    
    
    /**
     Orchestrates audio sample writing
     **/
    class AudioProcessor {
        
    public:
        
        enum class ProcessResult {
            Success,
            PriorityWriterError,
            NonPriorityWriterError
        };
        
        /**
         Constructor.
         
         @param sw
            Vector of pointers to stream writers audio processor will be delegating write operation to.
        
         **/
        AudioProcessor(std::vector<std::shared_ptr<StreamWriter>> sw);
        
        
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
         
         @param callback
            Callback function to call when all stream writers are done
         @return future
         **/
        std::future<void> stop(std::function<void ()> callback);
        
        
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
        void schedulePostProcess(std::vector<std::shared_ptr<StreamWriter>> sw, std::function<void ()> callback);
        
        
//        std::shared_ptr<StreamWriter> getCurrentPriorityStreamWriter();
        
    protected:
    
    private:
        
        struct StreamWriterBundle {
            bool isPostProcessing = false;
            StreamWriterBundle(StreamWriterBundle&&) = default;
            StreamWriterBundle operator=(const StreamWriterBundle&) = delete;
            StreamWriterBundle& operator=(StreamWriterBundle&& other) = default;
            StreamWriterBundle(const StreamWriterBundle& other) = delete;
            StreamWriterBundle() = delete;
            StreamWriterBundle(std::vector<std::shared_ptr<StreamWriter>> sw): streamWriters(sw) {}
            
            std::vector<std::shared_ptr<StreamWriter>> streamWriters;
        };
        
        std::vector<std::shared_ptr<StreamWriterBundle>> streamWriterBundles;
        
        
        std::int8_t streamWriterKillThreshold = 100; //number of buffers
        
        void schedulePostProcess(std::function<void ()> callback);
//        StreamWriter * getPriorityStreamWriterFromBundle(StreamWriterBundle *);
        
    };
}

#endif /* audioprocessor_hpp */

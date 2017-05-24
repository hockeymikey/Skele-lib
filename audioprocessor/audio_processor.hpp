
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
        
        enum class ProcessResult {
            Success,
            PriorityWriterError,
            NonPriorityWriterError
        };
        
        /**
         Constructor. Stream writer at index 0 has highest priority. 
         
         @param streamWriters
            A vector of stream writers audio processor will be delegating write operation to.
         **/
        AudioProcessor(std::vector<StreamWriter> &streamWriters);
        
        /**
         Passes audio buffer to stream writers to process.
                  
         @param samples
            Pointer to array of samples
         @param nsamples
            Number of samples in array
         @return ProcessResult            
         **/
        ProcessResult process(std::int16_t *samples, std::size_t nsamples);
        
        
        /**
         Stops listening for incoming samples. Blocks while waiting for the queues to be emptied.
         **/
        void stop();
        
        /**
         Return number of stream writers. This is mainly for testing purposes.
         
         @return number of stream wrtiers
         **/
        std::size_t getNumberOfStreamWriters() const {
            return streamWriters.size();
        }
        
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
        std::vector<StreamWriter> streamWriters;
        std::size_t streamWriterKillThreshold = 100; //number of buffers
        
    };
}

#endif /* audioprocessor_hpp */

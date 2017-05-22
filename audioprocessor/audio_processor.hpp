
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
        /**
         Constructor. Stream writer at index 0 has highest priority. 
         
         @param streamWriters
            A vector of stream writers audio processor will be delegating write operation to.
         **/
        AudioProcessor(std::vector<StreamWriter> &streamWriters);
        
        /**
         Passes audio buffer to stream writers to process.
                  
         @param audioBuffer
            Audio buffer
         **/
        void process(std::int16_t *samples, std::size_t nsamples);
        
        
        /**
         Stops listening for incoming samples  
         **/
        void stop();
        
        AudioProcessor() = delete;
        AudioProcessor(const AudioProcessor&) = delete;
        AudioProcessor operator=(const AudioProcessor&) = delete;
        
        /**
         Must be run on separate thread
         **/
        void scheduleAudioPostProcessing(std::function<void ()>);
        
    protected:
    
    private:
        std::vector<StreamWriter> streamWriters;
        size_t streamWriterKillThreshold = 1000;
        
    };
}

#endif /* audioprocessor_hpp */

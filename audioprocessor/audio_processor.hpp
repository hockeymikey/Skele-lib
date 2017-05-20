
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
        void process(const AudioBuffer& audioBuffer);
        
        
        /**
         Stops listening for incoming samples  
         **/
        void stop();
        
        /**
         Destructor
         **/
        ~AudioProcessor();
        
        /**
         Must be run on separate thread
         **/
        void scheduleAudioPostProcessing(std::function<void ()>);
        
    protected:
    
    private:
        std::vector<StreamWriter> streamWriters;
        
        AudioProcessor();
        AudioProcessor(const AudioProcessor&);
        AudioProcessor operator=(const AudioProcessor&);
    };
}

#endif /* audioprocessor_hpp */

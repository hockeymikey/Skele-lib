
#ifndef audioprocessor_hpp
#define audioprocessor_hpp

#include "stream_writer.hpp"
#include <fstream>
#include <vector>


namespace CAP {
    /**
     Orchestrates audio sample writing
     **/
    class AudioProcessor {
        
    public:
        /**
         Constructor
         
         @param streamWriters
            A list of stream writers audio processor will be delegating write operation to.
         **/
        AudioProcessor(std::vector<StreamWriter> streamWriters);
        
        /**
         Accepts raw samples and passes to available stream writers for processing.
         Even though it is convenient to use vectors in the implementation, it might be difficult to set up parameter in client code.
         If that turns out to be the case, will switch out to arrays.
         
         @param samples
            A reference to a list of raw samples 
         **/
        void writeAudioSamples(std::vector<int16_t>& samples);
        
        /**
         Stops stream writers  
         **/
        void close();
    protected:
    
    private:
        std::vector<StreamWriter> streamWriters;
        
        AudioProcessor();
        AudioProcessor(const AudioProcessor&);
        AudioProcessor operator=(const AudioProcessor&);
    };
}

#endif /* audioprocessor_hpp */

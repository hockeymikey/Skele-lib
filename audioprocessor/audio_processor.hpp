
#ifndef audioprocessor_hpp
#define audioprocessor_hpp

#include "stream_writer.hpp"
#include <fstream>
#include <vector>


namespace CAP {
    class AudioProcessor {
        
    public:
        AudioProcessor(std::vector<StreamWriter>);
        void writeAudioSamples(std::vector<int16_t>);
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


#ifndef audioprocessor_hpp
#define audioprocessor_hpp

#include "StreamWriter.hpp"
#include <fstream>
#include <vector>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

namespace CAP {
    class AudioProcessor {
        
    public:
        AudioProcessor(std::vector<StreamWriter>);
        void writeAudioSamples(std::vector<int16_t>);
        void close();
    protected:
    
    private:
        std::vector<StreamWriter> streamWriters;
//        boost::filesystem::path rawAudioPath;
//        boost::filesystem::path mp3AudioPath;
//        std::ofstream rawAudioFileStream;
////        boost::filesystem::ofstream compressedFileStream;
        
        AudioProcessor();
        AudioProcessor(const AudioProcessor&);
        AudioProcessor operator=(const AudioProcessor&);
    };
}

#endif /* audioprocessor_hpp */

//
//  CompressedStreamWriter.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/3/17.
//
//

#ifndef CompressedStreamWriter_hpp
#define CompressedStreamWriter_hpp

#include "stream_writer.hpp"
#include "lame.h"
#include <vector>
namespace CAP {
    /**
     Compresses audio samples to MP3 before writing compressed frames to file.
     **/
    class CompressedStreamWriter: public StreamWriter {
    public:
        
        enum class Status {
            Success,
            CompressorInitializationError,
            CompressorBufferTooSmall,
            CompressorMemoryAllocationError,
            CompressorPsychoAcousticError
        };
        
        /**
         Constructor
         
         @param filepath
            Path to file where compressed frames will be written to.
         @param compressionQuality
            Compression quality. 0 is best (very slow), 9 is worst
         **/
        CompressedStreamWriter(std::string filepath, int compressionQuality);
        
        
        /**
         Overrides parent method, gracefully ends compression before closing the file. 
         **/
        void close();
    protected:
        
    private:
        CompressedStreamWriter(std::string);
        lame_t lame;
        int compressionQuality;
        bool compressorInitialized = false;
        Status write(std::vector<int16_t> buffer);
    };
}

#endif /* CompressedStreamWriter_hpp */

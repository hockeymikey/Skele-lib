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
namespace CAP {
    
    class CompressedStreamWriter: public StreamWriter {
    public:
        
        enum class Status {
            Success,
            CompressorInitializationError,
            CompressorBufferTooSmall,
            CompressorMemoryAllocationError,
            CompressorPsychoAcousticError
        };
        
        CompressedStreamWriter(std::string, int);
        Status write(int16_t samples[], int size);
        void close();
    protected:
        
    private:
        CompressedStreamWriter(std::string);
        lame_t lame;
        bool compressorInitialized = false;
    };
}

#endif /* CompressedStreamWriter_hpp */

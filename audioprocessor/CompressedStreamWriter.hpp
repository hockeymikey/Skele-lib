//
//  CompressedStreamWriter.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/3/17.
//
//

#ifndef CompressedStreamWriter_hpp
#define CompressedStreamWriter_hpp

#include "StreamWriter.hpp"

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
    protected:
        
    private:
        int compressionQuality;
    };
}

#endif /* CompressedStreamWriter_hpp */

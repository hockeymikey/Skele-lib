//
//  CompressedStreamWriter.cpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/3/17.
//
//

#include "compressed_stream_writer.hpp"
#include <lame/lame.h>

namespace CAP {
    CompressedStreamWriter::CompressedStreamWriter(std::string filePath, int quality): StreamWriter(filePath), compressionQuality(quality) {
        
    }
    CompressedStreamWriter::Status CompressedStreamWriter::write(int16_t samples[], int nsamples) {
        unsigned char compressedBuffer[nsamples];
        
        lame_t lame = lame_init();
        lame_set_quality(lame, compressionQuality);
        if (lame_init_params(lame) == -1) {
            return Status::CompressorInitializationError;
        }
        int output = lame_encode_buffer(lame, samples, samples, nsamples / 2, compressedBuffer, nsamples * sizeof(unsigned char));
        switch (output) {
            case -1:
                return Status::CompressorBufferTooSmall;
            case -2:
                return Status::CompressorMemoryAllocationError;
            case -3:
                abort(); //should not happen since lame init called above
            case -4:
                return Status::CompressorPsychoAcousticError;
            case 0:
                 return Status::Success;
            default:
                break;
        }
        
        StreamWriter::write(reinterpret_cast<int16_t *>(compressedBuffer), output * sizeof(unsigned char));
        return Status::Success;
    }
    
}

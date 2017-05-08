//
//  CompressedStreamWriter.cpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/3/17.
//
//

#include "compressed_stream_writer.hpp"


namespace CAP {
    CompressedStreamWriter::CompressedStreamWriter(std::string filePath, int quality): StreamWriter(filePath) {
        lame = lame_init();
        lame_set_quality(lame, quality);
    }
    CompressedStreamWriter::Status CompressedStreamWriter::write(int16_t samples[], int nsamples) {
        unsigned char compressedBuffer[nsamples];
        
        if (!compressorInitialized) {
            compressorInitialized = true;
            if (lame_init_params(lame) == -1) {
                return Status::CompressorInitializationError;
            }
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
    
    void CompressedStreamWriter::close() {
        lame_close(lame);
        StreamWriter::close();
    }
    
}

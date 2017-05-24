//
//  mp3_compressor.cpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/14/17.
//
//

#include "mp3_compressor.hpp"
#include <cstdlib>

CAP::Mp3Compressor::Mp3Compressor(int compressionQuality):  Compressor() {
    lame = lame_init();
    if (!lame) {
        throw std::runtime_error("Cannot instantiate lame");
    }
    if (lame_set_quality(lame, compressionQuality) == -1 || lame_init_params(lame) == -1) {
        throw std::runtime_error("Cannot initialize lame");
    }
}

CAP::Mp3Compressor::~Mp3Compressor() {
    lame_close(lame);
}

std::vector<int16_t> CAP::Mp3Compressor::compress(std::vector<int16_t> const& buffer) {
    unsigned char compressedBuffer[buffer.size()];
    
    int output = lame_encode_buffer(lame, buffer.data(), buffer.data(), buffer.size() / 2, compressedBuffer, buffer.size() * sizeof(unsigned char));
    switch (output) {
        case -1:
            throw std::runtime_error("Lame buffer is too small");
        case -2:
            throw std::runtime_error("Lame memory allocation issue");
        case -3:
            abort(); //should not happen since lame init called above
        case -4:
            throw std::runtime_error("Lame psychoacoustic error");
        case 0:
        default:
            break;
    }
    
    auto compressedPointer = reinterpret_cast<int16_t *>(compressedBuffer);
    
    return std::vector<int16_t>(compressedPointer, compressedPointer + output );
};

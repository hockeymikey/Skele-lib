//
//  mp3_compressor.cpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/14/17.
//
//

#include "mp3_compressor.hpp"
#include <cstdlib>

CAP::Mp3Compressor::Mp3Compressor(int compressionQuality, int sampleRate):  CAP::SignalProcessor() {
    lame = lame_init();
    if (!lame) {
        throw std::runtime_error("Cannot instantiate lame");
    }
    if (lame_set_in_samplerate(lame, sampleRate) == -1 || lame_set_quality(lame, compressionQuality) == -1 || lame_init_params(lame) == -1) {
        throw std::runtime_error("Cannot initialize lame");
    }
}

CAP::Mp3Compressor::~Mp3Compressor() {
    lame_close(lame);
}

CAP::AudioBuffer CAP::Mp3Compressor::process(AudioBuffer audioBuffer) {
    unsigned char compressedBuffer[audioBuffer.size()];
    int output = lame_encode_buffer(lame, audioBuffer.getSamples(), audioBuffer.getSamples(), audioBuffer.size() / 2, compressedBuffer, audioBuffer.size());
 
    switch (output) {
        case -1:
            throw std::runtime_error("Lame buffer is too small");
        case -2:
            throw std::runtime_error("Lame memory allocation issue");
        case -3:
            std::abort(); //should not happen since lame init called above
        case -4:
            throw std::runtime_error("Lame psychoacoustic error");
        case 0:
        default:
            break;
    }
    
    auto samples = reinterpret_cast<int16_t *>(compressedBuffer);
    auto size = output;
        
    return AudioBuffer(samples, size);
};

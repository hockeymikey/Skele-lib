//
//  mp3_compressor.cpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/14/17.
//
//

#include "mp3_compressor.hpp"
#include <cstdlib>
#include <iostream>
#include "lame.h"

CAP::Mp3Compressor::Mp3Compressor(int compressionQuality, int sampleRate):  CAP::SignalProcessor() {
    lame = lame_init();
    if (!lame) {
        throw std::runtime_error("Cannot instantiate lame");
    }
    
    if (lame_set_mode(lame, MPEG_mode::MONO) == -1 ||
        lame_set_num_channels(lame, 1) == -1 ||
        lame_set_in_samplerate(lame, sampleRate) == -1 ||
        lame_set_quality(lame, compressionQuality) == -1 ||
        lame_init_params(lame) == -1) {
        
        throw std::runtime_error("Cannot initialize lame");
    }
}

CAP::Mp3Compressor::~Mp3Compressor() {
    lame_close(lame);
}

void CAP::Mp3Compressor::finalizeFileAtPath(std::string path) const {
    
}

bool CAP::Mp3Compressor::process(const AudioBuffer& audioBuffer, unsigned char *rawBuffer, int& nbytes) const {
    
    int output = lame_encode_buffer(lame, audioBuffer.getBuffer(), nullptr, audioBuffer.size(), rawBuffer, nbytes);
    
    switch (output) {
        case -1:
            std::cerr << "Lame buffer is too small" << std::endl;
            return false;
        case -2:
            std::cerr << "Lame memory allocation issue" << std::endl;
            return false;
        case -3:
            std::abort(); //should not happen since lame init called above
        case -4:
            std::cerr << "Lame psychoacoustic error" << std::endl;
            return false;
        case 0:
        default:
            break;
    }
    
    nbytes = output;
    return true;
}

bool CAP::Mp3Compressor::process(const AudioBuffer& audioBuffer, AudioBuffer& processed) const {
    return true;
}

bool CAP::Mp3Compressor::usesRawBufferForProcessing() const {
    return true;
}

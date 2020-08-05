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
#include <stdexcept>
#include <jni.h>

#define BUFFER_SIZE 8192
#define be_short(s) ((short) ((unsigned short) (s) << 8) | ((unsigned short) (s) >> 8))

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

CAP::Mp3Compressor::Mp3Compressor(int compressionQuality, int sampleRate, int numChannels, int outBitrate, int outSampleRate):  CAP::SignalProcessor() {
    lame = lame_init();
    if (!lame) {
        throw std::runtime_error("Cannot instantiate lame");
    }

    if (lame_set_mode(lame, MPEG_mode::MONO) == -1 ||
        lame_set_num_channels(lame, numChannels) == -1 ||
        lame_set_in_samplerate(lame, sampleRate) == -1 ||
        lame_set_quality(lame, compressionQuality) == -1 ||
        lame_init_params(lame) == -1 ||
        lame_set_brate(lame, outBitrate) == -1 ||
        lame_set_out_samplerate(lame, outSampleRate) == -1) {

        throw std::runtime_error("Cannot initialize lame");
    }
}

CAP::Mp3Compressor::~Mp3Compressor() {
    lame_close(lame);
}

void CAP::Mp3Compressor::finalizeFileAtPath(std::string path) {
    
}

void CAP::Mp3Compressor::encodeFile(std::string sourcePath, std::string targetPath) {

    FILE *input_file, *output_file;
    input_file = fopen(sourcePath.c_str(), "rb");
    output_file = fopen(targetPath.c_str(), "wb");

    int PCM_SIZE = BUFFER_SIZE*3;
    int MP3_SIZE = BUFFER_SIZE*3;

    short input[PCM_SIZE];
    unsigned char output[MP3_SIZE];

    int read, write;

    do {
        read = fread(input, sizeof(short int), PCM_SIZE, input_file);

        if (read == 0) {
            write = lame_encode_flush(lame,output, MP3_SIZE);
        } else {
            write = lame_encode_buffer(lame, input, input, read, output, MP3_SIZE);
        }
        fwrite(output, write, 1, output_file);
    } while (read != 0);

    fclose(input_file);
    fclose(output_file);
}

bool CAP::Mp3Compressor::process(const AudioBuffer& audioBuffer, AudioBuffer& processed) {
    auto compressedBuffer = reinterpret_cast<unsigned char *>(processed.getBuffer());
    int output = lame_encode_buffer(lame, audioBuffer.getBuffer(), audioBuffer.getBuffer(), audioBuffer.size(),
    compressedBuffer, processed.AudioBufferCapacity);

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

    processed.setSize(output);
    return true;
}
int CAP::Mp3Compressor::process(short * audioBuffer, int inSize, u_char * processed, int processedSize) {
    int output = lame_encode_buffer(lame, audioBuffer, audioBuffer, inSize, processed, processedSize);

    return output;
}
int CAP::Mp3Compressor::flush(AudioBuffer& audioBuffer) {
    auto compressedBuffer = reinterpret_cast<unsigned char *>(audioBuffer.getBuffer());
    int result = lame_encode_flush(lame, compressedBuffer, audioBuffer.size());

    return result;
}

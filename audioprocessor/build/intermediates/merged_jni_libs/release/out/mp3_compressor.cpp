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
int CAP::Mp3Compressor::read_samples(FILE *input_file, short *input) {
    int nb_read;
    nb_read = fread(input, 1, sizeof(short), input_file) / sizeof(short);

    int i = 0;
    while (i < nb_read) {
        input[i] = be_short(input[i]);
        i++;
    }

    return nb_read;
}

void CAP::Mp3Compressor::encodeFile(std::string sourcePath, std::string targetPath) {

    FILE *input_file, *output_file;
    input_file = fopen(sourcePath.c_str(), "rb");
    output_file = fopen(targetPath.c_str(), "wb");

    short input[BUFFER_SIZE];
    char output[BUFFER_SIZE];
    int nb_read = 0;
    int nb_write = 0;
    int nb_total = 0;

    while (nb_read = read_samples(input_file, input)) {
        nb_write = lame_encode_buffer(lame, input, input, nb_read,
                                      reinterpret_cast<unsigned char *>(output),
                                      BUFFER_SIZE);
        fwrite(output, nb_write, 1, output_file);
        nb_total += nb_write;
    }

    nb_write = lame_encode_flush(lame, reinterpret_cast<unsigned char *>(output), BUFFER_SIZE);
    fwrite(output, nb_write, 1, output_file);

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

//
//  pcm_processor.cpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 6/15/17.
//
//

#include "pcm_processor.hpp"
#include <fstream>

typedef struct wav_header {
    // RIFF Header
    std::uint8_t riff_header[4]; // Contains "RIFF"
    std::uint32_t wav_size; // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
    std::uint8_t wave_header[4]; // Contains "WAVE"
    
    // Format Header
    std::uint8_t fmt_header[4]; // Contains "fmt " (includes trailing space)
    std::uint32_t fmt_chunk_size; // Should be 16 for PCM
    std::uint16_t audio_format; // Should be 1 for PCM. 3 for IEEE Float
    std::uint16_t num_channels;
    std::uint32_t sample_rate;
    std::uint32_t byte_rate; // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
    std::uint16_t sample_alignment; // num_channels * Bytes Per Sample
    std::uint16_t bit_depth; // Number of bits per sample
    
    // Data
    std::uint8_t data_header[4]; // Contains "data"
    std::uint32_t data_bytes; // Number of bytes in data. Number of samples * num_channels * sample byte size
    // uint8_t bytes[]; // Remainder of wave file is bytes
    
    void writeTo(std::ostream& stream) {
        stream.write((char*) &riff_header, sizeof(riff_header));
        stream.write((char*) &wav_size, sizeof(wav_size));
        stream.write((char*) &wave_header, sizeof(wave_header));
        stream.write((char*) &fmt_header, sizeof(fmt_header));
        stream.write((char*) &fmt_chunk_size, sizeof(fmt_chunk_size));
        stream.write((char*) &audio_format, sizeof(audio_format));
        stream.write((char*) &num_channels, sizeof(num_channels));
        stream.write((char*) &sample_rate, sizeof(sample_rate));
        stream.write((char*) &byte_rate, sizeof(byte_rate));
        stream.write((char*) &sample_alignment, sizeof(sample_alignment));
        stream.write((char*) &bit_depth, sizeof(bit_depth));
        stream.write((char*) &data_header, sizeof(data_header));
        stream.write((char*) &data_bytes, sizeof(data_bytes));
    }
    
} wav_header;



bool CAP::PcmProcessor::process(const AudioBuffer& audioBuffer, AudioBuffer& processed) {
    auto original = audioBuffer.getBuffer();
    auto pseudoProcessed = processed.getBuffer();
    
    for (std::size_t i = 0; i < audioBuffer.size(); i++) {
        pseudoProcessed[i] = original[i];
    }
    
    processed.setSize(audioBuffer.size());
    return true;
};

void CAP::PcmProcessor::finalizeFileAtPath(std::string path) {
    
    std::ifstream originalFile(path, std::ifstream::binary | std::ifstream::ate);
    wav_header header;
    
    auto tempFilePath = path + ".tmp";
    std::ofstream tmpstream(tempFilePath, std::ofstream::binary | std::ofstream::app);
    
    std::size_t size = originalFile.tellg();
    
    header.riff_header[0] = 'R';
    header.riff_header[1] = 'I';
    header.riff_header[2] = 'F';
    header.riff_header[3] = 'F';
    header.wav_size = size + 44 - 8; //data size + header size - first 8 bytes
    header.wave_header[0] = 'W';
    header.wave_header[1] = 'A';
    header.wave_header[2] = 'V';
    header.wave_header[3] = 'E';
    header.fmt_header[0] = 'f';
    header.fmt_header[1] = 'm';
    header.fmt_header[2] = 't';
    header.fmt_header[3] = ' ';
    header.fmt_chunk_size = 16;
    header.audio_format = 1;
    header.num_channels = 1;
    header.sample_rate = 44100;
    header.byte_rate = header.sample_rate * header.num_channels * 2;
    header.sample_alignment = header.num_channels * 2; //num_channels * Bytes Per Sample
    header.bit_depth = 16;
    header.data_header[0] = 'd';
    header.data_header[1] = 'a';
    header.data_header[2] = 't';
    header.data_header[3] = 'a';
    header.data_bytes = size; // Number of bytes in data. Number of samples * num_channels * sample byte size

    originalFile.seekg(0);

    header.writeTo(tmpstream);
    
    tmpstream << originalFile.rdbuf();
    originalFile.close();
    tmpstream.close();
    std::remove(path.c_str());
    std::rename(tempFilePath.c_str(), path.c_str());
};

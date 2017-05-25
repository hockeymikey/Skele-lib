//
//  system_file.cpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/24/17.
//
//

#include "system_file.hpp"

CAP::SystemFile::SystemFile(std::string filepath_): CAP::File(), filepath(filepath_) {
    
}

void CAP::SystemFile::close() {
    fileStream.close();
}

bool CAP::SystemFile::isOpen() const {
    return fileStream.is_open();
}

bool CAP::SystemFile::write(const CAP::AudioBuffer &audioBuffer) {
    auto b = reinterpret_cast<const char *>(audioBuffer.getBuffer());
    fileStream.write(b, audioBuffer.size() * sizeof(*audioBuffer.getBuffer()));
    return !fileStream.bad();
}

void CAP::SystemFile::open() {
    fileStream.open(filepath, std::ofstream::binary | std::ofstream::app);
}

std::string CAP::SystemFile::path() const {
    return filepath;
}

CAP::SystemFile::~SystemFile() {
    
}

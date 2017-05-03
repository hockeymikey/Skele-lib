//
//  StreamWriter.cpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/2/17.
//
//

#include "StreamWriter.hpp"

namespace CAP {
    StreamWriter::StreamWriter(std::string filePath): fileStream(filePath, std::ios_base::binary) {
    }
    StreamWriter::StreamWriter(const StreamWriter& other) {
        
    }
    void StreamWriter::write(int16_t sample) {
        fileStream.write(reinterpret_cast<char *>(&sample), sizeof(int16_t));
    }
    
    void StreamWriter::close() {
        fileStream.close();
    }
}


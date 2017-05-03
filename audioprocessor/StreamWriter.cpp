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
    void StreamWriter::write(int16_t samples[], int size) {
        fileStream.write(reinterpret_cast<char *>(samples), size * sizeof(int16_t));
    }
    
    void StreamWriter::close() {
        fileStream.close();
    }
}


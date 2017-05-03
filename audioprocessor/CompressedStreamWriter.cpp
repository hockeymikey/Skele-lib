//
//  CompressedStreamWriter.cpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/3/17.
//
//

#include "CompressedStreamWriter.hpp"
#include <lame/lame.h>

namespace CAP {
    CompressedStreamWriter::CompressedStreamWriter(std::string filePath, int quality): StreamWriter(filePath), compressionQuality(quality) {
        
    }
    void CompressedStreamWriter::write(int16_t sample) {
        lame_t lame = lame_init();
        lame_set_quality(lame, compressionQuality);
        lame_init_params(lame);
        lame_encode_buffer(lame, <#const short *buffer_l#>, <#const short *buffer_r#>, <#const int nsamples#>, <#unsigned char *mp3buf#>, <#const int mp3buf_size#>)
//        parent::write(sample);
    }
    
}

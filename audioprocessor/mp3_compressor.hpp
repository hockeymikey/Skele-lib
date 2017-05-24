//
//  mp3_compressor.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/14/17.
//
//

#ifndef Mp3Compressor_hpp
#define Mp3Compressor_hpp

#include "compressor.hpp"
#include "lame.h"
#include <vector>

namespace CAP {
    class Mp3Compressor: public Compressor {
    public:
        /**
         Compresses the buffer to MP3 format and returns it
         
         @param buffer
            An array or samples
         **/
        std::vector<int16_t> compress(std::vector<int16_t> const& buffer);
        
        /**
         Constructor
         
         @param compressionQuality
         Compression quality. 0 is best (very slow), 9 is worst
         **/
        Mp3Compressor(int compressionQuality);
        
        
        /**
         Destructor
         **/
        ~Mp3Compressor();
        
    protected:
    private:
        lame_t lame;        
    };
}

#endif /* Mp3Compressor_hpp */

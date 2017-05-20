//
//  mp3_compressor.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/14/17.
//
//

#ifndef Mp3Compressor_hpp
#define Mp3Compressor_hpp

#include "signal_processor.hpp"
#include "lame.h"
#include <vector>

namespace CAP {
    class Mp3Compressor: public SignalProcessor {
    public:
        /**
         Compresses the buffer to MP3 format and returns it
         
         @param samples
            An array or samples
         **/
        AudioBuffer process(const AudioBuffer& audioBuffer);
        
        /**
         Constructor
         
         @param compressionQuality
            Compression quality. 0 is best (very slow), 9 is worst
         @param sampleRate
            Let lame know what sample rate is
         **/
        Mp3Compressor(int compressionQuality, int sampleRate);
        
        
        /**
         Destructor
         **/
        ~Mp3Compressor();
        
    protected:
    private:
        lame_t lame;
//        unsigned char compressedBuffer[4096];
    };
}

#endif /* Mp3Compressor_hpp */

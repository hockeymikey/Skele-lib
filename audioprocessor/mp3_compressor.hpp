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

struct lame_global_struct;
typedef struct lame_global_struct lame_global_flags;
typedef lame_global_flags *lame_t;


namespace CAP {
    class Mp3Compressor: public SignalProcessor {
    public:

        /**
         Compresses to MP3 'in' buffer and places the compressed samples in 'out' buffer
         
         @param in
            Constant reference to buffer with raw samples
         @param out
            Reference to buffer where compressed samples will be stored
         **/
        bool process(const AudioBuffer& in, AudioBuffer& out) const;
        
        bool process(const AudioBuffer& audioBuffer, unsigned char *rawBuffer, int& nbytes) const;
        
        
        void finalizeFileAtPath(std::string path) const;
        
        bool usesRawBufferForProcessing() const;
        
        /**
         Constructor
         
         @param compressionQuality
            Compression quality. 0 is best (very slow), 9 is worst
         @param sampleRate
            Let lame know what sample rate is
         **/
        Mp3Compressor(int compressionQuality, int sampleRate);
        
        
        /**
         Destructor. closes lame
         **/
        ~Mp3Compressor();
        
    protected:
    private:
        lame_t lame;
    };
}

#endif /* Mp3Compressor_hpp */

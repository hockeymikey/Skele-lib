//
//  mp3_compressor.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/14/17.
//
//

#ifndef Mp3Compressor_hpp
#define Mp3Compressor_hpp

#include <jni.h>
#include "signal_processor.hpp"
#include "lame.h"

namespace CAP {
    class Mp3Compressor: public SignalProcessor {
    public:
        /**
         * Compresses to MP3 'in' buffer and places the compressed samples in 'out' buffer
         *
         * @param in Constant reference to buffer with raw samples
         * @param out Reference to buffer where compressed samples will be stored
         * @return if process was successful
         */
        bool process(const AudioBuffer& in, AudioBuffer& out);

        /**
         * Compresses to MP3 'in' buffer and places the compressed samples in 'out' buffer
         *
         * @param audioBuffer input audio buffer
         * @param processed output audio buffer for processed
         * @param sampleRate rate to sample from audio buffer. Should be length of audioBuffer or less for a sped up output
         * @return return code from lame
         */
        int process(short *audioBuffer, int, u_char *processed, int sampleRate);

        /**
         * Flush LAME buffer.
         *
         * @param out   result encoded MP3 stream.  Must be at least 7200 bytes.
         * @return  number of bytes output to mp3buf.  Can be 0
         */
        int flush(AudioBuffer& out);

        void finalizeFileAtPath(std::string path);

        /**
         * Compress source file into target file
         *
         * @param sourcePath source file to encode
         * @param targetPath target file to encode into
         */
        void encodeFile(std::string sourcePath, std::string targetPath);

         /**
          * Constructor
          *
          * @param compressionQuality Compression quality. 0 is best (very slow), 9 is worst
          * @param sampleRate Let lame know what sample rate is
          * @param numChannels Number of channels for lame to use
          * @param outBitrate Let lame know what outBitrate is
          * @param outSampleRate Let lame know what out sample rate is
          */
        Mp3Compressor(int compressionQuality, int sampleRate, int numChannels, int outBitrate,
                      int outSampleRate);
         /**
          * Constructor
          *
          * @param compressionQuality Compression quality. 0 is best (very slow), 9 is worst
          * @param sampleRate Let lame know what sample rate is
          */
        Mp3Compressor(int compressionQuality, int sampleRate);

        /**
         * Destructor. closes lame
         **/
        ~Mp3Compressor();
        
    protected:
    private:
        lame_t lame;

        int read_samples(FILE *input_file, short *input);
    };
}

#endif /* Mp3Compressor_hpp */

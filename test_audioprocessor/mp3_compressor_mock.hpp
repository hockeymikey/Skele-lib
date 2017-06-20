//
//  mp3_compressor_mock.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/23/17.
//
//

#ifndef mp3_compressor_mock_hpp
#define mp3_compressor_mock_hpp

#include "gmock/gmock.h"
#include "signal_processor.hpp"
#include "audio_buffer.hpp"
#include "file.hpp"

class Mp3CompressorMock : public CAP::SignalProcessor {
public:
    
    MOCK_METHOD2(process, bool (const CAP::AudioBuffer& in, CAP::AudioBuffer& out));
    MOCK_METHOD1(finalizeFileAtPath, void (std::string path));
  
//    MOCK_METHOD2(do_process, bool (const CAP::AudioBuffer* in, CAP::AudioBuffer* out));
//    
//    bool process(const CAP::AudioBuffer& in, CAP::AudioBuffer& out) {
//        return do_process(&in, &out);
//    }
};

#endif /* mp3_compressor_mock_hpp */

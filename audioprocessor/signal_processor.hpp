//
//  signal_processor.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/14/17.
//
//

#ifndef signal_processor_hpp
#define signal_processor_hpp

#include <cstddef>
#include <cstdint>
#include "audio_buffer.hpp"

namespace CAP {
    class SignalProcessor {
    public:
        
        /**
         Needs to be implemented by derived class
         
         @param audioBuffer
            Array of samples
         @param compressed
            Array of processed samples
         **/
        virtual void process(const AudioBuffer& audioBuffer, AudioBuffer& processed) = 0;
        
        SignalProcessor(const SignalProcessor& other) = delete;
        SignalProcessor() = default;
        
        SignalProcessor(SignalProcessor&&) = default;
        
    protected:
    private:
        
    };
}

#endif /* compressor_hpp */

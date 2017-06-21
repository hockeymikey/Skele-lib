//
//  signal_processor.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/14/17.
//
//

#ifndef signal_processor_hpp
#define signal_processor_hpp

#include "audio_buffer.hpp"
#include "file.hpp"

namespace CAP {
    class SignalProcessor {
    public:
        
        /**
         Needs to be implemented by derived class
         
         @param audioBuffer
            Array of samples
         @param compressed
            Array of processed samples
         
         @return success/failure
         **/
        virtual bool process(const AudioBuffer& audioBuffer, AudioBuffer& processed) = 0;
        
        virtual void finalizeFileAtPath(std::string path) = 0;
        
        virtual ~SignalProcessor() {
            
        };
        
        SignalProcessor(const SignalProcessor& other) = delete;
        SignalProcessor() = default;
        
        SignalProcessor(SignalProcessor&&) = default;
        
    protected:
    private:
        
    };
}

#endif /* compressor_hpp */

//
//  pcm_processor.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 6/15/17.
//
//

#ifndef pcm_processor_hpp
#define pcm_processor_hpp

#include "signal_processor.hpp"

namespace CAP {
    class PcmProcessor: public SignalProcessor {
    public:
        
        /**
         Pass-through no processing here
         
         @param boolean
            Always returns true
         **/
        bool process(const AudioBuffer& in, AudioBuffer& out) const;
        
        bool process(const AudioBuffer& audioBuffer, unsigned char *rawBuffer, int& nbytes) const;
        
        bool usesRawBufferForProcessing() const;
        
        void finalizeFileAtPath(std::string path) const;
                
        PcmProcessor() = default;
        
        
    protected:
    private:
        
    };
}

#endif /* pcm_processor_hpp */

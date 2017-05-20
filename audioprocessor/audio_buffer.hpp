//
//  audio_buffer.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/18/17.
//
//

#ifndef audio_buffer_hpp
#define audio_buffer_hpp

#include <cstdlib>
#include <cstdint>
#include <array>


namespace CAP {
    class AudioBuffer {
    public:
        
        /**
         Move constructor
         **/
        AudioBuffer(AudioBuffer&&) = default;
        
        /**
         Copy Constructor
         **/
        AudioBuffer(const AudioBuffer &other) = default;
        
        /**
         Constructor
         **/
        AudioBuffer(std::int16_t samples_[], std::size_t nsamples_): nsamples(nsamples_) {
            for (int i = 0; i < nsamples; i++) {
                buffer[i] = samples_[i];
            }
        };
        
        std::size_t size() const {
            return nsamples;
        };
        
        const std::int16_t * getBuffer() const {
            return buffer.data();
        };
        
        AudioBuffer() = delete;
        AudioBuffer operator=(const AudioBuffer&) = delete;
        
    protected:
    private:
        
        std::array<std::int16_t, 4096> buffer;
        std::size_t nsamples;
    };
}
#endif /* audio_buffer_hpp */

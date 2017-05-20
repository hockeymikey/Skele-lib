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
        AudioBuffer(const AudioBuffer &other) {
            nsamples = other.nsamples;
            for (int i = 0; i < nsamples; i++) {
                samples[i] = other.samples[i];
            }
        }
        
        /**
         Constructor
         **/
        AudioBuffer(std::int16_t samples_[], std::size_t nsamples_): samples(samples_), nsamples(nsamples_) {};
        
        std::size_t size() {
            return nsamples;
        };
        
        std::int16_t * getSamples() {
            return samples;
        };
        
        AudioBuffer() = delete;
        AudioBuffer operator=(const AudioBuffer&) = delete;
        
    protected:
    private:
        
        
        std::int16_t *samples;
        std::size_t nsamples;
    };
}
#endif /* audio_buffer_hpp */

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
#include <string>

namespace CAP {
    /**
     Stores 4096 samples
     **/
    class AudioBuffer {
    public:
        
        /**
         Move constructor
         **/
        AudioBuffer(AudioBuffer&&) = default;
        
        /**
         Copy Constructor. Don't let copying
         **/
        AudioBuffer(const AudioBuffer &other) = delete;
        
        /**
         Constructor. Copies samples into internal buffer
         
         @param samples_
            Pointer to array of samples
         @param nsamples_
            Number of samples array contains
         **/
        AudioBuffer(std::int16_t samples_[], std::size_t nsamples_): nsamples(nsamples_) {
            if (nsamples > buffer.max_size()) {
                std::string msg("buffer overflow (max buffer size is " + std::to_string(buffer.max_size()));
                throw std::runtime_error(msg);
            }
            for (int i = 0; i < nsamples; i++) {
                buffer[i] = samples_[i];
            }
        };
        
        /**
         Returns the number of samples in buffer
         
         @return number of samples
         **/
        std::size_t size() const {
            return nsamples;
        };
        
        /**
         Returns pointer to array of samples
         **/
        const std::int16_t * getBuffer() const {
            return buffer.data();
        };
        
        /**
        Don't allow default constructor
         **/
        AudioBuffer() = delete;
        
        /**
         Don't allow copy by assignment
         **/
        AudioBuffer operator=(const AudioBuffer&) = delete;
        
    protected:
    private:
        
        std::array<std::int16_t, 4096> buffer; //use STL array to get to max size
        std::size_t nsamples;
    };
}
#endif /* audio_buffer_hpp */

//
//  audio_buffer.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/18/17.
//
//

#ifndef audio_buffer_hpp
#define audio_buffer_hpp

#include <string>

namespace CAP {
    /**
     Stores 4410 samples (10 msec at 44.1 kHz)
     **/
    class AudioBuffer {
    public:
        
        static const std::size_t AudioBufferCapacity = 4410;
        
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
         
         @throws std::overflow_error
            If nsamples is
         **/
        AudioBuffer(std::int16_t samples_[], std::size_t nsamples_): nsamples(nsamples_) {
            if (nsamples > AudioBufferCapacity) {
                std::string msg("buffer overflow (max buffer size is " + std::to_string(AudioBufferCapacity));
                throw std::overflow_error(msg);
            }
            for (int i = 0; i < nsamples; i++) {
                buffer[i] = samples_[i];
            }
        };
        /**
         Updates the number of samples buffer has
         
         @throws std::overflow_error
         **/
        void setSize(std::size_t newSize) {
            if (newSize > AudioBufferCapacity) {
                std::string msg("buffer overflow (max buffer size is " + std::to_string(AudioBufferCapacity));
                throw std::overflow_error(msg);
            }
            nsamples = newSize;
        }
        
        
        /**
         Returns the number of samples in buffer
         
         @return number of samples
         **/
        std::size_t size() const {
            return nsamples;
        };
        
        /**
         Returns pointer to array of samples. Array contents cannot be changed
         
         @return pointer to array of samples
         **/
        const std::int16_t * getBuffer() const {
            return buffer;
        };
        
        /**
         Returns pointer to array that can be modified
         
         @return pointer to array of samples
         **/
        std::int16_t * getBuffer() {
            return buffer;
        };
        
        /**
         Allow initializing empty buffer
         **/
        AudioBuffer() = default;
        
//        virtual ~AudioBuffer();
        
        /**
         Don't allow copy by assignment
         **/
        AudioBuffer operator=(const AudioBuffer&) = delete;
        
    protected:
    private:
        std::int16_t buffer[AudioBufferCapacity];
        std::size_t nsamples = 0;
    };
}
#endif /* audio_buffer_hpp */

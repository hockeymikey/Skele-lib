//
//  file.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/24/17.
//
//

#ifndef file_hpp
#define file_hpp

#include "audio_buffer.hpp"

namespace CAP {
    class File {
    public:
        virtual void open() = 0;
        virtual bool isOpen() const = 0;
        virtual bool write(const CAP::AudioBuffer &audioBuffer) = 0;
        virtual ~File() {};
        virtual std::string path() const = 0;
        virtual void close() = 0;
        
        File(const File& other) = delete;
        File() = default;
        
        File(File&&) = default;
    protected:
    private:
    };
}
#endif /* file_hpp */

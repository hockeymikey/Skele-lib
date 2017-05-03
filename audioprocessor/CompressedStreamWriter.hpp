//
//  CompressedStreamWriter.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/3/17.
//
//

#ifndef CompressedStreamWriter_hpp
#define CompressedStreamWriter_hpp

#include "StreamWriter.hpp"

namespace CAP {
    class CompressedStreamWriter: public StreamWriter {
    public:
        CompressedStreamWriter(std::string, int);
        void write(int16_t);
    protected:
        
    private:
        int compressionQuality;
    };
}

#endif /* CompressedStreamWriter_hpp */

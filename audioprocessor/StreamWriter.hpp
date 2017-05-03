//
//  StreamWriter.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/2/17.
//
//

#ifndef StreamWriter_hpp
#define StreamWriter_hpp

#include <string>
#include <fstream>

namespace CAP {
    class StreamWriter {
    public:
        StreamWriter(std::string);
        StreamWriter(const StreamWriter&);
        void write(int16_t[], int);
        void close();
    protected:
        
    private:
        std::ofstream fileStream;
        StreamWriter();
        StreamWriter operator=(const StreamWriter&);

    };
}

#endif /* StreamWriter_hpp */

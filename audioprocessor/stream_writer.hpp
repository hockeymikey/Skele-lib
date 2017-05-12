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
    /**
     Base class for writing raw audio samples to the file specified by the client.
     **/
    class StreamWriter {
    public:
        /**
         Constructor
         
         @param filepath
            The file path where this object is gonna write samples to.
         **/
        StreamWriter(std::string filepath);
        
        /**
         Copy constructor
         @param other
            Other stream writer object
         **/
        StreamWriter(const StreamWriter& other);
        
        /**
         Writes array of samples to file.
         
         @param samples
            Array of samples
         @param nsamples
            Number of samples in array
         **/
        void write(int16_t samples[], size_t nsamples);
        
        /**
         Closes the file.
         **/
        void close();
    protected:
        
    private:
        std::ofstream fileStream;
        StreamWriter();
        StreamWriter operator=(const StreamWriter&);

    };
}

#endif /* StreamWriter_hpp */

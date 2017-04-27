//
//  audioprocessor.hpp
//
//  Created by Aziz Usmanov on 4/11/17.
//  Copyright © 2017 Cogi. All rights reserved.
//

#ifndef audioprocessor_hpp
#define audioprocessor_hpp

#include "AudioDescription.hpp"
#include <iostream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

namespace Cogi {
    class AudioProcessor {
        
    public:
        AudioProcessor(AudioDescription, std::string);
        void writeAudioSamples(int [], int);
        void close();
    protected:
    
    private:
        AudioDescription audioDescription;
        boost::filesystem::path rawAudioPath;
        boost::filesystem::ofstream ofstream;
        AudioProcessor();
        AudioProcessor(const AudioProcessor&);
        AudioProcessor operator=(const AudioProcessor&);
    };
}

#endif /* audioprocessor_hpp */

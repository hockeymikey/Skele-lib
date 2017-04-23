//
//  audioprocessor.hpp
//  test
//
//  Created by Aziz Usmanov on 4/11/17.
//  Copyright © 2017 Cogi. All rights reserved.
//

#ifndef audioprocessor_hpp
#define audioprocessor_hpp

#include <iostream>

namespace Cogi {
    
    struct AudioDescription {
        float sampleRate;
        int numberOfChannels;
        int bitsPerChannel;
    };
    
    class AudioProcessor {
        
        AudioDescription audioDescription;
        std::string filePathToStoreRawData;
        
        public:
        AudioProcessor(AudioDescription, std::string);
        void writeAudioSamples(char []);
    };
}

#endif /* audioprocessor_hpp */

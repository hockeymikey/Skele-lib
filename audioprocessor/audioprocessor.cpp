//
//  audioprocessor.cpp
//  test
//
//  Created by Aziz Usmanov on 4/11/17.
//  Copyright © 2017 Cogi. All rights reserved.
//

#include "audioprocessor.hpp"
#include <iostream>
//#include <boost>

namespace Cogi {
    AudioProcessor::AudioProcessor(AudioDescription ad, std::string path) {
        audioDescription = ad;
        filePathToStoreRawData = path;
    }
    
    void AudioProcessor::writeAudioSamples(char samples[]) {
        
    }
}

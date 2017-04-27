//
//  AudioProcessor.cpp
//  test
//
//  Created by Aziz Usmanov on 4/11/17.
//  Copyright © 2017 Cogi. All rights reserved.
//

#include "AudioProcessor.hpp"
#include <iostream>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>

namespace Cogi {
    AudioProcessor::AudioProcessor(AudioDescription ad, std::string path): audioDescription(ad.sampleRate, ad.numberOfChannels, ad.bitsPerChannel), rawAudioPath(path) {
        if (!boost::filesystem::exists(rawAudioPath)) {
            std::string message = std::string("cannot open file at path: ") + path;
            throw std::runtime_error(message);
        }
    }
    
    void AudioProcessor::writeAudioSamples(int samples[], int size) {
        
        if (!ofstream.is_open()) {
            ofstream.open(rawAudioPath);
        }
        
        ofstream.write(reinterpret_cast<const char*>(samples), size * sizeof(int));
        
    }
    void AudioProcessor::close() {
        ofstream.close();
    }
}

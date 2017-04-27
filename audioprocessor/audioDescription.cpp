//
//  AudioDescription.cpp
//  test
//
//  Created by Aziz Usmanov on 4/11/17.
//  Copyright © 2017 Cogi. All rights reserved.
//
#include "AudioDescription.hpp"

namespace Cogi {
    AudioDescription::AudioDescription(float sampleRate_, int numberOfChannels_, int bitsPerChannel_): sampleRate(sampleRate_), numberOfChannels(numberOfChannels_), bitsPerChannel(bitsPerChannel_) {
        
    }
    AudioDescription::AudioDescription(const AudioDescription& other) {
        sampleRate = other.sampleRate;
        numberOfChannels = other.numberOfChannels;
        bitsPerChannel = other.bitsPerChannel;
    }
}

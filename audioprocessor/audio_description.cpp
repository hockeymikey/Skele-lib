
#include "audio_description.hpp"

namespace CAP {
    AudioDescription::AudioDescription(float sampleRate_, int numberOfChannels_, int bitsPerChannel_): sampleRate(sampleRate_), numberOfChannels(numberOfChannels_), bitsPerChannel(bitsPerChannel_) {
        
    }
    AudioDescription::AudioDescription(const AudioDescription& other) {
        sampleRate = other.sampleRate;
        numberOfChannels = other.numberOfChannels;
        bitsPerChannel = other.bitsPerChannel;
    }
}

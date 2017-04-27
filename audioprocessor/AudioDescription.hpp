//
//  AudioDescription.hpp
//  test
//
//  Created by Aziz Usmanov on 4/11/17.
//  Copyright © 2017 Cogi. All rights reserved.
//

#ifndef audiodescription_hpp
#define audiodescription_hpp



namespace Cogi {
    
    struct AudioDescription {
    public:
        float sampleRate;
        int numberOfChannels;
        int bitsPerChannel;
        AudioDescription(float, int, int);
        AudioDescription(const AudioDescription&);
    private:
        AudioDescription();
        AudioDescription operator=(const AudioDescription&);
    };
}

#endif /* audiodescription_hpp */

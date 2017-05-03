

#ifndef audiodescription_hpp
#define audiodescription_hpp



namespace CAP {
    
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

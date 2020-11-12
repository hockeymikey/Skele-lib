//
//  StreamWriter.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/2/17.
//
//

#ifndef AudioProcessorBridge_hpp
#define AudioProcessorBridge_hpp

#include "../cpp/audio_processor.hpp"

#include <string>

namespace CAP {

    class AudioProcessorBridge {
    public:
        AudioProcessorBridge(const AbstractStreamWriterObserver& other) = delete;
        AbstractStreamWriterObserver() = default;
        AbstractStreamWriterObserver(AbstractStreamWriterObserver&&) = default;

        virtual void streamWriterStopped() = 0;
        virtual void streamWriterKilled() = 0;
        virtual ~AbstractStreamWriterObserver() {

        };

        AudioProcessor *audioprocessor;
        int killThresholdSeconds;
    };

}

#endif /* StreamWriter_hpp */

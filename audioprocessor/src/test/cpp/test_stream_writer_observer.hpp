//
//  test_stream_writer_observer.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 8/8/17.
//
//

#ifndef test_stream_writer_observer_hpp
#define test_stream_writer_observer_hpp

#include "stream_writer.hpp"

class TestStreamWriterObserver: public CAP::AbstractStreamWriterObserver {
public:
    std::atomic_bool streamWriterDidStop = {false};
    std::atomic_bool streamWriterDidKill = {false};
    void streamWriterStopped() {
        streamWriterDidStop = true;
    }
    void streamWriterKilled() {
        streamWriterDidKill = true;
    }
};


#endif /* test_stream_writer_observer_hpp */

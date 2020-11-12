#include "stream_writer.hpp"
#include "audio_processor_bridge.cpp"
#include <string>

namespace CAP {
    class StreamWriterObserver : public AbstractStreamWriterObserver {

    public:
        AudioProcessorWrapper *bridge;

        void streamWriterStopped();
        void streamWriterKilled();

        std::string filePath;
    };
}
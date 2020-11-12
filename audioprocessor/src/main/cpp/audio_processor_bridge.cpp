#include "audio_processor.hpp"

using namespace CAP;

struct AudioProcessorWrapper {
    AudioProcessor *audioprocessor;
    int killThresholdSeconds;
};
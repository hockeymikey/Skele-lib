
#include "audio_processor.hpp"
#include "audio_processor_test.hpp"
#include "mp3_compressor.hpp"
#include <vector>
#include <random>

using namespace CAP;
using namespace std;

AudioProcessorTest::AudioProcessorTest() {
    // Have qux return true by default
//    ON_CALL(m_bar,qux()).WillByDefault(Return(true));
    // Have norf return false by default
//    ON_CALL(m_bar,norf()).WillByDefault(Return(false));
}

AudioProcessorTest::~AudioProcessorTest() {};

void AudioProcessorTest::SetUp() {};

void AudioProcessorTest::TearDown() {};




TEST(AudioProcessorTest, TestStream) {
    shared_ptr<Mp3Compressor> compressor(new Mp3Compressor(5));
    StreamWriter sw1("AudioProcessorTest_TestStream.bin");
    StreamWriter sw2("AudioProcessorTest_TestStream.mp3", compressor);
    
    vector<StreamWriter> sws;
    sws.push_back(sw1);
    sws.push_back(sw2);
    AudioProcessor ap(sws);
    
    while (true) {
        random_device rd;
        mt19937 mt(rd());
        uniform_real_distribution<double> dist(INT16_MIN, INT16_MAX);
        vector<int16_t> samples;
        for(int i = 0; i < 1024; i++) {
            samples.push_back((int16_t) dist(mt));
        }
        ap.writeAudioSamples(samples);
        
    }
//
}



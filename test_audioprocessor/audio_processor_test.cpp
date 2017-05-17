
#include "audio_processor.hpp"
#include "audio_processor_test.hpp"
#include "mp3_compressor.hpp"
#include <vector>
#include <random>
#include <cstdio>

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

TEST(AudioProcessorTest, NoStreamWriters) {
    vector<shared_ptr<StreamWriter>> sws;
    EXPECT_THROW(AudioProcessor ap(sws), runtime_error);
}

TEST(AudioProcessorTest, IllegalCompressor) {
    vector<shared_ptr<StreamWriter>> sws;
    auto sw2 = make_shared<StreamWriter>("AudioProcessorTest_TestStream.mp3", make_shared<Mp3Compressor>(5));
    sws.push_back(sw2);
    EXPECT_THROW(AudioProcessor ap(sws), runtime_error);
}


TEST(AudioProcessorTest, TestStream) {
    auto compressor = make_shared<Mp3Compressor>(5);
    remove("AudioProcessorTest_TestStream.bin");
    remove("AudioProcessorTest_TestStream.mp3");
    auto sw1 = make_shared<StreamWriter>("AudioProcessorTest_TestStream.bin");
    auto sw2 = make_shared<StreamWriter>("AudioProcessorTest_TestStream.mp3", compressor);
    
    vector<shared_ptr<StreamWriter>> sws;
    sws.push_back(sw1);
    sws.push_back(sw2);
    AudioProcessor ap(sws);
    
    
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(INT16_MIN, INT16_MAX);
    vector<int16_t> buffer;
    int seconds = 5;
    int nsamples = seconds * 44100;
    //generate 5 seconds worth of samples
    for (int i = 1; i <= nsamples; i++) {
        int16_t sample = (int16_t)dist(mt);
        buffer.push_back(sample);
        
        if (i % 1050 == 0) {
            ap.writeAudioSamples(buffer);
            buffer.clear();
        }
    }
    
    ap.stop();
}




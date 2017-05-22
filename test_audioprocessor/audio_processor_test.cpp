
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
    vector<StreamWriter> sws = {};
    EXPECT_THROW(AudioProcessor ap(sws), runtime_error);
}

TEST(AudioProcessorTest, TestStream) {
    auto sampleRate = 44100;
    auto compressor = make_shared<Mp3Compressor>(9, 44100);
    remove("AudioProcessorTest_TestStream.bin");
    remove("AudioProcessorTest_TestStream.mp3");
    StreamWriter sw1("AudioProcessorTest_TestStream.bin");
    StreamWriter sw2("AudioProcessorTest_TestStream.mp3", compressor);
    
    vector<StreamWriter> sws;
    sws.push_back(std::move(sw1));
    sws.push_back(std::move(sw2));
    
    AudioProcessor ap(sws);
    
    
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(INT16_MIN, INT16_MAX);
    std::int16_t buffer[1050];
    int seconds = 10;
    int nsamples = seconds * sampleRate;
    //generate 5 seconds worth of samples
    for (int i = 1; i <= nsamples; i++) {
        int16_t sample = (int16_t)dist(mt);
        buffer[(i - 1) % 1050] = sample;
        
        if (i % 1050 == 0) {
            ap.process(buffer, 1050);
        }
    }
    
    ap.stop();
}


TEST(AudioProcessorTest, TestLongStream) {
    auto sampleRate = 44100;
    auto compressor = make_shared<Mp3Compressor>(9, sampleRate);
    remove("AudioProcessorTest_TestLongStream.bin");
    remove("AudioProcessorTest_TestLongStream.mp3");
    
    
    vector<StreamWriter> sws = {};
    sws.push_back(StreamWriter("AudioProcessorTest_TestLongStream.bin"));
    sws.push_back(StreamWriter("AudioProcessorTest_TestLongStream.mp3", compressor));
    AudioProcessor ap(sws);
    
    
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(INT16_MIN, INT16_MAX);
    std::int16_t buffer[4410];
    int seconds = 2000;
    int nsamples = seconds * sampleRate;
    
    
    for (int i = 1; i <= nsamples; i++) {
        int16_t sample = (int16_t)dist(mt);
        buffer[(i - 1) % 4410] = sample;
        
        if (i % (4410) == 0) {
            ap.process(buffer, 4410);
//            this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    ap.stop();
//    while (true) {
//        cout << 12313 << endl;
//        this_thread::sleep_for(std::chrono::seconds(1));
//    }
    
}




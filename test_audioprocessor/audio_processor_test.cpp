#include "gmock/gmock.h"
#include "mp3_compressor_mock.hpp"
#include "audio_processor.hpp"
#include "audio_processor_test.hpp"
#include "mp3_compressor.hpp"
#include <vector>
#include <random>
#include <cstdio>

using namespace CAP;
using namespace std;
using ::testing::AtLeast;

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

//TEST(AudioProcessorTest, TestStream) {
//    auto sampleRate = 44100;
//    auto compressor = make_shared<Mp3Compressor>(9, 44100);
//    remove("AudioProcessorTest_TestStream.bin");
//    remove("AudioProcessorTest_TestStream.mp3");
//    StreamWriter sw1("AudioProcessorTest_TestStream.bin");
//    StreamWriter sw2("AudioProcessorTest_TestStream.mp3", compressor);
//    
//    vector<StreamWriter> sws;
//    sws.push_back(std::move(sw1));
//    sws.push_back(std::move(sw2));
//    
//    AudioProcessor ap(sws);
//    
//    
//    random_device rd;
//    mt19937 mt(rd());
//    uniform_real_distribution<double> dist(INT16_MIN, INT16_MAX);
//    std::int16_t buffer[1050];
//    int seconds = 10;
//    int nsamples = seconds * sampleRate;
//    //generate 5 seconds worth of samples
//    for (int i = 1; i <= nsamples; i++) {
//        int16_t sample = (int16_t)dist(mt);
//        buffer[(i - 1) % 1050] = sample;
//        
//        if (i % 1050 == 0) {
//            ap.process(buffer, 1050);
//        }
//    }
//    
//    ap.stop();
//}


//TEST(AudioProcessorTest, TestKillCompressorDueToLame) {
//    auto sampleRate = 44100;
//    auto compressor = make_shared<Mp3CompressorMock>();
//    auto raw = "AudioProcessorTest_TestKillCompressorDueToLame.bin";
//    auto mp3 = "AudioProcessorTest_TestKillCompressorDueToLame.mp3";
//    remove(raw);
//    remove(mp3);
//    std::int16_t t[] = {1,2,3,4,5};
//    
//    CAP::AudioBuffer in(t, 5);
//    CAP::AudioBuffer out(t, 5);
//    EXPECT_CALL(*compressor, process(in, out)).Times(AtLeast(1));
//    
//    vector<StreamWriter> sws = {};
//    sws.push_back(StreamWriter(raw));
//    sws.push_back(StreamWriter(mp3, compressor));
//    AudioProcessor ap(sws);
//    
//    
//    random_device rd;
//    mt19937 mt(rd());
//    uniform_real_distribution<double> dist(INT16_MIN, INT16_MAX);
//    std::int16_t buffer[4410];
//    int seconds = 5;
//    int nsamples = seconds * sampleRate;
//    int numExceptionsCaught = 0;
//    
//    for (int i = 1; i <= nsamples; i++) {
//        int16_t sample = (int16_t)dist(mt);
//        buffer[(i - 1) % 4410] = sample;
//        
//        if (i % (4410) == 0) {
//            try {
//                ap.process(buffer, 4410);
//            } catch (WriterProcessingException e) {
//                numExceptionsCaught++;
//            }
//        }
//    }
//    
//    ap.stop();
//    
//    ASSERT_EQ(numExceptionsCaught, 0);
//}


TEST(AudioProcessorTest, TestKillCompressorDueToSlow) {
    auto sampleRate = 44100;
    auto compressor = make_shared<Mp3Compressor>(9, sampleRate);
    string raw = "AudioProcessorTest_TestKillCompressorDueToSlow.bin";
    string mp3 = "AudioProcessorTest_TestKillCompressorDueToSlow.mp3";
    remove(raw.c_str());
    remove(mp3.c_str());
    
    
    vector<StreamWriter> sws = {};
    sws.push_back(StreamWriter(raw));
    sws.push_back(StreamWriter(mp3, compressor));
    AudioProcessor ap(sws);
    
    
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(INT16_MIN, INT16_MAX);
    std::int16_t buffer[4410];
    int seconds = 30;
    int nsamples = seconds * sampleRate;
    int bufferCount = 1;
    int numExceptionsCaught = 0;
    for (int i = 1; i <= nsamples; i++) {
        int16_t sample = (int16_t)dist(mt);
        buffer[(i - 1) % 4410] = sample;
        
        if (i % (4410) == 0) {
            
            try {
                ap.process(buffer, 4410);
            } catch (WriterProcessingException e) {
                numExceptionsCaught++;
            }
            
            bufferCount += 1;
            
        }
    }
    
    ap.stop();
    
    ASSERT_EQ(numExceptionsCaught, 1);
    ASSERT_EQ(ap.getNumberOfStreamWriters(), 1);
    
}




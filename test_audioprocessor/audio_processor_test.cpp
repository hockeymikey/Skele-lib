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
using ::testing::Return;

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
//    auto compressor = unique_ptr<SignalProcessor>(new Mp3Compressor(9, sampleRate));
//    remove("AudioProcessorTest_TestStream.bin");
//    remove("AudioProcessorTest_TestStream.mp3");
//    StreamWriter sw1("AudioProcessorTest_TestStream.bin");
//    StreamWriter sw2("AudioProcessorTest_TestStream.mp3", move(compressor));
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
//    int seconds = 1000;
//    int nsamples = seconds * sampleRate;
//    //generate 5 seconds worth of samples
//    for (int i = 1; i <= nsamples; i++) {
//        int16_t sample = (int16_t)dist(mt);
//        buffer[(i - 1) % 1050] = sample;
//        
//        if (i % 1050 == 0) {
//            ap.process(buffer, 1050);
//            this_thread::sleep_for(chrono::milliseconds(1));
//        }
//    }
//    
//    ap.stop();
//}


TEST(AudioProcessorTest, TestKillCompressorDueToLame) {
    auto sampleRate = 44100;
    auto compressor = unique_ptr<SignalProcessor>(new Mp3CompressorMock());
    auto raw = "AudioProcessorTest_TestKillCompressorDueToLame.bin";
    auto mp3 = "AudioProcessorTest_TestKillCompressorDueToLame.mp3";
    remove(raw);
    remove(mp3);
    
    Mp3CompressorMock *mp3Compressor = (Mp3CompressorMock *)compressor.get();
    
    EXPECT_CALL(*mp3Compressor, process(testing::_, testing::_)).Times(AtLeast(1)).WillRepeatedly(Return(false));
    
    vector<StreamWriter> sws = {};
    auto rawfile = unique_ptr<File>(new SystemFile(raw));
    auto mp3file = unique_ptr<File>(new SystemFile(mp3));
    sws.push_back(StreamWriter(move(rawfile)));
    sws.push_back(StreamWriter(move(mp3file), move(compressor)));
    AudioProcessor ap(sws);
    
    
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(INT16_MIN, INT16_MAX);
    std::int16_t buffer[4410];
    int seconds = 11;
    int nsamples = seconds * sampleRate;
    bool compressorErrorOccurred = false;
    bool rawWriterErrorOccurred = false;
    
    for (int i = 1; i <= nsamples; i++) {
        int16_t sample = (int16_t)dist(mt);
        buffer[(i - 1) % 4410] = sample;
        
        if (i % (4410) == 0) {
            auto result = ap.process(buffer, 4410);
            if (result == AudioProcessor::ProcessResult::NonPriorityWriterError) {
                compressorErrorOccurred = true;
            } else if (result == AudioProcessor::ProcessResult::PriorityWriterError) {
                rawWriterErrorOccurred = true;
                break;
            }
            this_thread::sleep_for(chrono::microseconds(10));
        }
    }
    
    ap.stop();
    
    ASSERT_EQ(compressorErrorOccurred, true);
    ASSERT_EQ(rawWriterErrorOccurred, false);
    ASSERT_EQ(ap.getNumberOfStreamWriters(), 1);
    
}


TEST(AudioProcessorTest, TestKillCompressorDueToSlow) {
    auto sampleRate = 44100;
    auto compressor = unique_ptr<SignalProcessor>(new Mp3Compressor(9, sampleRate));
    string raw = "AudioProcessorTest_TestKillCompressorDueToSlow.bin";
    string mp3 = "AudioProcessorTest_TestKillCompressorDueToSlow.mp3";
    remove(raw.c_str());
    remove(mp3.c_str());
    
    
    vector<StreamWriter> sws = {};
    auto rawfile = unique_ptr<File>(new SystemFile(raw));
    auto mp3file = unique_ptr<File>(new SystemFile(mp3));
    
    sws.push_back(StreamWriter(move(rawfile)));
    sws.push_back(StreamWriter(move(mp3file), move(compressor)));
    AudioProcessor ap(sws);
    
    
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(INT16_MIN, INT16_MAX);
    std::int16_t buffer[4410];
    int seconds = 30;
    int nsamples = seconds * sampleRate;
    int bufferCount = 0;
    int compressorErrorOccurred = false;
    for (int i = 1; i <= nsamples; i++) {
        int16_t sample = (int16_t)dist(mt);
        buffer[(i - 1) % 4410] = sample;
        
        if (i % (4410) == 0) {
            
            if (ap.process(buffer, 4410) == AudioProcessor::ProcessResult::NonPriorityWriterError) {
                compressorErrorOccurred = true;
            }
            bufferCount += 1;
        }
    }
    
    ap.stop();
    
    ASSERT_EQ(compressorErrorOccurred, true);
    ASSERT_EQ(ap.getNumberOfStreamWriters(), 1);
    ASSERT_EQ(bufferCount, nsamples / 4410);
}




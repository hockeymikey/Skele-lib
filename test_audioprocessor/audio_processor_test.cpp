#include "gmock/gmock.h"
#include "file_mock.hpp"
#include "mp3_compressor_mock.hpp"
#include "audio_processor.hpp"
#include "audio_processor_test.hpp"
#include "mp3_compressor.hpp"
#include "pcm_processor.hpp"
#include <vector>
#include <random>
#include <cstdio>

using namespace CAP;
using namespace std;
using ::testing::AtLeast;
using ::testing::Return;
using ::testing::NiceMock;

AudioProcessorTest::AudioProcessorTest() {
}

AudioProcessorTest::~AudioProcessorTest() {};

void AudioProcessorTest::SetUp() {};

void AudioProcessorTest::TearDown() {};


TEST(AudioProcessorTest, SchedulePostProcess) {
    auto bin = "AudioProcessorTest_SchedulePostProcess.wav";
    auto bin2 = "AudioProcessorTest_SchedulePostProcess2.wav";
    
    remove(bin);
    remove(bin2);
    
    auto file1 = unique_ptr<File>(new SystemFile(bin));
    auto file2 = unique_ptr<File>(new SystemFile(bin2));
    
    auto pcmProcessor = unique_ptr<SignalProcessor>(new PcmProcessor());
    auto sw1 = make_shared<StreamWriter>(move(file1), move(pcmProcessor));
    vector<shared_ptr<StreamWriter>> vec;
    vec.push_back(sw1);
    AudioProcessor ap(vec);
    int16_t samples[100] = {};
    
    ap.processBuffer(samples, 100);
    auto compressor = unique_ptr<SignalProcessor>(new Mp3Compressor(10, 44100));
    
    bool callbackCalled = false;
//    auto sw1 = ap.getCurrentPriorityStreamWriter();
    auto sw2 = make_shared<StreamWriter>(move(file2), move(compressor));
    vector<shared_ptr<StreamWriter>> vec2;
    vec2.push_back(sw2);
    
    ap.schedulePostProcess(vec2, [&callbackCalled] () {
        callbackCalled = true;
    });
    for (uint8_t i = 0; i < 100; i++) {
        int16_t samples2[100] = {};
        ap.processBuffer(samples2, 100);
    }
//    auto sw2 = ap.getCurrentPriorityStreamWriter();
    
    ap.stop([] {});

    ASSERT_EQ(1, *(sw1->numberOfBuffersWritten().get()));
    ASSERT_EQ(100, *(sw2->numberOfBuffersWritten().get()));
    ASSERT_TRUE(callbackCalled);
}

TEST(AudioProcessorTest, StreamFailureCantOpenNonPriorityStreamKillIt) {
    auto bin = "AudioProcessorTest_StreamFailureCantOpenNonPriorityStreamKillIt.wav";
    remove(bin);
    
    
    auto mp3file = unique_ptr<File>(new NiceMock<FileMock>());
    NiceMock<FileMock> *mock = (NiceMock<FileMock> *)mp3file.get();
    ON_CALL(*mock, isOpen()).WillByDefault(Return(false));
    ON_CALL(*mock, path()).WillByDefault(Return("mock/mp3/path"));
    
    auto binfile = unique_ptr<File>(new SystemFile(bin));
    auto pcmProcessor = unique_ptr<SignalProcessor>(new PcmProcessor());
    
    auto sw1 = make_shared<StreamWriter>(move(binfile), move(pcmProcessor));
    auto sw2 = make_shared<StreamWriter>(move(mp3file), move(pcmProcessor));
    
    vector<shared_ptr<StreamWriter>> vec;
    vec.push_back(sw1);
    vec.push_back(sw2);
    
    AudioProcessor ap(vec);
    int i = 0;
    bool nonpriorityError = false;
    while (i < 1000) {
        int16_t t[3000];
        AudioProcessor::ProcessResult result = ap.processBuffer(t, 3000);
        if (result == AudioProcessor::ProcessResult::NonPriorityWriterError) {
            nonpriorityError = true;
        }
        i++;
        this_thread::sleep_for(chrono::microseconds(10));
    }
    
//    auto sw1 = ap.getCurrentPriorityStreamWriter();
//    auto sw2 = ap.getCurrentPriorityStreamWriter() + 1;
    ASSERT_TRUE(sw1->isWriteable());
    ASSERT_FALSE(sw2->isWriteable());
    
    ap.stop([] {});
    
    ASSERT_EQ(sw1->queueSize(), 0);
    ASSERT_GE(sw2->queueSize(), 0);
    ASSERT_TRUE(nonpriorityError);
    
}

TEST(AudioProcessorTest, StreamFailureCantOpenPriorityFileKillAllStreams) {
    auto mp3 = "AudioProcessorTest_StreamFailureCantOpenPriorityFileKillAllStreams.wav";
    remove(mp3);

    auto rawfile = unique_ptr<File>(new NiceMock<FileMock>());
    NiceMock<FileMock> *mock = (NiceMock<FileMock> *)rawfile.get();
    ON_CALL(*mock, isOpen()).WillByDefault(Return(false));
    ON_CALL(*mock, path()).WillByDefault(Return("mock/path"));
    
    auto mp3file = unique_ptr<File>(new SystemFile(mp3));
    auto pcmProcessor = unique_ptr<SignalProcessor>(new PcmProcessor());
    auto pcmProcessor2 = unique_ptr<SignalProcessor>(new PcmProcessor());
    
    auto sw1 = make_shared<StreamWriter>(move(rawfile), move(pcmProcessor));
    auto sw2 = make_shared<StreamWriter>(move(mp3file), move(pcmProcessor2));
    
    vector<shared_ptr<StreamWriter>> vec;
    vec.push_back(sw1);
    vec.push_back(sw2);
    
    AudioProcessor ap(vec);
    
    while (true) {
        int16_t t[3000];
        auto result = ap.processBuffer(t, 3000);
        if (result == AudioProcessor::ProcessResult::PriorityWriterError) {
            break;
        }
//        int i = 0;
//        for(auto &sw: sws) {
//            auto numBuffersWritten = sw.numberOfBuffersWritten();
//            cout << i << ":" << *numBuffersWritten << " q:" << sw.queueSize() << endl;
//            i++;
//        }
        
        this_thread::sleep_for(chrono::microseconds(10));
    }
    
    ASSERT_FALSE(sw1->isWriteable());
    ASSERT_FALSE(sw2->isWriteable());
    
    ap.stop([] {});
    
}

TEST(AudioProcessorTest, TestKillCompressorDueToLame) {
    auto sampleRate = 44100;
    auto compressor = unique_ptr<SignalProcessor>(new Mp3CompressorMock());
    auto pcmProcessor = unique_ptr<SignalProcessor>(new PcmProcessor());
    auto raw = "AudioProcessorTest_TestKillCompressorDueToLame.wav";
    auto mp3 = "AudioProcessorTest_TestKillCompressorDueToLame.mp3";
    remove(raw);
    remove(mp3);
    
    Mp3CompressorMock *mp3Compressor = (Mp3CompressorMock *)compressor.get();
    
    EXPECT_CALL(*mp3Compressor, process(testing::_, testing::_)).Times(AtLeast(1)).WillRepeatedly(Return(false));
    
    
    auto rawfile = unique_ptr<File>(new SystemFile(raw));
    auto mp3file = unique_ptr<File>(new SystemFile(mp3));
    
    auto s1 = make_shared<StreamWriter>(move(rawfile), move(pcmProcessor));
    auto s2 = make_shared<StreamWriter>(move(mp3file), move(compressor));
    
    vector<shared_ptr<StreamWriter>> sw;
    sw.push_back(s1);
    sw.push_back(s2);
    
    AudioProcessor ap(sw);
    
    
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
            auto result = ap.processBuffer(buffer, 4410);
            if (result == AudioProcessor::ProcessResult::NonPriorityWriterError) {
                compressorErrorOccurred = true;
            } else if (result == AudioProcessor::ProcessResult::PriorityWriterError) {
                rawWriterErrorOccurred = true;
            }
            this_thread::sleep_for(chrono::microseconds(10));
        }
    }
    
    ASSERT_TRUE(s1->isWriteable());
    ASSERT_FALSE(s2->isWriteable());
    
    ap.stop([] {});
    
    ASSERT_EQ(compressorErrorOccurred, true);
    ASSERT_EQ(rawWriterErrorOccurred, false);
    
    
}


TEST(AudioProcessorTest, TestKillCompressorDueToSlow) {
    auto sampleRate = 44100;
    auto compressor = unique_ptr<SignalProcessor>(new Mp3Compressor(9, sampleRate));
    auto pcmProcessor = unique_ptr<SignalProcessor>(new PcmProcessor());
    string raw = "AudioProcessorTest_TestKillCompressorDueToSlow.wav";
    string mp3 = "AudioProcessorTest_TestKillCompressorDueToSlow.mp3";
    remove(raw.c_str());
    remove(mp3.c_str());
    
    
    auto rawfile = unique_ptr<File>(new SystemFile(raw));
    auto mp3file = unique_ptr<File>(new SystemFile(mp3));
   
    auto sw1 = make_shared<StreamWriter>(move(rawfile), move(pcmProcessor));
    auto sw2 = make_shared<StreamWriter>(move(mp3file), move(compressor));
    
    vector<shared_ptr<StreamWriter>> vec;
    vec.push_back(sw1);
    vec.push_back(sw2);
    
    AudioProcessor ap(vec);
    
    
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
            
            if (ap.processBuffer(buffer, 4410) == AudioProcessor::ProcessResult::NonPriorityWriterError) {
                compressorErrorOccurred = true;
            }
            bufferCount += 1;
        }
    }
    
    ASSERT_TRUE(sw1->isWriteable());
    ASSERT_FALSE(sw2->isWriteable());
    
    ap.stop([] {});
    
    ASSERT_EQ(compressorErrorOccurred, true);
    
    ASSERT_EQ(bufferCount, nsamples / 4410);
}


//TEST(AudioProcessorTest, TestStream) {
//    auto sampleRate = 44100;
//    auto pcmprocessor = unique_ptr<SignalProcessor>(new PcmProcessor());
//    auto compressor = unique_ptr<SignalProcessor>(new Mp3Compressor(9, sampleRate));
//    
//    auto raw = "AudioProcessorTest_TestStream.bin";
//    auto mp3 = "AudioProcessorTest_TestStream.mp3";
//    
//    remove(raw);
//    remove(mp3);
//    auto rawfile = unique_ptr<File>(new SystemFile(raw));
//    auto mp3file = unique_ptr<File>(new SystemFile(mp3));
//    
//    auto sw1 = make_shared<StreamWriter>(move(rawfile), move(pcmprocessor));
//    auto sw2 = make_shared<StreamWriter>(move(mp3file), move(compressor));
//    
//    vector<shared_ptr<StreamWriter>> vec;
//    
//    vec.push_back(sw1);
//    vec.push_back(sw2);
//    
//    AudioProcessor ap(vec);
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
//            ap.processBuffer(buffer, 1050);
//            this_thread::sleep_for(chrono::milliseconds(1));
//        }
//    }
//    
//    ap.stop([] {});
//}



#include "gmock/gmock.h"
#include "file_mock.hpp"
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
using ::testing::NiceMock;

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
    StreamWriter sws[] = {};
    EXPECT_THROW(AudioProcessor ap(sws, 0), runtime_error);
}

TEST(AudioProcessorTest, TooManyStreamWriters) {
    StreamWriter sws[] = {StreamWriter(unique_ptr<File>(new FileMock())),StreamWriter(unique_ptr<File>(new FileMock())),StreamWriter(unique_ptr<File>(new FileMock())),StreamWriter(unique_ptr<File>(new FileMock())),StreamWriter(unique_ptr<File>(new FileMock())),StreamWriter(unique_ptr<File>(new FileMock()))};
    EXPECT_THROW(AudioProcessor ap(sws, 6), runtime_error);
}

TEST(AudioProcessorTest, StreamFailureCantOpenNonPriorityStreamKillIt) {
    auto bin = "AudioProcessorTest_StreamFailureCantOpenNonPriorityStreamKillIt.bin";
    remove(bin);
    
    
    auto mp3file = unique_ptr<File>(new NiceMock<FileMock>());
    NiceMock<FileMock> *mock = (NiceMock<FileMock> *)mp3file.get();
    ON_CALL(*mock, isOpen()).WillByDefault(Return(false));
    ON_CALL(*mock, path()).WillByDefault(Return("mock/mp3/path"));
    
    auto binfile = unique_ptr<File>(new SystemFile(bin));
    
    
    StreamWriter sws[] = {StreamWriter(move(binfile)), StreamWriter(move(mp3file))};
    
    AudioProcessor ap(sws, 2);
    int i = 0;
    while (i < 1000) {
        int16_t t[3000];
        ap.processBuffer(t, 3000);
        i++;
        this_thread::sleep_for(chrono::microseconds(10));
    }
    
    ASSERT_TRUE(sws[0].isWriteable());
    ASSERT_FALSE(sws[1].isWriteable());
    
    ap.stop();
    
    ASSERT_EQ(sws[0].queueSize(), 0);
    ASSERT_GE(sws[1].queueSize(), 0);
    
}

TEST(AudioProcessorTest, StreamFailureCantOpenPriorityFileKillAllStreams) {
    auto mp3 = "AudioProcessorTest_StreamFailureCantOpenPriorityFileKillAllStreams.bin";
    remove(mp3);

    auto rawfile = unique_ptr<File>(new NiceMock<FileMock>());
    NiceMock<FileMock> *mock = (NiceMock<FileMock> *)rawfile.get();
    ON_CALL(*mock, isOpen()).WillByDefault(Return(false));
    ON_CALL(*mock, path()).WillByDefault(Return("mock/path"));
    
    auto mp3file = unique_ptr<File>(new SystemFile(mp3));
    
    
    StreamWriter sws[] = {StreamWriter(move(rawfile)), StreamWriter(move(mp3file))};

    AudioProcessor ap(sws, 2);
    
    while (true) {
        int16_t t[3000];
        auto result = ap.processBuffer(t, 3000);
        if (result == AudioProcessor::ProcessResult::PriorityWriterError) {
            break;
        }
        int i = 0;
        for(auto &sw: sws) {
            auto numBuffersWritten = sw.numberOfBuffersWritten();
            cout << i << ":" << *numBuffersWritten << " q:" << sw.queueSize() << endl;
            i++;
        }
        
        this_thread::sleep_for(chrono::microseconds(10));
    }
    
    ASSERT_FALSE(sws[0].isWriteable());
    ASSERT_FALSE(sws[1].isWriteable());
    
    ap.stop();
    
}

/*
TEST(AudioProcessorTest, TestStream) {
    auto sampleRate = 44100;
    auto compressor = unique_ptr<SignalProcessor>(new Mp3Compressor(9, sampleRate));
    
    auto raw = "AudioProcessorTest_TestStream.bin";
    auto mp3 = "AudioProcessorTest_TestStream.mp3";
    
    remove(raw);
    remove(mp3);
    auto rawfile = unique_ptr<File>(new SystemFile(raw));
    auto mp3file = unique_ptr<File>(new SystemFile(mp3));
    StreamWriter sw1(move(rawfile));
    StreamWriter sw2(move(mp3file), move(compressor));
    
    StreamWriter sws[] = {move(sw1), move(sw2)};
    
    AudioProcessor ap(sws, 2);
    
    
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(INT16_MIN, INT16_MAX);
    std::int16_t buffer[1050];
    int seconds = 1000;
    int nsamples = seconds * sampleRate;
    //generate 5 seconds worth of samples
    for (int i = 1; i <= nsamples; i++) {
        int16_t sample = (int16_t)dist(mt);
        buffer[(i - 1) % 1050] = sample;
        
        if (i % 1050 == 0) {
            ap.process(buffer, 1050);
            this_thread::sleep_for(chrono::milliseconds(1));
        }
    }
    
    ap.stop();
}
*/

TEST(AudioProcessorTest, TestKillCompressorDueToLame) {
    auto sampleRate = 44100;
    auto compressor = unique_ptr<SignalProcessor>(new Mp3CompressorMock());
    auto raw = "AudioProcessorTest_TestKillCompressorDueToLame.bin";
    auto mp3 = "AudioProcessorTest_TestKillCompressorDueToLame.mp3";
    remove(raw);
    remove(mp3);
    
    Mp3CompressorMock *mp3Compressor = (Mp3CompressorMock *)compressor.get();
    
    EXPECT_CALL(*mp3Compressor, process(testing::_, testing::_)).Times(AtLeast(1)).WillRepeatedly(Return(false));
    
    
    auto rawfile = unique_ptr<File>(new SystemFile(raw));
    auto mp3file = unique_ptr<File>(new SystemFile(mp3));
    
    StreamWriter sw[] = {StreamWriter(move(rawfile)), StreamWriter(move(mp3file), move(compressor))};
    AudioProcessor ap(sw, 2);
    
    
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
    
    ASSERT_TRUE(sw[0].isWriteable());
    ASSERT_FALSE(sw[1].isWriteable());
    
    ap.stop();
    
    ASSERT_EQ(compressorErrorOccurred, true);
    ASSERT_EQ(rawWriterErrorOccurred, false);
    
    
}


TEST(AudioProcessorTest, TestKillCompressorDueToSlow) {
    auto sampleRate = 44100;
    auto compressor = unique_ptr<SignalProcessor>(new Mp3Compressor(9, sampleRate));
    string raw = "AudioProcessorTest_TestKillCompressorDueToSlow.bin";
    string mp3 = "AudioProcessorTest_TestKillCompressorDueToSlow.mp3";
    remove(raw.c_str());
    remove(mp3.c_str());
    
    
    auto rawfile = unique_ptr<File>(new SystemFile(raw));
    auto mp3file = unique_ptr<File>(new SystemFile(mp3));
    
    StreamWriter sws[] = {
        StreamWriter(move(rawfile)),
        StreamWriter(move(mp3file), move(compressor))
    };
    AudioProcessor ap(sws, 2);
    
    
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
    
    
    ASSERT_TRUE(sws[0].isWriteable());
    ASSERT_FALSE(sws[1].isWriteable());
    
    ap.stop();
    
    ASSERT_EQ(compressorErrorOccurred, true);
    
    ASSERT_EQ(bufferCount, nsamples / 4410);
}




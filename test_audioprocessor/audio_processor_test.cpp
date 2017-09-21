#include "gmock/gmock.h"
#include "file_mock.hpp"
#include "mp3_compressor_mock.hpp"
#include "audio_processor.hpp"
#include "audio_processor_test.hpp"
#include "mp3_compressor.hpp"
#include "pcm_processor.hpp"
#include "test_stream_writer_observer.hpp"
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
    auto swo1 = make_shared<TestStreamWriterObserver>();
    sw1->streamWriterObserver = swo1;
    
    AudioProcessor ap(unique_ptr<AbstractCircularQueue>(new CircularQueue<31 * 44100>));
    ap.startHighlight(vec, 15 * 44100);
    int16_t samples[100] = {};
    
    ap.processSamples(samples, 100);
    
    
    
    pcmProcessor = unique_ptr<SignalProcessor>(new PcmProcessor());
    auto sw2 = make_shared<StreamWriter>(move(file2), move(pcmProcessor));
    vector<shared_ptr<StreamWriter>> vec2;
    auto swo2 = make_shared<TestStreamWriterObserver>();
    sw2->streamWriterObserver = swo2;
    vec2.push_back(sw2);
    
    ap.stopHighlight(false);
    
    ap.startHighlight(vec2, 0);
    
    for (uint8_t i = 0; i < 100; i++) {
        int16_t samples2[100] = {};
        ap.processSamples(samples2, 100);
    }

    
    ap.stopHighlight(true);
    
    while (!swo1->streamWriterDidStop || !swo2->streamWriterDidStop) {
        this_thread::sleep_for(chrono::milliseconds(10));
    }

    ASSERT_EQ(0, *(sw1->numberOfSamplesWritten().get()));
    ASSERT_EQ(10100, *(sw2->numberOfSamplesWritten().get()));
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
    
    auto swo1 = make_shared<TestStreamWriterObserver>();
    auto swo2 = make_shared<TestStreamWriterObserver>();
    
    sw1->streamWriterObserver = swo1;
    sw2->streamWriterObserver = swo2;

    
    vector<shared_ptr<StreamWriter>> vec;
    vec.push_back(sw1);
    vec.push_back(sw2);
    
    AudioProcessor ap(unique_ptr<AbstractCircularQueue>(new CircularQueue<31 * 44100>));
    ap.startHighlight(vec, 10 * 44100);
    int i = 0;
    bool nonpriorityError = false;
    while (i < 1000) {
        int16_t t[3000];
        auto result = ap.processSamples(t, 3000);
        if (result == AudioProcessor::Status::NonPriorityWriterError) {
            nonpriorityError = true;
        }
        i++;
        this_thread::sleep_for(chrono::microseconds(10));
    }
    
    
    ap.stopHighlight(true);
    
    while (!swo1->streamWriterDidStop || !swo2->streamWriterDidStop) {
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    
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
    
    auto swo1 = make_shared<TestStreamWriterObserver>();
    auto swo2 = make_shared<TestStreamWriterObserver>();
    
    sw1->streamWriterObserver = swo1;
    sw2->streamWriterObserver = swo2;
    
    vector<shared_ptr<StreamWriter>> vec;
    vec.push_back(sw1);
    vec.push_back(sw2);
    
    AudioProcessor ap(unique_ptr<AbstractCircularQueue>(new CircularQueue<31 * 44100>));
    
    ap.startHighlight(vec, 15 * 44100);
    
    while (true) {
        int16_t t[3000];
        auto result = ap.processSamples(t, 3000);
        if (result == AudioProcessor::Status::PriorityWriterError) {
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
    
    ap.stopHighlight(true);
    
    while (!swo1->streamWriterDidStop || !swo2->streamWriterDidStop) {
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    
    ASSERT_TRUE(swo1->streamWriterDidKill);
    ASSERT_TRUE(swo2->streamWriterDidKill);
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
    
    auto swo1 = make_shared<TestStreamWriterObserver>();
    auto swo2 = make_shared<TestStreamWriterObserver>();
    
    s1->streamWriterObserver = swo1;
    s2->streamWriterObserver = swo2;
    
    vector<shared_ptr<StreamWriter>> sw;
    sw.push_back(s1);
    sw.push_back(s2);
    
    AudioProcessor ap(unique_ptr<AbstractCircularQueue>(new CircularQueue<31 * 44100>));
    
    ap.startHighlight(sw, 10 * 44100);
    
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(INT16_MIN, INT16_MAX);
    std::int16_t buffer[4410];
    int seconds = 30;
    int nsamples = seconds * sampleRate;
    bool compressorErrorOccurred = false;
    bool rawWriterErrorOccurred = false;
    
    for (int i = 1; i <= nsamples; i++) {
        int16_t sample = (int16_t)dist(mt);
        buffer[(i - 1) % 4410] = sample;
        
        if (i % (4410) == 0) {
            auto result = ap.processSamples(buffer, 4410);
            if (result == CAP::AudioProcessor::Status::NonPriorityWriterError) {
                compressorErrorOccurred = true;
            } else if (result == CAP::AudioProcessor::Status::PriorityWriterError) {
                rawWriterErrorOccurred = true;
            }
            this_thread::sleep_for(chrono::microseconds(10));
        }
    }
    
    ap.stopHighlight(true);
    
    
    while (!swo1->streamWriterDidStop || !swo2->streamWriterDidStop) {
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    
//    ASSERT_TRUE(s1->isWriteable());
//    ASSERT_FALSE(s2->isWriteable());
    
    
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
   
    auto swo1 = make_shared<TestStreamWriterObserver>();
    auto swo2 = make_shared<TestStreamWriterObserver>();
    auto sw1 = make_shared<StreamWriter>(move(rawfile), move(pcmProcessor));
    sw1->streamWriterObserver = swo1;
    auto sw2 = make_shared<StreamWriter>(move(mp3file), move(compressor));
    sw2->streamWriterObserver = swo2;
    
    vector<shared_ptr<StreamWriter>> vec;
    vec.push_back(sw1);
    vec.push_back(sw2);
    
    AudioProcessor ap(unique_ptr<AbstractCircularQueue>(new CircularQueue<31 * 44100>));
    ap.startHighlight(vec, 10 * 44100);
    
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(INT16_MIN, INT16_MAX);
    std::int16_t buffer[4410];
    int seconds = 40;
    int nsamples = seconds * sampleRate;
    int bufferCount = 0;
    int compressedBufferCount = 0;
    auto priorityError = false;
    auto compressorErrorOccurred = false;
    auto fullBuffer = false;
    for (int i = 1; i <= nsamples; i++) {
        int16_t sample = (int16_t)dist(mt);
        buffer[(i - 1) % 4410] = sample;
        
        if (i % (4410) == 0) {
            size_t nsamples = 4410;
            
            auto status = ap.processSamples(buffer, nsamples);
            if (status == AudioProcessor::Status::NonPriorityWriterError) {
                compressorErrorOccurred = true;
                compressedBufferCount += 1;
            } else if (status == AudioProcessor::Status::FullBuffer) {
                fullBuffer = true;
                break;
            } else if (status == AudioProcessor::Status::PriorityWriterError) {
                priorityError = true;
                break;
            }
            bufferCount += 1;
        }
    }
    
    ap.stopHighlight(true);
    
    while (!swo1->streamWriterDidStop || !swo2->streamWriterDidStop) {
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    
    ASSERT_EQ(*(sw1->numberOfBuffersWritten()), bufferCount);
//    ASSERT_EQ(*(sw2->numberOfBuffersWritten()), 100);
    
    
    ASSERT_TRUE(compressorErrorOccurred);
    ASSERT_FALSE(fullBuffer);
    ASSERT_FALSE(priorityError);
    
    ASSERT_EQ(bufferCount, nsamples / 4410);
}


TEST(AudioProcessorTest, TestNoHighlightError) {
    auto sampleRate = 44100;
    auto compressor = unique_ptr<SignalProcessor>(new Mp3Compressor(9, sampleRate));
    auto pcmProcessor = unique_ptr<SignalProcessor>(new PcmProcessor());
    string raw = "AudioProcessorTest_TestNoHighlightError.wav";
    string mp3 = "AudioProcessorTest_TestNoHighlightError.mp3";
    remove(raw.c_str());
    remove(mp3.c_str());
    
    
    auto rawfile = unique_ptr<File>(new SystemFile(raw));
    auto mp3file = unique_ptr<File>(new SystemFile(mp3));
    
    auto swo1 = make_shared<TestStreamWriterObserver>();
    auto swo2 = make_shared<TestStreamWriterObserver>();
    auto sw1 = make_shared<StreamWriter>(move(rawfile), move(pcmProcessor));
    sw1->streamWriterObserver = swo1;
    auto sw2 = make_shared<StreamWriter>(move(mp3file), move(compressor));
    sw2->streamWriterObserver = swo2;
    
    vector<shared_ptr<StreamWriter>> vec;
    vec.push_back(sw1);
    vec.push_back(sw2);
    
    AudioProcessor ap(unique_ptr<AbstractCircularQueue>(new CircularQueue<31 * 44100>));
//    ap.startHighlight(vec, 10);
    
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(INT16_MIN, INT16_MAX);
    std::int16_t buffer[4410];
    int seconds = 40;
    int nsamples = seconds * sampleRate;
    int bufferCount = 0;
    
    auto noHighlightError = false;
    
    for (int i = 1; i <= nsamples; i++) {
        int16_t sample = (int16_t)dist(mt);
        buffer[(i - 1) % 4410] = sample;
        
        if (i % (4410) == 0) {
            size_t nsamples = 4410;
            
            auto status = ap.processSamples(buffer, nsamples);
            if (status == AudioProcessor::Status::Success) {
                noHighlightError = true;
                
            }
            bufferCount += 1;
        }
    }
    
    ASSERT_EQ(ap.circularQueueSize(), 31 * 44100);
    
    ap.stopHighlight(true);
    
    ASSERT_EQ(0, ap.circularQueueSize());
    ASSERT_TRUE(noHighlightError);
    
}


TEST(AudioProcessorTest, TestBeginningOFAudioBeingAppendedToEndOfFile) {
    
    auto pcmProcessor = unique_ptr<SignalProcessor>(new PcmProcessor());
    string raw = "AudioProcessorTest_TestBeginningOFAudioBeingAppendedToEndOfFile.wav";
    remove(raw.c_str());
    
    
    auto rawfile = unique_ptr<File>(new SystemFile(raw));
    
    
    auto swo1 = make_shared<TestStreamWriterObserver>();
    auto sw1 = make_shared<StreamWriter>(move(rawfile), move(pcmProcessor));
    sw1->streamWriterObserver = swo1;
    
    
    vector<shared_ptr<StreamWriter>> empty;
    vector<shared_ptr<StreamWriter>> vec;
    vec.push_back(sw1);
    
    
    AudioProcessor ap(unique_ptr<AbstractCircularQueue>(new CircularQueue<10>));
    ap.startHighlight(empty, 5);
    
    
    int16_t pcm[] = {1,2,3,4,5,6,7};
    
    
    auto status = ap.processSamples(pcm, 7);
    ASSERT_EQ(status, AudioProcessor::Status::Success);
    
    ap.stopHighlight(false);
    ap.startHighlight(vec, 0);
    ap.stopHighlight(true);
    
    while (!swo1->streamWriterDidStop) {
        this_thread::sleep_for(chrono::milliseconds(10));
    }
    
    
    
    ifstream file(raw, ifstream::binary | ifstream::in);
    
    char byte;
    ASSERT_TRUE(file.read(&byte, 44));
    
    ASSERT_TRUE(file.read(&byte, sizeof(int16_t)));
    auto sample = reinterpret_cast<int16_t&>(byte);
    ASSERT_EQ(sample, 3);
    
    ASSERT_TRUE(file.read(&byte, sizeof(int16_t)));
    sample = reinterpret_cast<int16_t&>(byte);
    ASSERT_EQ(sample, 4);
    
    ASSERT_TRUE(file.read(&byte, sizeof(int16_t)));
    sample = reinterpret_cast<int16_t&>(byte);
    ASSERT_EQ(sample, 5);
    
    ASSERT_TRUE(file.read(&byte, sizeof(int16_t)));
    sample = reinterpret_cast<int16_t&>(byte);
    ASSERT_EQ(sample, 6);
    
    ASSERT_TRUE(file.read(&byte, sizeof(int16_t)));
    sample = reinterpret_cast<int16_t&>(byte);
    ASSERT_EQ(sample, 7);
    
    ASSERT_FALSE(file.read(&byte, sizeof(int16_t)));
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
//    
//    AudioProcessor ap(unique_ptr<AbstractCircularQueue>(new CircularQueue<31 * 44100>));
//    
//    
//    ap.startHighlight(vec, 15);
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
//            size_t nsamples = 1050;
//            ap.processSamples(buffer, nsamples);
//            
////            this_thread::sleep_for(chrono::milliseconds(1));
//        }
//    }
//    
//    
//}



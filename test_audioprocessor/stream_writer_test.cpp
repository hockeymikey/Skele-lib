//
//  StreamWriterTest.cpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/2/17.
//
//


#include <fstream>
#include <future>
#include <chrono>
#include <random>
#include <cstdio>
#include "system_file.hpp"
#include "stream_writer_test.hpp"
#include "mp3_compressor.hpp"
#include "pcm_processor.hpp"
#include "test_stream_writer_observer.hpp"

using namespace std;
using namespace CAP;


StreamWriterTest::StreamWriterTest() {
    
}

StreamWriterTest::~StreamWriterTest() {};

void StreamWriterTest::SetUp() {};

void StreamWriterTest::TearDown() {};

TEST(StreamWriterTest, TestCompressing) {
    auto testEventHandler = make_shared<TestStreamWriterObserver>();
    
    string filename = "StreamWriterTest_TestCompressing.mp3";
    remove(filename.c_str());
    auto sampleRate = 44100;
    auto compressor = make_shared<Mp3Compressor>(5, sampleRate);
    auto file = unique_ptr<File>(new SystemFile(filename));
    StreamWriter streamWriter(move(file), move(compressor));
    streamWriter.streamWriterObserver = testEventHandler;
    streamWriter.start();
    
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(INT16_MIN, INT16_MAX);

    
    int seconds = 5;
    int nsamples = seconds * sampleRate;
    std::int16_t buffer[1050];
    int numberOfBuffersEnqueued = 0;
    //generate 5 seconds worth of samples
    for (int i = 1; i <= nsamples; i++) {
        int16_t sample = (int16_t)dist(mt);
        
        buffer[(i - 1) % 1050] = sample;

        if (i % 1050 == 0) {
            streamWriter.enqueue(AudioBuffer(buffer, 1050));
            numberOfBuffersEnqueued += 1;
        }
    }
    
    streamWriter.stop();
    
    while (!testEventHandler->streamWriterDidStop) {
        this_thread::sleep_for(chrono::milliseconds(1));
    }

    
    
    ASSERT_EQ(numberOfBuffersEnqueued, *(streamWriter.numberOfBuffersWritten()));
    

    std::ifstream in(filename, ifstream::ate | ifstream::binary);

    int bitrate = 128000 / 2;
    int bytesPerSecond = bitrate / 8;

    int filesize = seconds * bytesPerSecond;

    ASSERT_NEAR(in.tellg(), filesize, 1000);
}

TEST(StreamWriterTest, TestUngracefullStop) {
    string filename = "StreamWriterTest_TestUngracefullStop.wav";
    remove(filename.c_str());
    auto sampleRate = 44100;
    auto compressor = make_shared<Mp3Compressor>(9, sampleRate);
    auto file = unique_ptr<File>(new SystemFile(filename));
    StreamWriter streamWriter(move(file), move(compressor));
    auto testEventHandler = make_shared<TestStreamWriterObserver>();
    streamWriter.streamWriterObserver = testEventHandler;
    streamWriter.start();
    
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(INT16_MIN, INT16_MAX);
    
    
    int seconds = 60;
    int nsamples = seconds * sampleRate;
    std::int16_t buffer[1050];
    int numberOfBuffersEnqueued = 0;
    //generate 5 seconds worth of samples
    for (int i = 1; i <= nsamples; i++) {
        int16_t sample = (int16_t)dist(mt);
        
        buffer[(i - 1) % 1050] = sample;
        
        if (i % 1050 == 0) {
            streamWriter.enqueue(AudioBuffer(buffer, 1050));
            numberOfBuffersEnqueued += 1;
        }
    }
    
    streamWriter.kill();
    
    while (!testEventHandler->streamWriterDidKill) {
        this_thread::sleep_for(chrono::milliseconds(1));
    }
    
    
    ASSERT_GT(numberOfBuffersEnqueued, *(streamWriter.numberOfBuffersWritten()));
    cout << numberOfBuffersEnqueued << ":" << *(streamWriter.numberOfBuffersWritten());
    cout << endl;
}

TEST(StreamWriterTest, WriteRawAudioSamples) {
    
    auto testEventHandler = make_shared<TestStreamWriterObserver>();
    string filename = "StreamWriterTest_WriteRawAudioSamples.wav";
    remove(filename.c_str());
    auto file = unique_ptr<File>(new SystemFile(filename));
    auto pcmProcessor = make_shared<PcmProcessor>();
    StreamWriter streamWriter(move(file), move(pcmProcessor));
    
    streamWriter.streamWriterObserver = testEventHandler;

    std::int16_t testSamples[] = {30000, -12200, -12, 400, 5000};
    streamWriter.start();
    streamWriter.enqueue(AudioBuffer(testSamples, 5));
    streamWriter.stop();
    
    while (!testEventHandler->streamWriterDidStop) {
        this_thread::sleep_for(chrono::milliseconds(1));
    }
    

    
    ifstream instream(filename, ifstream::binary | ifstream::in);
    
    char output;
    char header[44];
    instream.read(header, sizeof(char) * 44);
    
    instream.read(&output, sizeof(int16_t));
    int16_t outputSample = reinterpret_cast<int16_t&>(output);
    ASSERT_EQ(outputSample, 30000);

    instream.read(&output, sizeof(int16_t));
    outputSample = reinterpret_cast<int16_t&>(output);
    ASSERT_EQ(outputSample, -12200);
    
    instream.read(&output, sizeof(int16_t));
    outputSample = reinterpret_cast<int16_t&>(output);
    ASSERT_EQ(outputSample, -12);
    
    
    instream.read(&output, sizeof(int16_t));
    outputSample = reinterpret_cast<int16_t&>(output);
    ASSERT_EQ(outputSample, 400);
    
    instream.read(&output, sizeof(int16_t));
    outputSample = reinterpret_cast<int16_t&>(output);
    ASSERT_EQ(outputSample, 5000);

    ASSERT_EQ(instream.readsome(&output, sizeof(int16_t)), 0);
}


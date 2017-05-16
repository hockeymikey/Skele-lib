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
#include "stream_writer.hpp"
#include "stream_writer_test.hpp"
#include "mp3_compressor.hpp"


using namespace std;
using namespace CAP;

StreamWriterTest::StreamWriterTest() {
}

StreamWriterTest::~StreamWriterTest() {};

void StreamWriterTest::SetUp() {};

void StreamWriterTest::TearDown() {};



TEST(StreamWriterTest, TestCompressing) {
    string filename = "StreamWriterTest_TestCompressing.mp3";
    shared_ptr<Mp3Compressor> compressor( new Mp3Compressor(5));
    
    StreamWriter streamWriter(filename, compressor);
    
    streamWriter.start();
    
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
            streamWriter.enqueue(buffer);
            buffer.clear();
        }
    }

    auto future = streamWriter.stop();
    
    future.get();
    
    
    
    this_thread::sleep_for(chrono::seconds(0));


    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);

    int bitrate = 128000;
    int bytesPerSecond = bitrate / 8;

    int filesize = seconds * bytesPerSecond;

    ASSERT_NEAR(in.tellg(), filesize, 1000);
}

TEST(StreamWriterTest, WriteRawAudioSamples) {
    string filename = "WriteAudioSamplesBatch_test_file.bin";
    
    StreamWriter streamWriter(filename);

    vector<int16_t> testSamples = {30000, -12200, -12, 400, 5000};
    streamWriter.start();
    streamWriter.enqueue(testSamples);
    auto future = streamWriter.stop();
    
    future.get();

    
    this_thread::sleep_for(chrono::seconds(0));
    
    
    ifstream instream(filename, ios_base::binary | ios_base::in);
    char output;
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


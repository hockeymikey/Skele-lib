//
//  StreamWriterTest.cpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/2/17.
//
//

#include <fstream>
#include "StreamWriter.hpp"
#include "StreamWriterTest.hpp"


using namespace std;

StreamWriterTest::StreamWriterTest() {
}

StreamWriterTest::~StreamWriterTest() {};

void StreamWriterTest::SetUp() {};

void StreamWriterTest::TearDown() {};

//TEST(AudioProcessorTest, FileDoesNotExist) {
//    
//    EXPECT_THROW(CAP::StreamWriter streamWriter("nonexistent/test_file"), std::runtime_error);
//}

TEST(StreamWriterTest, WriteAudioSamples) {
    CAP::StreamWriter streamWriter("test_file");

    streamWriter.write(30000);
    streamWriter.write(-12200);
    streamWriter.write(-12);
    streamWriter.write(400);
    streamWriter.write(5000);
    streamWriter.close();
    

    std::ifstream instream("test_file", std::ios_base::binary);

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

    ASSERT_EQ(instream.read(&output, sizeof(int16_t)), 0);
}


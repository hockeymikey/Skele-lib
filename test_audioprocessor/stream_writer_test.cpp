//
//  StreamWriterTest.cpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/2/17.
//
//


#include <fstream>
#include "stream_writer.hpp"
#include "stream_writer_test.hpp"


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


//TEST(StreamWriterTest, WriteAudioSamplesBatch) {
//    string filename = "WriteAudioSamplesBatch_test_file";
//    CAP::StreamWriter streamWriter(filename);
//    
//    int16_t testSamples[] = {30000, -12200, -12, 400, 5000};
//    
//    streamWriter.write(testSamples, 5);
//    streamWriter.close();
//    
//    
//    std::ifstream instream(filename, std::ios_base::binary);
//    
//    char output;
//    instream.read(&output, sizeof(int16_t));
//    int16_t outputSample = reinterpret_cast<int16_t&>(output);
//    ASSERT_EQ(outputSample, 30000);
//    
//    instream.read(&output, sizeof(int16_t));
//    outputSample = reinterpret_cast<int16_t&>(output);
//    ASSERT_EQ(outputSample, -12200);
//    
//    instream.read(&output, sizeof(int16_t));
//    outputSample = reinterpret_cast<int16_t&>(output);
//    ASSERT_EQ(outputSample, -12);
//    
//    
//    instream.read(&output, sizeof(int16_t));
//    outputSample = reinterpret_cast<int16_t&>(output);
//    ASSERT_EQ(outputSample, 400);
//    
//    instream.read(&output, sizeof(int16_t));
//    outputSample = reinterpret_cast<int16_t&>(output);
//    ASSERT_EQ(outputSample, 5000);
//    
//    ASSERT_EQ(instream.readsome(&output, sizeof(int16_t)), 0);
//    instream.close();
//}


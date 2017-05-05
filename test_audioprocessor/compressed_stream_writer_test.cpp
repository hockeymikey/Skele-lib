//
//  compressed_stream_writer_test.cpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/4/17.
//
//

#include "compressed_stream_writer_test.hpp"
#include "compressed_stream_writer.cpp"
#include <random>
#include <vector>

using namespace std;
using namespace CAP;

CompressedStreamWriterTest::CompressedStreamWriterTest() {
}

CompressedStreamWriterTest::~CompressedStreamWriterTest() {};

void CompressedStreamWriterTest::SetUp() {};

void CompressedStreamWriterTest::TearDown() {};

TEST(CompressedStreamWriterTest, TestNothingToWrite) {
    CompressedStreamWriter csr("CompressedStreamWriterTest_TestNothingToWrite", 5);
    int16_t samples[] = {};
    CompressedStreamWriter::Status status = csr.write(samples, 0);
    ASSERT_EQ(status, CompressedStreamWriter::Status::Success);
}

//TEST(CompressedStreamWriterTest, TestCompressing) {
//
//    random_device rd;
//    mt19937 mt(rd());
//    uniform_real_distribution<int16_t> dist(INT16_MIN, INT16_MAX);
//    
//    vector<int16_t> buffer;
//    //generate 5 seconds worth of samples
//    for (int i = 0; i < 5 * 44100; i++) {
//        int16_t sample =  dist(mt);
//        buffer.push_back(sample);
//        
//        if (i % 1050 == 0) {
//            <#statements#>
//        }
//    }
//
//
////    char output[1024];
////    while (wav.read(output, 1024 * sizeof(char))) {
////        streamsize bytes = wav.gcount();
////        for (int i = 0; i < bytes; i++) {
////            int16_t outputSample = reinterpret_cast<int16_t&>(output[i]);
////        }
////
////    }
//    wav.close();
//}

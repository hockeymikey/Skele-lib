//
//  compressed_stream_writer_test.cpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/4/17.
//
//

//#include "compressed_stream_writer_test.hpp"
//#include "compressed_stream_writer.cpp"
//#include <random>
//#include <vector>
//
//using namespace std;
//using namespace CAP;
//
//CompressedStreamWriterTest::CompressedStreamWriterTest() {
//}
//
//CompressedStreamWriterTest::~CompressedStreamWriterTest() {};
//
//void CompressedStreamWriterTest::SetUp() {};
//
//void CompressedStreamWriterTest::TearDown() {};
//
//TEST(CompressedStreamWriterTest, TestNothingToWrite) {
//    CompressedStreamWriter csr("CompressedStreamWriterTest_TestNothingToWrite.mp3", 5);
//    int16_t samples[] = {};
//    CompressedStreamWriter::Status status = csr.write(samples, 0);
//    ASSERT_EQ(status, CompressedStreamWriter::Status::Success);
//}
//
//TEST(CompressedStreamWriterTest, TestCompressing) {
//    string filename = "CompressedStreamWriterTest_TestCompressing.mp3";
//    CompressedStreamWriter csr(filename, 5);
//    
//    random_device rd;
//    mt19937 mt(rd());
//    uniform_real_distribution<double> dist(INT16_MIN, INT16_MAX);
//    
//    vector<int16_t> buffer;
//    int seconds = 5;
//    int nsamples = seconds * 44100;
//    //generate 5 seconds worth of samples
//    for (int i = 1; i <= nsamples; i++) {
//        int16_t sample = (int16_t)dist(mt);
//        buffer.push_back(sample);
//
//        if (i % 1050 == 0) {
//            CompressedStreamWriter::Status status = csr.write(buffer.data(), buffer.size());
//            ASSERT_EQ(status, CompressedStreamWriter::Status::Success);
//            buffer.clear();
//        }
//    }
//    
//    csr.close();
//    
//    
//    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
//    
//    int bitrate = 128000;
//    int bytesPerSecond = bitrate / 8;
//    
//    int filesize = seconds * bytesPerSecond;
//    
//    ASSERT_NEAR(in.tellg(), filesize, 1000);
//    
//    
//    in.close();
//}


#include "audio_processor.hpp"
#include "audio_processor_test.hpp"
#include "mp3_compressor.hpp"
#include <vector>
#include <random>
#include <cstdio>

using namespace CAP;
using namespace std;

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

TEST(AudioProcessorTest, TestStream) {
    auto sampleRate = 44100;
    auto compressor = make_shared<Mp3Compressor>(9, 44100);
    remove("AudioProcessorTest_TestStream.bin");
    remove("AudioProcessorTest_TestStream.mp3");
    StreamWriter sw1("AudioProcessorTest_TestStream.bin");
    StreamWriter sw2("AudioProcessorTest_TestStream.mp3", compressor);
    
    vector<StreamWriter> sws = { std::move(sw1), std::move(sw2) };
    
    AudioProcessor ap(sws);
    
    
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(INT16_MIN, INT16_MAX);
    std::int16_t buffer[1050];
    int seconds = 10;
    int nsamples = seconds * sampleRate;
    //generate 5 seconds worth of samples
    for (int i = 1; i <= nsamples; i++) {
        int16_t sample = (int16_t)dist(mt);
        buffer[(i - 1) % 1050] = sample;
        
        if (i % 1050 == 0) {
            AudioBuffer ab(buffer, 1050);
            ap.process(std::move(ab));
        }
    }
    
    ap.stop();
}


//TEST(AudioProcessorTest, TestStream2) {
//    auto sampleRate = 44100;
//    auto compressor = make_shared<Mp3Compressor>(0, sampleRate);
//    remove("AudioProcessorTest_TestStream.bin");
//    remove("AudioProcessorTest_TestStream.mp3");
//    auto sw1 = make_shared<StreamWriter>("AudioProcessorTest_TestStream.bin");
//    auto sw2 = make_shared<StreamWriter>("AudioProcessorTest_TestStream.mp3", compressor);
//    
//    vector<shared_ptr<StreamWriter>> sws;
//    sws.push_back(sw1);
//    sws.push_back(sw2);
//    AudioProcessor ap(sws);
//    
//    
//    random_device rd;
//    mt19937 mt(rd());
//    uniform_real_distribution<double> dist(INT16_MIN, INT16_MAX);
//    vector<int16_t> buffer;
//    int seconds = 500;
//    int nsamples = seconds * sampleRate;
//    //generate 5 seconds worth of samples
//    for (int i = 1; i <= nsamples; i++) {
//        int16_t sample = (int16_t)dist(mt);
//        buffer.push_back(sample);
//        
//        if (i % (1050 * 10) == 0) {
//            ap.writeAudioSamples(buffer);
//            cout << "sw1:" << sw1->numberOfBuffersWritten() << " q:" << sw1->queueSize() << endl;
//            cout << "sw2:" << sw2->numberOfBuffersWritten() << " q:" << sw2->queueSize() << endl;
//            buffer.clear();
//        }
//    }
//    
//    ap.stop();
//    
//    
//    while (true) {
//        cout << "-sw1:" << sw1->numberOfBuffersWritten() << " q:" << sw1->queueSize() << endl;
//        cout << "-sw2:" << sw2->numberOfBuffersWritten() << " q:" << sw2->queueSize() << endl;
//
//        this_thread::sleep_for(chrono::seconds(1));
//        
//    }
//}




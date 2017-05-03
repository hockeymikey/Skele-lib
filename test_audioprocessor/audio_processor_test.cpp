
#include "audio_processor.hpp"
#include "audio_processor_test.hpp"
#include <vector>
#include <random>

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

//TEST(AudioProcessorTest, FileDoesNotExist) {
//    CAP::AudioDescription audiDescription(16, 1, 44110);
//    EXPECT_THROW(CAP::AudioProcessor ap(audiDescription, "nonexistent/test_file"), std::runtime_error);
//}



TEST(AudioProcessorTest, WriteAudioSamples) {
    CAP::StreamWriter sw1("sw1");
    vector<CAP::StreamWriter> sws;
    sws.push_back(sw1);
    CAP::AudioProcessor ap(sws);
    
    while (false) {
        random_device rd;
        mt19937 mt(rd());
        uniform_real_distribution<double> dist(INT16_MIN, INT16_MAX);
        vector<int16_t> samples;
        for(int i = 0; i < 1024; i++) {
            samples.push_back((int16_t) dist(mt));
        }
        ap.writeAudioSamples(samples);
        
    }
//
}
//
//TEST_F(FooTest, ByDefaultBazFalseIsFalse) {
//    Foo foo(m_bar);
//    EXPECT_EQ(foo.baz(false), false);
//}
//
//TEST_F(FooTest, SometimesBazFalseIsTrue) {
//    Foo foo(m_bar);
//    // Have norf return true for once
//    EXPECT_CALL(m_bar,norf()).WillOnce(Return(true));
//    EXPECT_EQ(foo.baz(false), true);
//}

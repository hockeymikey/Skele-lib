//
//  audio_buffer_test.cpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/21/17.
//
//

#include "audio_buffer_test.hpp"
#include "audio_buffer.hpp"

using namespace std;
using namespace CAP;

AudioBufferTest::AudioBufferTest() {
}

AudioBufferTest::~AudioBufferTest() {};

void AudioBufferTest::SetUp() {};

void AudioBufferTest::TearDown() {};

TEST(AUdioBufferTest, TestBufferOverflow) {
    std::int16_t temp[100];
    EXPECT_THROW(AudioBuffer(temp, 10000), overflow_error);
}

//
//  test_audioprocessor.cpp
//  test
//
//  Created by Aziz Usmanov on 4/11/17.
//  Copyright © 2017 Cogi. All rights reserved.
//

#include "AudioProcessor.hpp"
#include "test_audioprocessor.hpp"
#include <boost/filesystem/path.hpp>

using ::testing::Return;

AudioProcessorTest::AudioProcessorTest() {
    // Have qux return true by default
//    ON_CALL(m_bar,qux()).WillByDefault(Return(true));
    // Have norf return false by default
//    ON_CALL(m_bar,norf()).WillByDefault(Return(false));
}

AudioProcessorTest::~AudioProcessorTest() {};

void AudioProcessorTest::SetUp() {};

void AudioProcessorTest::TearDown() {};

TEST(AudioProcessorTest, FileDoesNotExist) {
    Cogi::AudioDescription audiDescription(16, 1, 44110);
    EXPECT_THROW(Cogi::AudioProcessor ap(audiDescription, "nonexistent/test_file"), std::runtime_error);
}

TEST(AudioProcessorTest, WriteAudioSamples) {
    Cogi::AudioDescription audiDescription(16, 1, 44110);
    boost::filesystem::path path("test_file");
    boost::filesystem::ofstream stream(path);
    Cogi::AudioProcessor ap(audiDescription, path.string());
    int array[] = {1,2,3,4,5};
    ap.writeAudioSamples(array, 5);
    ap.close();
    boost::filesystem::ifstream instream(path);
    int num[5];
//    std::streamsize read = instream.readsome(&num[0], 5 * sizeof(int));
    
    
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

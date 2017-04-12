//
//  test_audioprocessor.cpp
//  test
//
//  Created by Aziz Usmanov on 4/11/17.
//  Copyright © 2017 Cogi. All rights reserved.
//

#include "audioprocessor.hpp"
#include "test_audioprocessor.hpp"


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

TEST_F(AudioProcessorTest, ByDefaultBazTrueIsTrue) {
    Cogi::AudioProcessor ap;
    ap.helloWorld();
//    EXPECT_EQ(foo.baz(true), true);
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

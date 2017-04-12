//
//  test_audioprocessor.hpp
//  test
//
//  Created by Aziz Usmanov on 4/11/17.
//  Copyright © 2017 Cogi. All rights reserved.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
//#include "mockbar.h"

// The fixture for testing class Foo.
class AudioProcessorTest : public ::testing::Test {
    
protected:
    
    // You can do set-up work for each test here.
    AudioProcessorTest();
    
    // You can do clean-up work that doesn't throw exceptions here.
    virtual ~AudioProcessorTest();
    
    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:
    
    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();
    
    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();
    
    // The mock bar library shaed by all tests
//    MockBar m_bar;
};

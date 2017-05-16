//
//  mp3_compressor_test.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/15/17.
//
//

#ifndef mp3_compressor_test_hpp
#define mp3_compressor_test_hpp
#include "gtest/gtest.h"

// The fixture for testing class Foo.
class Mp3CompressorTest : public ::testing::Test {
    
protected:
    
    // You can do set-up work for each test here.
    Mp3CompressorTest();
    
    // You can do clean-up work that doesn't throw exceptions here.
    virtual ~Mp3CompressorTest();
    
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


#endif /* mp3_compressor_test_hpp */

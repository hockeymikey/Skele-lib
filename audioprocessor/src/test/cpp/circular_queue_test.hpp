//
//  circular_queue_test.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 8/2/17.
//
//

#ifndef circular_queue_test_hpp
#define circular_queue_test_hpp

#include "gtest/gtest.h"

// The fixture for testing class Foo.
class CircularQueueTest : public ::testing::Test {
    
protected:
    
    // You can do set-up work for each test here.
    CircularQueueTest();
    
    // You can do clean-up work that doesn't throw exceptions here.
    virtual ~CircularQueueTest();
    
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

#endif /* circular_queue_test_hpp */

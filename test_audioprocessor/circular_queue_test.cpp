//
//  circular_queue_test.cpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 8/2/17.
//
//

#include "circular_queue_test.hpp"
#include "circular_queue.hpp"
#include "audio_buffer.hpp"

using namespace std;
using namespace CAP;

CircularQueueTest::CircularQueueTest() {
}

CircularQueueTest::~CircularQueueTest() {};

void CircularQueueTest::SetUp() {};

void CircularQueueTest::TearDown() {};

TEST(CircularQueueTest, TestNotFull) {
    
    CircularQueue<3> q;
    
    q.enqueue(1);
    q.enqueue(2);
    
    ASSERT_FALSE(q.isFull());
    
}

TEST(CircularQueueTest, TestFull) {
    
    CircularQueue<3> q;
    
    q.enqueue(1);
    q.enqueue(2);
    q.enqueue(3);
    
    
    ASSERT_TRUE(q.isFull());
    
}

TEST(CircularQueueTest, TestEmpty) {
    
    CircularQueue<3> q;
    
    q.enqueue(1);
    q.enqueue(2);
    q.enqueue(3);
    q.enqueue(4);
    
    q.dequeue();
    q.dequeue();
    q.dequeue();
    q.dequeue();
    
    ASSERT_TRUE(q.isEmpty());
    
}


TEST(CircularQueueTest, TestCircular) {
    
    CircularQueue<3> q;
    
    q.enqueue(1);
    q.enqueue(2);
    q.enqueue(3);
    
    ASSERT_EQ(q.front(), 1);
    
    q.dequeue();
    
    ASSERT_EQ(q.front(), 2);
    
    q.enqueue(4);
    
    ASSERT_EQ(q.front(), 2);
    q.dequeue();
    
    ASSERT_EQ(q.front(), 3);
    q.dequeue();
    
    ASSERT_EQ(q.front(), 4);
    
}

TEST(CircularQueueTest, TestSizeEmpty) {
    CircularQueue<5> q;
    
    ASSERT_EQ(q.size(), 0);
}

TEST(CircularQueueTest, TestSizeNotEmpty) {
    CircularQueue<5> q;
    
    q.enqueue(1);
    q.enqueue(2);
    q.enqueue(3);

    
    ASSERT_EQ(q.size(), 3);
}

TEST(CircularQueueTest, TestSizeNotEmpty2) {
    CircularQueue<5> q;
    
    q.enqueue(1);
    q.enqueue(2);
    q.enqueue(3);
    q.enqueue(4);
    q.enqueue(5);
    
    q.dequeue();
    q.enqueue(6);
    
    ASSERT_EQ(q.size(), 5);
}

TEST(CircularQueueTest, TestSizeNotEmpty3) {
    CircularQueue<5> q;
    
    q.enqueue(1);
    q.enqueue(2);
    q.enqueue(3);
    q.enqueue(4);
    q.enqueue(5);
    q.enqueue(6);
    
    q.dequeue();
    q.dequeue();
    
    ASSERT_EQ(q.size(), 3);
}

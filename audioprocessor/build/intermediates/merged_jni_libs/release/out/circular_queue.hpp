//
//  circular_queue.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 8/2/17.
//
//

#ifndef circular_queue_hpp
#define circular_queue_hpp

#include <array>

namespace CAP {
    
    class AbstractCircularQueue {
    public:
        virtual bool isEmpty() const = 0;
        virtual bool isFull() const = 0;
        virtual void enqueue(std::int16_t sample) = 0;
        virtual void dequeue() = 0;
        virtual std::int16_t front() const = 0;
        virtual std::size_t size() const = 0;
        virtual std::size_t capacity() const = 0;
    };
    
    template <typename std::size_t N>
    class CircularQueue: public AbstractCircularQueue {
    public:
        CircularQueue() = default;
        CircularQueue(CircularQueue&&) = default;
        
        CircularQueue(const CircularQueue& other) = delete;
        
        
        bool isEmpty() const {
            return front_ == -1 && rear == -1;
        }
        
        bool isFull() const {
            return (rear + 1) % N == front_ ? true : false;
        }
        
        void enqueue(std::int16_t sample) {
            if (isFull()) {
                return;
            }
            if (isEmpty()) {
                front_ = rear = 0;
            } else {
                rear = (rear + 1) % N;
            }
            container[rear] = std::move(sample);
        }
        
        void dequeue() {
            if(isEmpty()) {
                return;
            } else if(front_ == rear) {
                rear = front_ = -1;
            } else {
                front_ = (front_ + 1) % N;
            }
        }
        
        std::size_t capacity() const {
            return N;
        }
        
        std::size_t size() const {
            if (isEmpty()) {
                return 0;
            }
            if (isFull()) {
                return N;
            }
            return front_ > rear ? (N - front_ + rear + 1) : (rear - front_ + 1);
        }
        
        std::int16_t front() const {
            return std::move(container[front_]);
        }
        
    private:
        std::size_t front_ = -1;
        std::size_t rear = -1;
        std::array<std::int16_t, N> container;
    };
}


#endif /* circular_queue_hpp */

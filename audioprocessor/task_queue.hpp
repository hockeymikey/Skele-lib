//
//  task_queue.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/12/17.
//
//

#ifndef task_queue_hpp
#define task_queue_hpp

#include <queue>
#include <memory>

namespace CAP {
    
    class TaskQueue {
    public:
        TaskQueue();
        ~TaskQueue();
        void start();
        void run();
        
    protected:
    private:
        std::queue<std::function<void()>> queue;
    };
    
}

#endif /* task_queue_hpp */

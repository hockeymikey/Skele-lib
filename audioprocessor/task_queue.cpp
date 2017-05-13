//
//  task_queue.cpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/12/17.
//
//

#include "task_queue.hpp"
#include <iostream>
#include <thread>

CAP::TaskQueue::TaskQueue() {
    
}
CAP::TaskQueue::~TaskQueue() {
}

void CAP::TaskQueue::run() {
    while (true) {
        //            auto size = queue.size();
        
        std::cout << std::this_thread::get_id() << std::endl;
    }
}

void CAP::TaskQueue::start() {
    // detach() breack connection between c++ object and thread of execution
    std::thread(&CAP::TaskQueue::run, this).detach();
}

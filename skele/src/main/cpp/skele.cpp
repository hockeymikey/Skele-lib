
#include "skele.hpp"

void CAP::Skele::testingLocks() {
    std::mutex bundlesMute;


    std::unique_lock<std::mutex> bundlesLock(bundlesMute, std::defer_lock);
    bundlesLock.lock();
    //bundlesLock.lock();
}

CAP::Skele::Skele() {
    bundlesMutex = new std::mutex();
}

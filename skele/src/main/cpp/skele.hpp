
#ifndef skele_hpp
#define skele_hpp
#include <array>
#include <mutex>

namespace CAP {
    
    /**
     Orchestrates audio sample writing
     **/
    class Skele {
        
    public:
        Skele();
        void testingLocks();

    protected:
    
    private:
        std::mutex* bundlesMutex;

    };
}

#endif /* audioprocessor_hpp */

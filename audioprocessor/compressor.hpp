//
//  compressor.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/14/17.
//
//

#ifndef compressor_hpp
#define compressor_hpp

#include <vector>

namespace CAP {
    class Compressor {
    public:
        
        
        /**
         Needs to be implemented by derived class
         **/
        virtual std::vector<int16_t> compress(std::vector<int16_t> const &) = 0;
        
        
    protected:
    private:
        
    };
}

#endif /* compressor_hpp */

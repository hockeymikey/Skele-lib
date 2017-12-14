//
//  file_mock.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/24/17.
//
//

#ifndef file_mock_hpp
#define file_mock_hpp

#include "gmock/gmock.h"
#include "file.hpp"

class FileMock: public CAP::File {
public:
    MOCK_METHOD0(open, void ());
    MOCK_CONST_METHOD0(isOpen, bool());
    MOCK_METHOD1(write, bool(const CAP::AudioBuffer &audioBuffer));
    MOCK_METHOD2(write, bool(unsigned char *rawBuffer, int nbytes));
    MOCK_CONST_METHOD0(path, std::string ());
    MOCK_METHOD0(close, void ());
};


#endif /* file_mock_hpp */

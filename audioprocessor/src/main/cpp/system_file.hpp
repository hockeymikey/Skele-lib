//
//  system_file.hpp
//  AudioProcessor
//
//  Created by Aziz Usmanov on 5/24/17.
//
//

#ifndef system_file_hpp
#define system_file_hpp

#include "file.hpp"
#include <string>
#include <fstream>

namespace CAP {
    class SystemFile: public File {
    public:
        
        void close();
        
        /**
         Provides the status if the file has been opened successfully or not
         
         @return boolean            
         **/
        bool isOpen() const;
        
        /**
         Opens the file         
         **/
        void open();
        
        /**
         Writes audio buffer to file
         
         @param audioBuffer
            Reference to audioBuffer
         
         @return boolean
            true on successful write, false otherwise
         **/
        bool write(const CAP::AudioBuffer &audioBuffer);
        
        /**
         Getter for filepath
         
         @return string
         **/
        std::string path() const;
        
        /**
         Constructor. Opens file during construction
         
         @param filepath
            Path to file to open
         **/
        SystemFile(std::string filepath);
        
        SystemFile(const SystemFile&) = delete;
        
        SystemFile(SystemFile&&) = default;
        
        /**
         Destructor
         **/
        ~SystemFile();
        
        SystemFile() = delete;
    protected:
    private:
        std::string filepath;
        std::ofstream filestream;
    };
}


#endif /* system_file_hpp */

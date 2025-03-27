#pragma once

#include <iostream>
#include <fstream>
#include "Logger.hpp"

class FileReader
{
    private:
        std::ifstream file;
        std::string fileStr;
    public:
        FileReader(const std::string &fileName);
        void fileToStr();
        std::string getFileStr() const;

        ~FileReader();
        class FileReadingException : public std::exception
        {
            private:
                std::string err_message;
            public:
            FileReadingException(std::string err_msg);
                virtual ~FileReadingException() throw();
                const char* what() const throw();
        };

};
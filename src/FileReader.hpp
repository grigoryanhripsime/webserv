#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include "Directive.hpp"

class FileReader
{
    private:
        std::ifstream file;
        std::string fileStr;
    public:
        FileReader(const std::string &fileName);
        // std::vector<Directive *> &readConfig();
        void fileToStr();
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
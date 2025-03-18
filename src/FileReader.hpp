#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "Directive.hpp"
#include <algorithm>

class FileReader
{
    private:
        std::ifstream file;
        std::string fileStr;
        Directive directives;
    public:
        FileReader(const std::string &fileName);
        // std::vector<Directive *> &readConfig();
        void parseConfig();
        void printDirective(const Directive &directive, int indent) ;
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
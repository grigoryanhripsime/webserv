#pragma once

#include <iostream>
#include <sstream>
#include "Directive.hpp"
#include <algorithm>
#include "FileReader.hpp"
#include "Logger.hpp"

class TokenConfig
{
    private:
        std::string strFile;
        Directive directives;
        
    public:
        TokenConfig(std::string strFile);
        void fillingDirectives();
        void fillingDirectivesRec(std::stringstream &ss, Directive &directives);
        void printDirective(const Directive &directive, int indent);
        const Directive &getDirectives() const;
        class TokenConfigException : public std::exception
        {
            private:
                std::string err_message;
            public:
 
            TokenConfigException(std::string err_msg);
                virtual ~TokenConfigException() throw();
                const char* what() const throw();
        };
        ~TokenConfig() {}//std::cout << "TokenConfig dtor is called\n";}
};
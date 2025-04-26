#pragma once

#include <map>
#include <iostream>
#include <vector>

class Directive 
{
public:
    std::multimap<std::string, std::vector<std::string> > simpleDir;
    std::multimap<std::string, Directive *> blocks;
    Directive(const Directive& other);
    Directive& operator=(const Directive& other);
    Directive();
    ~Directive();
};
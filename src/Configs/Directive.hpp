#pragma once

#include <map>
#include <iostream>
#include <vector>

struct Directive 
{
    std::multimap<std::string, std::vector<std::string> > simpleDir;
    std::multimap<std::string, Directive *> blocks;
    Directive(const Directive& other);
    Directive();
    ~Directive();
};
#pragma once

#include <map>
#include <iostream>
#include <vector>

struct Directive 
{
    std::multimap<std::string, std::vector<std::string> > values;
    std::multimap<std::string, Directive *> blocks;
    ~Directive();
};
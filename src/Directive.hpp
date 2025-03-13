#pragma once

#include <map>
#include <iostream>
#include <vector>

struct Directive 
{
    std::multimap<std::string, std::vector<std::string> > values;
    std::multimap<std::string, Directive *> blocks;

    ~Directive() {
        std::multimap<std::string, Directive*>::iterator it;
        for (it = blocks.begin(); it != blocks.end(); ++it) {
            delete it->second;
        }
        blocks.clear();
    }
};
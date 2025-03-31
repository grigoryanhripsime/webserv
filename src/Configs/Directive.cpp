#include "Directive.hpp"

Directive::Directive() {}

Directive::Directive(const Directive& other) {
    simpleDir = other.simpleDir;
    for (std::multimap<std::string, Directive*>::const_iterator it = other.blocks.begin();
         it != other.blocks.end(); ++it) 
        blocks.insert(std::make_pair(it->first, new Directive(*(it->second))));
}

Directive::~Directive() {
    std::multimap<std::string, Directive*>::iterator it;
    for (it = blocks.begin(); it != blocks.end(); ++it) {
        delete it->second;
    }
    blocks.clear();
}
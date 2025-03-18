#include "Directive.hpp"

Directive::~Directive() {
    std::multimap<std::string, Directive*>::iterator it;
    for (it = blocks.begin(); it != blocks.end(); ++it) {
        delete it->second;
    }
    blocks.clear();
}
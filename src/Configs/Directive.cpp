#include "Directive.hpp"

Directive::Directive() {}

Directive::Directive(const Directive& other) {
    simpleDir = other.simpleDir;
    for (std::multimap<std::string, Directive*>::const_iterator it = other.blocks.begin();
         it != other.blocks.end(); ++it) 
        blocks.insert(std::make_pair(it->first, new Directive(*(it->second))));
}
Directive& Directive::operator=(const Directive& other) {
    if (this != &other) {
        // 1. Очищаем текущие данные
        simpleDir.clear();
        
        // 2. Копируем simpleDir (поверхностная копия - вектор копируется нормально)
        simpleDir = other.simpleDir;
        
        // 3. Очищаем и глубоко копируем blocks
        // Сначала удаляем старые Directive*
        for (std::multimap<std::string, Directive*>::iterator it = blocks.begin();
             it != blocks.end(); ++it) {
            delete it->second;
        }
        blocks.clear();
        
        // Глубокое копирование новых Directive*
        for (std::multimap<std::string, Directive*>::const_iterator it = other.blocks.begin();
             it != other.blocks.end(); ++it) {
            blocks.insert(std::make_pair(it->first, new Directive(*(it->second))));
        }
    }
    return *this;
}

Directive::~Directive() {
    std::multimap<std::string, Directive*>::iterator it;
    for (it = blocks.begin(); it != blocks.end(); ++it) {
        delete it->second;
    }
    blocks.clear();
}
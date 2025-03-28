#pragma once

#include <iostream>
#include <vector>
#include <map>
#include "ServerDirective.hpp"

class LocationDirective : public ServerDirective
{
    private:
        std::string path;
        std::map<int, std::string> redirect;

};

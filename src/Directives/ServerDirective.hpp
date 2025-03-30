#pragma once

#include "AGeneralDirective.hpp"
#include <vector>
#include <stdexcept>
#include <sstream>
#include "LocationDirective.hpp"
class LocationDirective;
class ServerDirective : public AGeneralDirective{
private:
    std::vector<LocationDirective*> locdir;
    std::string listen;//no default=>REQUIRE,   Must define port (no default)
    std::string server_name;//allow but not required
public:
    ServerDirective();
    ~ServerDirective();
    virtual void validate() const;  // для валидации значений

};

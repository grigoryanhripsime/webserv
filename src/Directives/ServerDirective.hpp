#pragma once

#include "AGeneralDirective.hpp"
#include <vector>
class LocationDirective;
class ServerDirective : public AGeneralDirective{
private:
    std::vector<LocationDirective*> locdir;
    std::string listen;//no default=>REQUIRE,   Must define port (no default)
    std::string server_name;//allow but not required
        
        // std::string index;//Should have a default (e.g., index.html)., ham kara serverum lini ham locationum 
        // std::string client_max_body_size;//Optional but recommended.ham kara serverum lini ham locationum  
public:
    ServerDirective();
    ~ServerDirective();
    virtual void validate() const;  // для валидации значений

};

#pragma once

class Request
{
private:
    int servIndex;
    int locIndex;
    std::string method;
    std::string uri;//GET-i hamar
public:
    Request();
    ~Request();

};



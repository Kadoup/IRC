#pragma once

#include <string>
#include <vector>

#include "clients.hpp"

class channel
{
private:
    std::vector<clients>members;
    std::string _name;
    
public:
    channel();
    ~channel();
};

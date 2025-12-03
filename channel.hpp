#include "server.hpp"

class channel
{
private:
    std::vector<clients>members;
    std::string _name;
    
public:
    channel();
    ~channel();
};

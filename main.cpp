#include "server.hpp"

int main(int argc, char **argv)
{
	 if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }
    
    try
    {
        server srv(std::atoi(argv[1]));
        srv.initServer();
        srv.runServer();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;

}
#include "server.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <port>" << "<password>" << std::endl;
		return 1;
	}
	
	try
	{
		int port = std::atoi(argv[1]);
		if (port < 1024 || port > 65535)
		{
			std::cerr << "Error: Port must be between 1024 and 65535" << std::endl;
			return 1;
		}

		server srv(port, argv[2]);
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
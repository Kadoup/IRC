#include "server.hpp"

bool g_serverRunning = false;
bool g_ctrlC = false;

void checkPort(char **argv)
{
    long port = std::strtol(argv[1], NULL, 10);
    if (port < 1024 || port > 65535)
    {
        std::cerr << "Port number must be between 1024 and 65535" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void	_handleSignal(int signal) {
	g_serverRunning = false;
    g_ctrlC = true;
	(void) signal;
}

int main(int argc, char **argv)
{
	if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << " <password>" << std::endl;
        return 1;
    }
    
    try
    {
        checkPort(argv);
        server srv(std::atoi(argv[1]), argv[2]);
        g_serverRunning = true;
        signal(SIGINT, _handleSignal);
        signal(SIGQUIT, _handleSignal);
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
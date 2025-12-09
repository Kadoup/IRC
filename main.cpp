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
        server srv(std::atoi(argv[1]), argv[2]);
        // srv.registerCommand("NICK", nickCommand());
        // srv.registerCommand("USER", &server::handleUser);
        // srv.registerCommand("PASS", &server::handlePass);
        // srv.registerCommand("PRIVMSG", &server::handlePrvMsg);
        // srv.registerCommand("QUIT", &server::handleQuit);
        // srv.registerCommand("JOIN", &server::handleJoin);
        // srv.registerCommand("TOPIC", &server::handleTopic);
        // srv.registerCommand("KICK", &server::handleKick);
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
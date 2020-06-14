#include "fastev/tcp/tcp_server.hpp"

int main()
{
    try
    {
        auto s = fastev::TCPServer(8080);
        s.onConnect([&](int fd, char *ip, int port) {
            std::cout << "connect" << std::endl;
        });
        s.onDisconnect([&](int fd) {
            std::cout << "disconnect" << std::endl;
        });
        s.onData([&](int fd, char *message, size_t size) {
            std::cout << "data" << std::endl;
            s.tcpReply(fd, message, size);
        });
        s.start();
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}
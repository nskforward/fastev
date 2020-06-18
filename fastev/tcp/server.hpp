#ifndef FASTEV_TCP_SERVER_HPP
#define FASTEV_TCP_SERVER_HPP

#ifdef __APPLE__
#include "kqueue.hpp"
#endif
#ifdef __linux__
#include "epoll.hpp"
#endif
#include "socket.hpp"

#ifndef FASTEV_TCP_CHUNK_SIZE
#define FASTEV_TCP_CHUNK_SIZE 48
#endif

using namespace std;

namespace fastev
{
    typedef function<void(int fd, char *ip, int port)> TCPConnectFunc;
    typedef function<void(int fd)> TCPDisconnectFunc;
    typedef function<void(int fd, char *data, size_t size)> TCPChunkFunc;

    class TCPServer : public Reactor
    {
    private:
        int master_sock = 0;

        TCPConnectFunc connect_cb;
        TCPDisconnectFunc disconnect_cb;
        TCPChunkFunc chunk_cb;
        void onSocketEvent(int fd) override;

    public:
        TCPServer(int port);
        ~TCPServer();
        void onConnect(TCPConnectFunc func);
        void onDisconnect(TCPDisconnectFunc func);
        void onChunk(TCPChunkFunc func);
        void disconnect(int fd);
        void tcpReply(int fd, const char *message, size_t size);
    };
} // namespace fastev

#endif
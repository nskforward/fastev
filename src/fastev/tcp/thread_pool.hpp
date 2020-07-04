#ifndef FASTEV_TCP_THREAD_POOL
#define FASTEV_TCP_THREAD_POOL

#include <unistd.h>
#include <thread>
#include <functional>
#include <sys/socket.h>
#include "../event/kqueue.hpp"
#include "../core/exception.hpp"

#ifndef FASTEV_TCP_THREAD_POOL_SIZE
#define FASTEV_TCP_THREAD_POOL_SIZE 4
#endif

#ifndef FASTEV_TCP_CHUNK_SIZE
#define FASTEV_TCP_CHUNK_SIZE 256
#endif

using namespace std;

namespace fastev
{
    class ThreadPool
    {
    private:
        Reactor _reactor_pool[FASTEV_TCP_THREAD_POOL_SIZE];
        thread *_thread_pool[FASTEV_TCP_THREAD_POOL_SIZE];
        int _current_worker = 0;
        function<void(int fd, int worker_id, const char *chunk, size_t size)> on_chunk_cb;
        function<void(int fd, int worker_id)> on_disconnect_cb;

    public:
        ThreadPool();
        ~ThreadPool();
        void watch(int fd);
        void onChunk(function<void(int fd, int worker_id, const char *chunk, size_t size)> func);
        void onDisonnect(function<void(int fd, int worker_id)> func);
        void disconnect(int fd, int worker_id);
    };

    ThreadPool::ThreadPool()
    {
        for (int i = 0; i < FASTEV_TCP_THREAD_POOL_SIZE; i++)
        {
            thread *worker = new thread(
                [this](Reactor *reactor, int worker_id) {
                    reactor->onRead([this, &worker_id](int fd) {
                        char chunk[FASTEV_TCP_CHUNK_SIZE];
                        ssize_t ret = recv(fd, chunk, FASTEV_TCP_CHUNK_SIZE - 1, 0);
                        if (ret == 0 || ret > FASTEV_TCP_CHUNK_SIZE - 1) // disconnect
                        {
                            disconnect(fd, worker_id);
                            return;
                        }
                        if (ret < 0)
                        {
                            if (errno == EAGAIN || errno == EWOULDBLOCK) // normal continue
                            {
                                Logger::log(LogLevel::INFO, "normal block");
                                return;
                            }
                            else // error
                            {
                                disconnect(fd, worker_id);
                                switch (errno)
                                {
                                case ECONNRESET:
                                    Logger::log(LogLevel::INFO, "[fd:%d] connection reset by peer", fd);
                                    break;

                                default:
                                    Logger::log(LogLevel::ERROR, "[fd:%d] recv returned error #%d", fd, errno);
                                    break;
                                }
                                return;
                            }
                        }
                        // handle data
                        chunk[ret] = '\0';
                        on_chunk_cb(fd, worker_id, chunk, ret);
                    });
                    reactor->start();
                },
                &_reactor_pool[i], i);
            worker->detach();
            _thread_pool[i] = worker;
        }
    }

    ThreadPool::~ThreadPool()
    {
        for (int i = 0; i < FASTEV_TCP_THREAD_POOL_SIZE; i++)
        {
            delete _thread_pool[i];
        }
    }

    void ThreadPool::onDisonnect(function<void(int fd, int worker_id)> func)
    {
        on_disconnect_cb = func;
    }

    void ThreadPool::onChunk(function<void(int fd, int worker_id, const char *chunk, size_t size)> func)
    {
        on_chunk_cb = func;
    }

    void ThreadPool::disconnect(int fd, int worker_id)
    {
        _reactor_pool[worker_id].unwatchRead(fd);
        close(fd);
        if (on_disconnect_cb != NULL)
        {
            on_disconnect_cb(fd, worker_id);
        }
    }

    void ThreadPool::watch(int fd)
    {
        auto n = fd % FASTEV_TCP_THREAD_POOL_SIZE;
        _reactor_pool[n].watchRead(fd);
    }
} // namespace fastev

#endif
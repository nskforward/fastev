#ifndef FASTEV_TCP_THREAD_POOL
#define FASTEV_TCP_THREAD_POOL

#include <unistd.h>
#include <thread>
#include <functional>
#include <vector>
#include "../event/kqueue.hpp"
#include "../core/exception.hpp"

using namespace std;

namespace fastev
{
    class ThreadPool
    {
    private:
        vector<KQueueReactor *> _reactor_pool;
        vector<thread *> _thread_pool;
        int _workers_num = 1;
        int _current_worker = 0;
        function<void(int fd, int worker_id)> read_cb;

    public:
        ThreadPool(int workers_num);
        ~ThreadPool();
        void watch(int fd);
        void unwatch(int fd, int worker_id);
        void onRead(function<void(int fd, int worker_id)> func);
    };

    ThreadPool::ThreadPool(int workers_num)
    {
        _workers_num = workers_num;
        for (int i = 0; i < _workers_num; i++)
        {
            KQueueReactor *reactor = new KQueueReactor();
            _reactor_pool.push_back(reactor);
            thread *worker = new thread(
                [this](KQueueReactor *reactor, int worker_id) {
                    reactor->start([&worker_id, this](int fd) {
                        read_cb(fd, worker_id);
                    });
                },
                reactor, i);
            _thread_pool.push_back(worker);
            worker->detach();
        }
    }

    ThreadPool::~ThreadPool()
    {
        for (int i = 0; i < _workers_num; i++)
        {
            delete _reactor_pool[i];
            delete _thread_pool[i];
        }
    }

    void ThreadPool::onRead(function<void(int fd, int worker_id)> func)
    {
        read_cb = func;
    }

    void ThreadPool::watch(int fd)
    {
        if (_current_worker == _workers_num)
        {
            _current_worker = 0;
        }
        _reactor_pool[_current_worker++]->watch(fd);
    }

    void ThreadPool::unwatch(int fd, int worker_id)
    {
        _reactor_pool[worker_id]->unwatch(fd);
    }

} // namespace fastev

#endif
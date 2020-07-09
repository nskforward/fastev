#ifndef FASTEV_CORE_CHANNEL
#define FASTEV_CORE_CHANNEL

#include <queue>
#include <mutex>
#include <condition_variable>

namespace fastev
{
    template <typename T>
    class Channel
    {
    public:
        Channel();
        ~Channel();
        T pop();
        void push_back(const T &item);
        void push_back(T &&item);
        int size();
        bool empty();

    private:
        std::deque<T> _queue;
        std::mutex _mutex;
        std::condition_variable _condition;
    };

    template <typename T>
    Channel<T>::Channel() {}

    template <typename T>
    Channel<T>::~Channel() {}

    template <typename T>
    T Channel<T>::pop()
    {
        std::unique_lock<std::mutex> mlock(_mutex);
        while (_queue.empty())
        {
            _condition.wait(mlock);
        }
        auto front = _queue.front();
        _queue.pop_front();
        return front;
    }

    template <typename T>
    void Channel<T>::push_back(const T &item)
    {
        std::unique_lock<std::mutex> mlock(_mutex);
        _queue.push_back(item);
        mlock.unlock();
        _condition.notify_one();
    }

    template <typename T>
    void Channel<T>::push_back(T &&item)
    {
        std::unique_lock<std::mutex> mlock(_mutex);
        _queue.push_back(std::move(item));
        mlock.unlock();
        _condition.notify_one();
    }

    template <typename T>
    int Channel<T>::size()
    {
        std::unique_lock<std::mutex> mlock(_mutex);
        int size = _queue.size();
        mlock.unlock();
        return size;
    }
} // namespace fastev

#endif
#ifndef __M_LOOPER_H__
#define __M_LOOPER_H__

#include "buffer.hpp"
#include <mutex>
#include <thread>
#include <condition_variable>
#include <memory>
#include <functional>
#include <pthread.h>

namespace ns_log
{
    enum class AsyncType
    {
        ASYNC_SAFE,
        ASYNC_UNSAFE
    };

    using Functor = std::function<void(Buffer &)>;
    class AsyncLooper
    {
    public:
        using ptr = std::shared_ptr<AsyncLooper>;
        AsyncLooper(const Functor &cb, const AsyncType &looper_type = AsyncType::ASYNC_SAFE) : _callBack(cb), _looper_type(looper_type), _stop(false), _thread(&AsyncLooper::threadEntry, this)
        {
        }
        ~AsyncLooper() { stop(); }
        void push(const char *data, const size_t &len)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (_looper_type == AsyncType::ASYNC_SAFE)
                _product_cond.wait(lock, [this, &len]()
                                   { return _product_buf.writeAbleSize() >= len; });
            _product_buf.push(data, len);
            _consume_cond.notify_all();
        }
        void stop()
        {
            if (!_stop)
            {
                _stop = true;
            }
            _consume_cond.notify_all();
            _thread.join();
        }

    private:
        void threadEntry()
        {
            while (true)
            {
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    if (_stop && this->_product_buf.empty())
                        break;
                    _consume_cond.wait(lock, [this]()
                                       { return _stop || !this->_product_buf.empty(); });
                    _consume_buf.swap(_product_buf);
                    if (_looper_type == AsyncType::ASYNC_SAFE)
                        _product_cond.notify_all(); //?
                }
                _callBack(_consume_buf);
                _consume_buf.reset();
            }
        }

    private:
        Functor _callBack;

    private:
        bool _stop;
        AsyncType _looper_type;
        Buffer _product_buf;
        Buffer _consume_buf;
        std::mutex _mutex;
        std::condition_variable _product_cond;
        std::condition_variable _consume_cond;
        std::thread _thread;
    };
}

#endif
#ifndef __M_LOOPER_H__
#define __M_LOOPER_H__

#include "buffer.hpp"
#include <mutex>
#include <thread>
#include <condition_variable>
#include <memory>
#include <functional>

namespace ns_log
{
    using Functor = std::function<void(Buffer &)>;
    class AsyncLooper
    {
    public:
        using ptr = std::shared_ptr<AsyncLooper>;
        AsyncLooper();
        void push(const char *, const size_t &len);
        void stop();

    private:
        void threadEntry();

    private:
        Functor _callBack;

    private:
        bool _stop;
        Buffer _product_buf;
        Buffer _consume_buf;
        std::mutex _mutex;
        std::condition_variable _product_cond;
        std::condition_variable _consume_cond;
        std::thread _thread;
    };
}

#endif
#ifndef __M_LOGGER_H__
#define __M_LOGGER_H__

#include "format.hpp"
#include "message.hpp"
#include "level.hpp"
#include "sink.hpp"
#include "looper.hpp"
#include <mutex>
#include <atomic>
#include <cstdarg>
#include <unordered_map>

namespace ns_log
{
    class Logger
    {
    public:
        using ptr = std::shared_ptr<Logger>;

        Logger(const std::string &logger_name, const LogLevel::value &level, Formatter::ptr &formatter, const std::vector<SinkLog::ptr> &sinks)
            : _logger_name(logger_name), _level(level), _formatter(formatter), _sinks(sinks.begin(), sinks.end())
        {
        }

        const std::string &getName()
        {
            return _logger_name;
        }

        virtual void debug(const std::string &file, const size_t &line, const std::string &fmt, ...)
        {
            if (LogLevel::value::DEBUG < _level)
            {
                return;
            }
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cerr << "vasprintf failed!\n";
                return;
            }
            va_end(ap);
            serialize(LogLevel::value::DEBUG, file, line, res);
            free(res);
        }
        virtual void info(const std::string &file, const size_t &line, const std::string &fmt, ...)
        {
            if (LogLevel::value::INFO < _level)
            {
                return;
            }
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cerr << "vasprintf failed!\n";
                return;
            }
            va_end(ap);
            serialize(LogLevel::value::INFO, file, line, res);
            free(res);
        }
        virtual void warn(const std::string &file, const size_t &line, const std::string &fmt, ...)
        {
            if (LogLevel::value::WARN < _level)
            {
                return;
            }
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cerr << "vasprintf failed!\n";
                return;
            }
            va_end(ap);
            serialize(LogLevel::value::WARN, file, line, res);
            free(res);
        }
        virtual void error(const std::string &file, const size_t &line, const std::string &fmt, ...)
        {
            if (LogLevel::value::ERROR < _level)
            {
                return;
            }
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cerr << "vasprintf failed!\n";
                return;
            }
            va_end(ap);
            serialize(LogLevel::value::ERROR, file, line, res);
            free(res);
        }
        virtual void fatal(const std::string &file, const size_t &line, const std::string &fmt, ...)
        {
            if (LogLevel::value::FATAL < _level)
            {
                return;
            }
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cerr << "vasprintf failed!\n";
                return;
            }
            va_end(ap);
            serialize(LogLevel::value::FATAL, file, line, res);
            free(res);
        }

    private:
        void serialize(const LogLevel::value &level, const std::string &file, const size_t &line, const char *res)
        {
            Message msg(level, file, line, _logger_name, res);
            std::stringstream ss;
            _formatter->format(ss, msg);
            log(ss.str().c_str(), ss.str().size());
        }

        virtual void log(const char *data, const size_t &len) = 0;

    protected:
        Formatter::ptr _formatter;
        std::vector<SinkLog::ptr> _sinks;
        std::atomic<LogLevel::value> _level;
        std::mutex _mutex;
        std::string _logger_name;
    };

    class SyncLogger : public Logger
    {
    public:
        SyncLogger(const std::string &logger_name, const LogLevel::value &level, Formatter::ptr &formatter, const std::vector<SinkLog::ptr> &sinks) : Logger(logger_name, level, formatter, sinks) {}

    private:
        void log(const char *data, const size_t &len)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (_sinks.empty())
                return;
            for (auto &sink : _sinks)
            {
                sink->log(data, len);
            }
        }
    };

    class AsyncLogger : public Logger
    {
    public:
        AsyncLogger(const std::string &logger_name, const LogLevel::value &level, Formatter::ptr &formatter, const std::vector<SinkLog::ptr> &sinks, const AsyncType &loop_type) : Logger(logger_name, level, formatter, sinks), _looper(std::make_shared<AsyncLooper>(std::bind(&AsyncLogger::realLog, this, std::placeholders::_1), loop_type)) {}
        void log(const char *data, const size_t &len)
        {
            _looper->push(data, len);
        }
        void realLog(Buffer &buf)
        {
            if (_sinks.empty())
                return;
            for (auto &sink : _sinks)
            {
                sink->log(buf.readBegin(), buf.readAbleSize());
            }
        }

    private:
        AsyncLooper::ptr _looper;
    };

    enum class LoggerType
    {
        LOGGER_SYNC,
        LOGGER_ASYNC
    };

    class LoggerBuilder
    {
    public:
        LoggerBuilder() : _logger_type(LoggerType::LOGGER_ASYNC), _limit_level(LogLevel::value::DEBUG), _looper_type(AsyncType::ASYNC_SAFE) {}
        void buildLoggerLevel(const LogLevel::value &level) { _limit_level = level; }
        void buildLoggerName(const std::string &logger_name) { _logger_name = logger_name; }
        void buildEnableUnSafeAsync() { _looper_type = AsyncType::ASYNC_UNSAFE; }
        void buildFormatter(const std::string &pattern) { _formatter = std::make_shared<Formatter>(pattern); }
        void buildLoggerType(const LoggerType &logger_type) { _logger_type = logger_type; }
        template <typename SinkType, typename... Args>
        void buildSink(Args &&...args)
        {
            SinkLog::ptr psink = SinkFactory::create<SinkType>(std::forward<Args>(args)...);
            _sinks.push_back(psink);
        }
        virtual Logger::ptr build() = 0;

    protected:
        AsyncType _looper_type;
        LoggerType _logger_type;
        Formatter::ptr _formatter;
        std::vector<SinkLog::ptr> _sinks;
        LogLevel::value _limit_level;
        std::string _logger_name;
    };

    class LocalLoggerBuilder : public LoggerBuilder
    {
    public:
        Logger::ptr build() override
        {
            assert(!_logger_name.empty());
            if (_formatter == nullptr)
            {
                _formatter = std::make_shared<Formatter>();
            }
            if (_sinks.empty())
            {
                buildSink<StdoutSink>();
            }
            if (_logger_type == LoggerType::LOGGER_ASYNC)
            {
                return std::make_shared<AsyncLogger>(_logger_name, _limit_level, _formatter, _sinks, _looper_type);
            }
            return std::make_shared<SyncLogger>(_logger_name, _limit_level, _formatter, _sinks);
        }
    };

    class LoggerManager
    {
    public:
        static LoggerManager &getInstance()
        {
            // C++11之后，针对静态局部变量，编译器在编译的层面实现了线程安全
            // 当静态局部变量在没有构造完成之前，其他的线程就会进入阻塞
            static LoggerManager eton;
            return eton;
        }
        void addLogger(Logger::ptr &logger)
        {
            if (hasLogger(logger->getName()) == true)
                return;
            std::unique_lock<std::mutex> lock(_mutex);
            _loggers.insert(std::make_pair(logger->getName(), logger));
        }
        bool hasLogger(const std::string &name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _loggers.find(name);
            if (it == _loggers.end())
            {
                return false;
            }
            return true;
        }
        Logger::ptr getLogger(const std::string &name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _loggers.find(name);
            if (it == _loggers.end())
            {
                return Logger::ptr();
            }
            return it->second;
        }
        Logger::ptr rootLogger()
        {
            return _root_logger;
        }

    private:
        LoggerManager()
        {
            std::unique_ptr<LocalLoggerBuilder> build(new LocalLoggerBuilder());
            build->buildLoggerName("root");
            _root_logger = build->build(); // 这里不能用全局建造者建造，不然会造成循环构造，阻塞在这
            _loggers.insert(std::make_pair(_root_logger->getName(), _root_logger));
            // 加锁？
        }

    private:
        Logger::ptr _root_logger;
        std::unordered_map<std::string, Logger::ptr> _loggers;
        std::mutex _mutex;
    };

    class GlobalLoggerBuilder : public LoggerBuilder
    {
    public:
        Logger::ptr build() override
        {
            assert(!_logger_name.empty());
            if (_formatter == nullptr)
            {
                _formatter = std::make_shared<Formatter>();
            }
            if (_sinks.empty())
            {
                buildSink<StdoutSink>();
            }
            Logger::ptr logger;
            if (_logger_type == LoggerType::LOGGER_ASYNC)
            {
                logger = std::make_shared<AsyncLogger>(_logger_name, _limit_level, _formatter, _sinks, _looper_type);
            }
            else
            {
                logger = std::make_shared<SyncLogger>(_logger_name, _limit_level, _formatter, _sinks);
            }
            LoggerManager::getInstance().addLogger(logger);
            return logger;
        }
    };
}

#endif
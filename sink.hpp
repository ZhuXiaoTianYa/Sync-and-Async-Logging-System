#ifndef __M_SINK_H__
#define __M_SINK_H__

#include "util.hpp"
#include <memory>
#include <cassert>
#include <fstream>
#include <sstream>

namespace ns_log
{
    class SinkLog
    {
    public:
        using ptr = std::shared_ptr<SinkLog>;
        virtual ~SinkLog() {}
        virtual void log(const char *data, const size_t &len) = 0;
    };

    class StdoutSink : public SinkLog
    {
    public:
        void log(const char *data, const size_t &len)
        {
            std::cout.write(data, len);
        }
    };

    class FileSink : public SinkLog
    {
    public:
        FileSink(const std::string &pathname)
            : _pathname(pathname)
        {
            ns_log::ns_util::File::createDirectory(ns_log::ns_util::File::path(_pathname));
            _ofs.open(_pathname, std::ios::binary | std::ios::app);
            if (!_ofs.is_open())
            {
                std::cerr << "打开文件失败: " << _pathname << std::endl;
                assert(false);
            }
        }
        void log(const char *data, const size_t &len)
        {
            _ofs.write(data, len);
            assert(_ofs.good());
        }
        ~FileSink()
        {
            _ofs.close();
        }

    private:
        std::string _pathname;
        std::ofstream _ofs;
    };

    class RollBySizeSink : public SinkLog
    {
    public:
        RollBySizeSink(const std::string &basename, const size_t &max_fsize)
            : _max_fsize(max_fsize), _cur_fsize(0), _basename(basename), _name_count(0)
        {
            std::string pathname = createNewFile();
            ns_util::File::createDirectory(ns_util::File::path(pathname));
            _ofs.open(pathname, std::ios::binary | std::ios::app);
            if (!_ofs.is_open())
            {
                std::cerr << "打开文件失败: " << pathname << std::endl;
                assert(false);
            }
        }
        void log(const char *data, const size_t &len)
        {
            if (_cur_fsize >= _max_fsize)
            {
                _ofs.close();
                std::string pathname = createNewFile();
                ns_util::File::createDirectory(ns_util::File::path(pathname));
                _ofs.open(pathname, std::ios::binary | std::ios::app);
                if (!_ofs.is_open())
                {
                    std::cerr << "打开文件失败: " << pathname << std::endl;
                    assert(false);
                }
                _cur_fsize = 0;
            }
            _ofs.write(data, len);
            assert(_ofs.good());
            _cur_fsize += len;
        }
        ~RollBySizeSink() { _ofs.close(); }

    private:
        std::string createNewFile()
        {
            std::stringstream ss;
            time_t t = ns_util::Date::now();
            struct tm lt;
            localtime_r(&t, &lt);
            ss << _basename << lt.tm_year + 1900 << lt.tm_mon + 1 << lt.tm_mday << lt.tm_hour << lt.tm_min << lt.tm_sec << "-" << _name_count++ << ".log";
            return ss.str();
        }

    private:
        size_t _name_count;
        std::string _basename;
        std::ofstream _ofs;
        size_t _max_fsize;
        size_t _cur_fsize;
    };

    class SinkFactory
    {
    public:
        template <typename T, typename... Args>
        static SinkLog::ptr create(Args &&...args)
        {
            return std::make_shared<T>(std::forward<Args>(args)...);
        }
    };
}

#endif
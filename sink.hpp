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
            std::string pathname = createNewFileName();
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
                std::string pathname = createNewFileName();
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
        std::string createNewFileName()
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

    enum class TimeGap
    {
        GAP_SECOND = 0,
        GAP_MINUTE,
        GAP_HOUR,
        GAP_DAY
    };

    class RollByTimeSink : public SinkLog
    {
    public:
        RollByTimeSink(const std::string &basename, const TimeGap &gap_type, const size_t &gap_size)
            : _basename(basename)
        {
            switch (gap_type)
            {
            case TimeGap::GAP_SECOND:
                _gap_size = gap_size;
                break;
            case TimeGap::GAP_MINUTE:
                _gap_size = gap_size * 60;
                break;
            case TimeGap::GAP_HOUR:
                _gap_size = gap_size * 3600;
                break;
            case TimeGap::GAP_DAY:
                _gap_size = gap_size * 216000;
                break;
            }
            std::string pathname = createNewFileName();
            ns_util::File::createDirectory(ns_util::File::path(pathname));
            _ofs.open(pathname, std::ios::binary | std::ios::app);
            _cur_gap = ns_util::Date::now() / _gap_size;
            if (!_ofs.is_open())
            {
                std::cerr << "打开文件失败: " << pathname << std::endl;
                assert(false);
            }
        }
        void log(const char *data, const size_t &len)
        {
            size_t cur = ns_util::Date::now() / _gap_size;
            if (_cur_gap != cur)
            {
                _ofs.close();
                std::string pathname = createNewFileName();
                ns_util::File::createDirectory(ns_util::File::path(pathname));
                _ofs.open(pathname, std::ios::binary | std::ios::app);
                if (!_ofs.is_open())
                {
                    std::cerr << "打开文件失败: " << pathname << std::endl;
                    assert(false);
                }
                _cur_gap = cur;
            }
            _ofs.write(data, len);
            assert(_ofs.good());
        }
        ~RollByTimeSink() { _ofs.close(); }

    private:
        std::string createNewFileName()
        {
            std::stringstream ss;
            time_t t = ns_util::Date::now();
            struct tm lt;
            localtime_r(&t, &lt);
            ss << _basename << lt.tm_year + 1900 << lt.tm_mon + 1 << lt.tm_mday << lt.tm_hour << lt.tm_min << lt.tm_sec << ".log";
            return ss.str();
        }

    private:
        std::string _basename;
        std::ofstream _ofs;
        size_t _cur_gap;
        size_t _gap_size;
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
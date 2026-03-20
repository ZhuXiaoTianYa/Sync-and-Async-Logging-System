#ifndef __M_FMT_H__
#define __M_FMT_H__

#include <memory>
#include <vector>
#include <sstream>
#include <cassert>
#include "message.hpp"

namespace ns_log
{
    class FormatItem
    {
    public:
        using ptr = std::shared_ptr<FormatItem>;
        virtual void format(std::ostream &out, const Message &msg) = 0;
    };
    class MsgFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const Message &msg) override
        {
            out << msg._payload;
        }
    };
    class FileFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const Message &msg) override
        {
            out << msg._file;
        }
    };
    class LineFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const Message &msg) override
        {
            out << msg._line;
        }
    };
    class ThreadFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const Message &msg) override
        {
            out << msg._tid;
        }
    };
    class LevelFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const Message &msg) override
        {
            out << LogLevel::toString(msg._level);
        }
    };
    class LoggerFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const Message &msg) override
        {
            out << msg._logger;
        }
    };
    class TimeFormatItem : public FormatItem
    {
    public:
        TimeFormatItem(const std::string &time_format = "%Y-%m-%d %H:%M:%S") : _time_format(time_format) {}
        void format(std::ostream &out, const Message &msg) override
        {
            struct tm t;
            localtime_r(&msg._time, &t);
            char tmp[32] = {0};
            strftime(tmp, (sizeof tmp) - 1, _time_format.c_str(), &t);
            out << tmp;
        }

    private:
        std::string _time_format;
    };

    class TabFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const Message &msg) override
        {
            out << "\t";
        }
    };

    class NLineFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const Message &msg) override
        {
            out << "\n";
        }
    };

    class OtherFormatItem : public FormatItem
    {
    public:
        OtherFormatItem(const std::string &str) : _str(str) {}
        void format(std::ostream &out, const Message &msg) override
        {
            out << _str;
        }

    private:
        std::string _str;
    };

    /*
        %d 表示日期 ，包含子格式 {%Y-%m-%d %H:%M:%S}
        %t 表示线程ID
        %c 表示日志器名称
        %f 表示源文件名
        %l 表示源文件行号
        %p 表示日志级别
        %T 表示制表符缩进
        %m 表示主体消息
        %n 表示换行
    */

    class Formatter
    {
    public:
        Formatter(const std::string &pattern = "[%d{%Y-%m-%d %H:%M:%S}][%p][%t][%c][%f:%l]%T%m%n") : _pattern(pattern) { assert(parsePattern()); }
        std::string format(const Message &msg)
        {
            std::stringstream ss;
            for (auto &item : _items)
            {
                item->format(ss, msg);
            }
            return ss.str(); // move?
        }
        // 输入输出流一般不能用const
        void format(std::ostream &out, const Message &msg)
        {
            for (auto &item : _items)
            {
                item->format(out, msg);
            }
            return;
        }

    private:
        bool parsePattern()
        {
            size_t pos = 0;
            std::string key, val;
            std::vector<std::pair<std::string, std::string>> fmt_order;
            while (pos < _pattern.size())
            {
                if (_pattern[pos] != '%')
                {
                    val += _pattern[pos++];
                    continue;
                }
                if (pos + 1 < _pattern.size() && _pattern[pos + 1] == '%')
                {
                    val += _pattern[pos];
                    pos += 2;
                    continue;
                }
                if (!val.empty())
                {
                    fmt_order.push_back(std::make_pair("", val));
                    val.clear();
                }
                pos += 1;
                if (pos >= _pattern.size())
                {
                    std::cerr << "%之后，没有对应的格式化字符!\n";
                    return false;
                }
                key = _pattern[pos];
                pos += 1;
                if (pos < _pattern.size() && _pattern[pos] == '{')
                {
                    pos += 1;
                    while (pos < _pattern.size() && _pattern[pos] != '}')
                    {
                        val += _pattern[pos++];
                    }
                    pos += 1;
                    if (pos >= _pattern.size())
                    {
                        std::cerr << "子规则{}匹配错误!\n";
                        return false;
                    }
                }
                fmt_order.push_back(std::make_pair(key, val));
                key.clear();
                val.clear();
            }
            for (auto &it : fmt_order)
            {
                _items.push_back(createItem(it.first, it.second));
            }
            return true;
        }

    private:
        FormatItem::ptr createItem(const std::string &key, const std::string &value)
        {
            if (key == "d")
                return std::make_shared<TimeFormatItem>(value);
            if (key == "t")
                return std::make_shared<ThreadFormatItem>();
            if (key == "c")
                return std::make_shared<LoggerFormatItem>();
            if (key == "f")
                return std::make_shared<FileFormatItem>();
            if (key == "l")
                return std::make_shared<LineFormatItem>();
            if (key == "p")
                return std::make_shared<LevelFormatItem>();
            if (key == "T")
                return std::make_shared<TabFormatItem>();
            if (key == "m")
                return std::make_shared<MsgFormatItem>();
            if (key == "n")
                return std::make_shared<NLineFormatItem>();
            if (key == "")
                return std::make_shared<OtherFormatItem>(value);
            std::cerr << "没有对应的格式化字符:%" << key << std::endl;
            abort();
            return FormatItem::ptr();
        }

    private:
        std::string _pattern;
        std::vector<FormatItem::ptr> _items;
    };
}

#endif
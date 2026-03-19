#ifndef __M_MESSAGE_H__
#define __M_MESSAGE_H__

#include <string>
#include <iostream>
#include <thread>
#include "level.hpp"
#include "util.hpp"

namespace ns_log
{
    struct Message
    {
        std::string _file;
        size_t _line;
        time_t _time;
        std::thread::id _tid;
        ns_log::LogLevel::value _level;
        std::string _payload;
        std::string _logger;

        Message(const ns_log::LogLevel::value &level, const std::string &file, const size_t &line, const std::string &logger, const std::string &payload)
            : _level(level), _file(file), _line(line), _logger(logger), _payload(payload), _time(ns_log::ns_util::Date::now()), _tid(std::this_thread::get_id())
        {
        }
    };
}

#endif
#ifndef __M_LOG_H__
#define __M_LOG_H__

#include "logger.hpp"

namespace ns_log
{

    Logger::ptr getLogger(const std::string &name)
    {
        return LoggerManager::getInstance().getLogger(name);
    }

    Logger::ptr rootLogger()
    {
        return LoggerManager::getInstance().rootLogger();
    }

#define debug(fmt, ...) debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define info(fmt, ...) info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define warn(fmt, ...) warn(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define error(fmt, ...) error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define fatal(fmt, ...) fatal(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define DEBUG(fmt, ...) ns_log::rootLogger()->debug(fmt, ##__VA_ARGS__)
#define INFO(fmt, ...) ns_log::rootLogger()->info(fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) ns_log::rootLogger()->warn(fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) ns_log::rootLogger()->error(fmt, ##__VA_ARGS__)
#define FATAL(fmt, ...) ns_log::rootLogger()->fatal(fmt, ##__VA_ARGS__)
}

#endif
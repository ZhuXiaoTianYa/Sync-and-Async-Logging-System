#ifndef __M_LEVEL_H__
#define __M_LEVEL_H__

#include <iostream>
#include <string>

namespace ns_log
{
    class LogLevel
    {
    public:
        enum class value
        {
            UNKNOW = 0,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            OFF
        };

        static const char *toString(const LogLevel::value &level)
        {
            switch (level)
            {
#define TOSTRING(name) #name
            case LogLevel::value::DEBUG:
                return TOSTRING(DEBUG);
            case LogLevel::value::INFO:
                return TOSTRING(INFO);
            case LogLevel::value::WARN:
                return TOSTRING(WARN);
            case LogLevel::value::ERROR:
                return TOSTRING(ERROR);
            case LogLevel::value::FATAL:
                return TOSTRING(FATAL);
            case LogLevel::value::OFF:
                return TOSTRING(OFF);
            }
            return TOSTRING(UNKNOW);
        }
    };
}

#endif
#ifndef __M_UTIL_H__
#define __M_UTIL_H__

#include <iostream>
#include <ctime>
#include <sys/stat.h>

namespace ns_log
{
    namespace ns_util
    {
        class Date
        {
        public:
            static size_t now()
            {
                return time(nullptr);
            }
        };

        class File
        {
        public:
            static bool exists(const std::string &pathname)
            {
                struct stat st;
                if (stat(pathname.c_str(), &st) < 0)
                {
                    return false;
                }
                return true;
                // return access(pathname.c_str(), F_OK) == 0;
            }
            static std::string path(const std::string &pathname)
            {
                // ./abc/bcd/de.txt
                size_t pos = pathname.find_last_of("/\\");
                if (pos == std::string::npos)
                {
                    return "."; // ./ ?
                }
                return pathname.substr(0, pos + 1);
            }
            static void createDirectory(const std::string &pathname)
            {
                // ./abc/bcd/des
                size_t pos = 0, idx = 0;
                while (idx < pathname.size())
                {
                    pos = pathname.find_first_of("/\\", idx);
                    if (pos == std::string::npos)
                    {
                        mkdir(pathname.c_str(), 0777);
                    }
                    std::string parent_dir = pathname.substr(0, pos + 1);
                    if (exists(parent_dir) == true)
                    {
                        idx = pos + 1;
                        continue;
                    }
                    // if (parent_dir == "." || parent_dir == "..")
                    // {
                    //     idx = pos + 1;
                    //     continue;
                    // }
                    mkdir(parent_dir.c_str(), 0777);
                    idx = pos + 1;
                }
            }
        };
    }
}

#endif
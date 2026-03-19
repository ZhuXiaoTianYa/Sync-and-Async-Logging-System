#include "util.hpp"
#include "level.hpp"
#include "message.hpp"

int main()
{
    // std::cout << ns_log::ns_util::Date::now() << std::endl;
    // std::string path = "./abc/def/ghe/file.txt";
    // ns_log::ns_util::File::createDirectory(ns_log::ns_util::File::path(path));

    std::cout << ns_log::LogLevel::toString(ns_log::LogLevel::value::DEBUG) << std::endl;
    std::cout << ns_log::LogLevel::toString(ns_log::LogLevel::value::INFO) << std::endl;
    std::cout << ns_log::LogLevel::toString(ns_log::LogLevel::value::WARN) << std::endl;
    std::cout << ns_log::LogLevel::toString(ns_log::LogLevel::value::ERROR) << std::endl;
    std::cout << ns_log::LogLevel::toString(ns_log::LogLevel::value::FATAL) << std::endl;
    std::cout << ns_log::LogLevel::toString(ns_log::LogLevel::value::OFF) << std::endl;
    std::cout << ns_log::LogLevel::toString(ns_log::LogLevel::value::UNKNOW) << std::endl;
    return 0;
}
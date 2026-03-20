#include "util.hpp"
#include "level.hpp"
#include "message.hpp"
#include "format.hpp"
#include "sink.hpp"

int main()
{
    // std::cout << ns_log::ns_util::Date::now() << std::endl;
    // std::string path = "./abc/def/ghe/file.txt";
    // ns_log::ns_util::File::createDirectory(ns_log::ns_util::File::path(path));

    // std::cout << ns_log::LogLevel::toString(ns_log::LogLevel::value::DEBUG) << std::endl;
    // std::cout << ns_log::LogLevel::toString(ns_log::LogLevel::value::INFO) << std::endl;
    // std::cout << ns_log::LogLevel::toString(ns_log::LogLevel::value::WARN) << std::endl;
    // std::cout << ns_log::LogLevel::toString(ns_log::LogLevel::value::ERROR) << std::endl;
    // std::cout << ns_log::LogLevel::toString(ns_log::LogLevel::value::FATAL) << std::endl;
    // std::cout << ns_log::LogLevel::toString(ns_log::LogLevel::value::OFF) << std::endl;
    // std::cout << ns_log::LogLevel::toString(ns_log::LogLevel::value::UNKNOW) << std::endl;

    // ns_log::Message msg(ns_log::LogLevel::value::INFO, "main.c", 56, "root", "测试日志格式化器");
    // ns_log::Formatter fmt("abc%%%g%g%gabc[%d{%H:%M:%S}] %m%g");
    // // ns_log::Formatter fmt;
    // std::string str = fmt.format(msg);
    // std::cout << str;

    ns_log::Message msg(ns_log::LogLevel::value::INFO, "main.c", 56, "root", "测试日志格式化器");
    ns_log::Formatter fmt;
    std::string str = fmt.format(msg);
    ns_log::SinkLog::ptr stdout_lsp = ns_log::SinkFactory::create<ns_log::StdoutSink>();
    ns_log::SinkLog::ptr file_lsp = ns_log::SinkFactory::create<ns_log::FileSink>("./logfile/test.log");
    ns_log::SinkLog::ptr roll_lsp = ns_log::SinkFactory::create<ns_log::RollBySizeSink>("./logfile/roll-", 1024 * 1024);
    stdout_lsp->log(str.c_str(), str.size());
    file_lsp->log(str.c_str(), str.size());
    int count = 0, cursize = 0;
    while (cursize < 1024 * 1024 * 10)
    {
        std::string tmp = str + std::to_string(count++);
        roll_lsp->log(tmp.c_str(), tmp.size());
        cursize += tmp.size();
    }

    return 0;
}
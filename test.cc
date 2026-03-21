#include "util.hpp"
#include "level.hpp"
#include "message.hpp"
#include "format.hpp"
#include "sink.hpp"
#include "logger.hpp"

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

    // ns_log::Message msg(ns_log::LogLevel::value::INFO, "main.c", 56, "root", "测试日志格式化器");
    // ns_log::Formatter fmt;
    // std::string str = fmt.format(msg);
    // ns_log::SinkLog::ptr stdout_lsp = ns_log::SinkFactory::create<ns_log::StdoutSink>();
    // ns_log::SinkLog::ptr file_lsp = ns_log::SinkFactory::create<ns_log::FileSink>("./logfile/test.log");
    // ns_log::SinkLog::ptr roll_lsp = ns_log::SinkFactory::create<ns_log::RollBySizeSink>("./logfile/roll-", 1024 * 1024);
    // stdout_lsp->log(str.c_str(), str.size());
    // file_lsp->log(str.c_str(), str.size());
    // int count = 0, cursize = 0;
    // while (cursize < 1024 * 1024 * 10)
    // {
    //     std::string tmp = str + std::to_string(count++);
    //     roll_lsp->log(tmp.c_str(), tmp.size());
    //     cursize += tmp.size();
    // }

    // ns_log::Message msg(ns_log::LogLevel::value::INFO, "main.c", 56, "root", "测试日志格式化器");
    // ns_log::Formatter fmt;
    // std::string str = fmt.format(msg);
    // ns_log::SinkLog::ptr roll_lsp = ns_log::SinkFactory::create<ns_log::RollByTimeSink>("./logfile/roll-", ns_log::TimeGap::GAP_SECOND, 1);
    // roll_lsp->log(str.c_str(), str.size());
    // time_t t = ns_log::ns_util::Date::now();
    // int count = 0;
    // while (ns_log::ns_util::Date::now() < t + 5)
    // {
    //     time_t tm = ns_log::ns_util::Date::now();
    //     std::string tmp = str + std::to_string(tm);
    //     roll_lsp->log(tmp.c_str(), tmp.size());
    // }

    // std::string logger_name = "sync_logger";
    // ns_log::LogLevel::value limit = ns_log::LogLevel::value::WARN;
    // std::string pattern = "[%d{%H:%M:%S}][%p][%c][%f:%l]%T%m%n";
    // ns_log::Formatter::ptr formatter(new ns_log::Formatter(pattern));
    // ns_log::SinkLog::ptr stdout_lsp = ns_log::SinkFactory::create<ns_log::StdoutSink>();
    // ns_log::SinkLog::ptr file_lsp = ns_log::SinkFactory::create<ns_log::FileSink>("./logfile/stdout/test.log");
    // ns_log::SinkLog::ptr roll_size_lsp = ns_log::SinkFactory::create<ns_log::RollBySizeSink>("./logfile/roll_size/roll-", 1024 * 1024);
    // ns_log::SinkLog::ptr roll_time_lsp = ns_log::SinkFactory::create<ns_log::RollByTimeSink>("./logfile/roll_time/roll-", ns_log::TimeGap::GAP_SECOND, 1);
    // std::vector<ns_log::SinkLog::ptr> sinks = {stdout_lsp, file_lsp, roll_size_lsp, roll_time_lsp};
    // ns_log::SyncLogger logger(logger_name, limit, formatter, sinks);
    // logger.debug(__FILE__, __LINE__, "%s", "debug");
    // logger.info(__FILE__, __LINE__, "%s", "info");
    // int count = 0, cursize = 0;
    // while (cursize < 1024 * 1024 * 10)
    // {
    //     logger.fatal(__FILE__, __LINE__, "%s-%d", "测试日志-", count++);
    //     cursize += 30;
    // }

    std::string logger_name = "sync_logger";
    std::string pattern = "[%d{%H:%M:%S}][%p][%c][%f:%l]%T%m%n";
    std::unique_ptr<ns_log::LoggerBuilder> builder(new ns_log::LocalLoggerBuilder());
    builder->buildFormatter(pattern);
    builder->buildLoggerLevel(ns_log::LogLevel::value::DEBUG);
    builder->buildLoggerName(logger_name);
    builder->buildLoggerType(ns_log::LoggerType::LOGGER_SYNC);
    builder->buildSink<ns_log::StdoutSink>();
    builder->buildSink<ns_log::FileSink>("./logfile/stdout/test.log");
    builder->buildSink<ns_log::RollBySizeSink>("./logfile/roll_size/roll-", 1024 * 1024);
    builder->buildSink<ns_log::RollByTimeSink>("./logfile/roll_time/roll-", ns_log::TimeGap::GAP_SECOND, 1);
    ns_log::Logger::ptr logger = builder->build();
    int count = 0, cursize = 0;
    while (cursize < 1024 * 1024 * 10)
    {
        logger->debug(__FILE__, __LINE__, "%s-%d", "测试日志-", count++);
        cursize += 30;
    }
    return 0;
}
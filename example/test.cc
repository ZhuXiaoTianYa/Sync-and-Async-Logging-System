#include "../log/log.hpp"

void test_log(const std::string &name)
{
    ns_log::Logger::ptr logger = ns_log::LoggerManager::getInstance().getLogger(name);
    INFO("%s", "测试开始");
    logger->debug("%s", "日志测试");
    logger->info("%s", "日志测试");
    logger->warn("%s", "日志测试");
    logger->error("%s", "日志测试");
    logger->fatal("%s", "日志测试");
    INFO("%s", "测试结束");
}

int main()
{
    std::string logger_name = "Sync_logger";
    std::string pattern = "[%d{%Y-%m-%d %H:%M:%S}][%p][%t][%c][%f:%l]%T%m%n";
    std::unique_ptr<ns_log::LoggerBuilder> builder(new ns_log::GlobalLoggerBuilder());
    builder->buildFormatter(pattern);
    builder->buildLoggerLevel(ns_log::LogLevel::value::DEBUG);
    builder->buildLoggerName(logger_name);
    builder->buildLoggerType(ns_log::LoggerType::LOGGER_SYNC);
    builder->buildSink<ns_log::StdoutSink>();
    builder->buildSink<ns_log::FileSink>("./logfile/logs.log");
    builder->buildSink<ns_log::RollBySizeSink>("./logfile/roll_by_size/roll-", 1024 * 1024);
    builder->buildSink<ns_log::RollByTimeSink>("./logfile/roll_by_time/roll-", ns_log::TimeGap::GAP_SECOND, 1);
    builder->build();
    test_log(logger_name);
    return 0;
}
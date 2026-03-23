#include "../log/log.hpp"
#include <chrono>

void bench(const std::string &logger_name, const size_t &thr_count, const size_t &msg_count, const size_t &msg_len)
{
    ns_log::Logger::ptr logger = ns_log::getLogger(logger_name);
    if (logger.get() == nullptr)
        return;
    std::cout << "测试日志: " << msg_count << " 条，总大小: " << (msg_len * msg_count) / 1024 << " KB\n";
    std::string msg(msg_len - 1, 'A');
    std::vector<std::thread> threads;
    std::vector<double> cost_arry(thr_count);
    size_t msg_per_thr = msg_count / thr_count;
    for (int i = 0; i < thr_count; i++)
    {
        threads.emplace_back([&, i]()
                             {
            auto start = std::chrono::high_resolution_clock::now();
            for (int j = 0; j < msg_per_thr; j++)
            {
                logger->fatal("%s", msg.c_str());
            }
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> cost = end - start;
            cost_arry[i] = cost.count();
            std::cout<< "线程" << i << ":\t输出数量: " << msg_per_thr << " 条，耗时: " << cost.count() << "s\n"; });
    }
    for (auto &thread : threads)
    {
        thread.join();
    }
    double max_cost = cost_arry[0];
    for (int i = 0; i < thr_count; i++)
    {
        max_cost = cost_arry[i] > max_cost ? cost_arry[i] : max_cost;
    }
    size_t msg_per_sec = msg_count / max_cost;
    size_t size_per_sec = (msg_count * msg_len) / (max_cost * 1024);
    std::cout << "\t总耗时: " << max_cost << " s\n";
    std::cout << "\t每秒输出日志数量: " << msg_per_sec << " 条\n";
    std::cout << "\t每秒输出日志大小: " << size_per_sec << " KB\n";
}

void sync_bench()
{
    std::unique_ptr<ns_log::LoggerBuilder> builder(new ns_log::GlobalLoggerBuilder());
    std::string logger_name = "Sync_logger";
    std::string pattern = "%m%n";
    builder->buildFormatter(pattern);
    builder->buildLoggerName(logger_name);
    builder->buildLoggerType(ns_log::LoggerType::LOGGER_SYNC);
    builder->buildSink<ns_log::FileSink>("./logfile/sync.log");
    builder->build();
    bench(logger_name, 1, 1000000, 100);
}

void async_bench()
{
    std::unique_ptr<ns_log::LoggerBuilder> builder(new ns_log::GlobalLoggerBuilder());
    std::string logger_name = "Async_logger";
    std::string pattern = "%m%n";
    builder->buildFormatter(pattern);
    builder->buildLoggerName(logger_name);
    builder->buildLoggerType(ns_log::LoggerType::LOGGER_ASYNC);
    builder->buildEnableUnSafeAsync();
    builder->buildSink<ns_log::FileSink>("./logfile/async.log");
    builder->build();
    bench(logger_name, 1, 1000000, 100);
}

int main()
{
    sync_bench();
    async_bench();
}
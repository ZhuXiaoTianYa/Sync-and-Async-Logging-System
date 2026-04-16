#include "../log/log.hpp"
#include <chrono>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <algorithm>
#include <iostream>
#include <iomanip>

using namespace std;

// 日志性能测试 + 计算 P99 / 平均延迟 / QPS / 吞吐量
void bench(const string &logger_name, size_t thr_count, size_t msg_count, size_t msg_len)
{
    auto logger = ns_log::getLogger(logger_name);
    if (!logger)
        return;

    cout << "======================================================" << endl;
    cout << "测试日志: " << msg_count << " 条, 单条长度: " << msg_len << " B" << endl;
    cout << "线程数量: " << thr_count << endl;

    string msg(msg_len - 1, 'A');
    vector<thread> threads;
    vector<long long> all_latency;
    mutex lat_mtx;

    size_t msg_per_thr = msg_count / thr_count;
    size_t remain = msg_count % thr_count;

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < thr_count; i++)
    {
        threads.emplace_back([&, i]()
                             {
            vector<long long> local_lat;
            size_t real_num = msg_per_thr + (i == thr_count - 1 ? remain : 0);

            for (size_t j = 0; j < real_num; j++)
            {
                auto s = chrono::high_resolution_clock::now();
                logger->fatal("%s", msg.c_str());
                auto e = chrono::high_resolution_clock::now();

                auto us = chrono::duration_cast<chrono::microseconds>(e - s).count();
                local_lat.push_back(us);
            }

            lock_guard<mutex> lock(lat_mtx);
            all_latency.insert(all_latency.end(), local_lat.begin(), local_lat.end()); });
    }

    for (auto &t : threads)
        t.join();
    auto end = chrono::high_resolution_clock::now();
    double total_sec = chrono::duration<double>(end - start).count();

    // 排序算 P99
    sort(all_latency.begin(), all_latency.end());
    long long avg = 0;
    for (auto v : all_latency)
        avg += v;
    avg /= all_latency.size();

    size_t p99_idx = all_latency.size() * 0.99;
    long long p99 = all_latency[p99_idx];

    double qps = msg_count / total_sec;
    double throughput = (msg_count * msg_len) / (total_sec * 1024 * 1024);

    cout << fixed << setprecision(2);
    cout << "总耗时: " << total_sec << " s" << endl;
    cout << "平均延迟: " << avg << " us" << endl;
    cout << "P99 延迟: " << p99 << " us" << endl;
    cout << "QPS: " << qps << " 条/秒" << endl;
    cout << "吞吐量: " << throughput << " MB/s" << endl;
    cout << "======================================================\n"
         << endl;
}

// 同步日志测试
void sync_test()
{
    unique_ptr<ns_log::LoggerBuilder> b(new ns_log::GlobalLoggerBuilder());
    b->buildLoggerName("sync");
    b->buildLoggerType(ns_log::LoggerType::LOGGER_SYNC);
    b->buildFormatter("%m%n");
    b->buildSink<ns_log::FileSink>("./sync.log");
    b->build();
    bench("sync", 1, 1000000, 100); // 1线程 10万条 100字节
}

// 异步日志测试
void async_test()
{
    unique_ptr<ns_log::LoggerBuilder> b(new ns_log::GlobalLoggerBuilder());
    b->buildLoggerName("async");
    b->buildLoggerType(ns_log::LoggerType::LOGGER_ASYNC);
    b->buildFormatter("%m%n");
    b->buildEnableUnSafeAsync();
    b->buildSink<ns_log::FileSink>("./async.log");
    b->build();
    bench("async", 1, 1000000, 100);
}

int main()
{
    sync_test();
    async_test();
    return 0;
}
# 同步与异步日志系统

## 项目简介

一个高性能的C++11日志系统，支持同步和异步两种模式，提供灵活的日志格式配置和多种输出目标，适用于各类C++应用场景。系统设计模块化，支持用户自定义日志落地方式。

## 核心特性

- **双模式支持**：同时支持同步和异步日志记录
- **高性能**：异步模式下采用双缓冲区设计，减少I/O阻塞
- **灵活配置**：支持自定义日志格式和多种输出目标
- **线程安全**：同步模式使用互斥锁，异步模式使用双缓冲区
- **日志滚动**：支持按大小和时间自动滚动日志文件
- **多级日志**：支持DEBUG、INFO、WARN、ERROR、FATAL等多级日志

## 性能测试

在单线程环境下，测试100万条日志（每条约100字节）的性能结果：

| 模式 | 耗时 | 每秒日志条数 | 每秒日志大小 |
|------|------|------------|------------|
| 同步模式 | 0.642s | 1,557,434 | 152,093 KB |
| 异步模式 | 0.779s | 1,282,802 | 125,273 KB |

## 系统架构

### 核心组件

1. **Logger**：日志记录器基类，定义了日志记录的接口
   - **SyncLogger**：同步日志实现，使用互斥锁保证线程安全
   - **AsyncLogger**：异步日志实现，使用双缓冲区和单独线程

2. **Formatter**：日志格式化器，支持自定义日志格式

3. **Sink**：日志输出目标
   - **StdoutSink**：输出到标准输出
   - **FileSink**：输出到文件
   - **RollBySizeSink**：按大小滚动日志文件
   - **RollByTimeSink**：按时间滚动日志文件

4. **AsyncLooper**：异步日志处理器，负责将日志从缓冲区写入目标

5. **LoggerManager**：日志管理器，负责管理和获取日志器

### 设计模式

- **Builder模式**：用于构建日志器，提供流畅的配置接口
- **工厂模式**：用于创建不同类型的Sink
- **单例模式**：用于日志管理器，确保全局唯一

## 快速开始

### 安装

1. 克隆仓库
   ```bash
   git clone https://github.com/ZhuXiaoTianYa/Sync-and-Async-Logging-System.git
   cd Sync-and-Async-Logging-System
   ```

2. 编译示例
   ```bash
   cd example
   make
   ./test
   ```

3. 运行基准测试
   ```bash
   cd benchmark
   make
   ./bench
   ```

### 基本使用

#### 同步日志

```cpp
#include "log/log.hpp"

int main() {
    // 创建同步日志器
    std::unique_ptr<ns_log::LoggerBuilder> builder(new ns_log::GlobalLoggerBuilder());
    builder->buildLoggerName("SyncLogger");
    builder->buildLoggerType(ns_log::LoggerType::LOGGER_SYNC);
    builder->buildSink<ns_log::FileSink>("./log/sync.log");
    builder->build();
    
    // 获取日志器
    auto logger = ns_log::getLogger("SyncLogger");
    
    // 记录日志
    logger->debug(__FILE__, __LINE__, "Debug message");
    logger->info(__FILE__, __LINE__, "Info message");
    logger->warn(__FILE__, __LINE__, "Warn message");
    logger->error(__FILE__, __LINE__, "Error message");
    logger->fatal(__FILE__, __LINE__, "Fatal message");
    
    return 0;
}
```

#### 异步日志

```cpp
#include "log/log.hpp"

int main() {
    // 创建异步日志器
    std::unique_ptr<ns_log::LoggerBuilder> builder(new ns_log::GlobalLoggerBuilder());
    builder->buildLoggerName("AsyncLogger");
    builder->buildLoggerType(ns_log::LoggerType::LOGGER_ASYNC);
    builder->buildEnableUnSafeAsync(); // 启用非安全异步模式以获得更高性能
    builder->buildSink<ns_log::FileSink>("./log/async.log");
    builder->build();
    
    // 获取日志器
    auto logger = ns_log::getLogger("AsyncLogger");
    
    // 记录日志
    logger->debug(__FILE__, __LINE__, "Debug message");
    logger->info(__FILE__, __LINE__, "Info message");
    logger->warn(__FILE__, __LINE__, "Warn message");
    logger->error(__FILE__, __LINE__, "Error message");
    logger->fatal(__FILE__, __LINE__, "Fatal message");
    
    return 0;
}
```

### 自定义日志格式

```cpp
// 使用自定义格式
std::string pattern = "[%d{%Y-%m-%d %H:%M:%S}][%p][%t][%c][%f:%l]\t%m\n";
builder->buildFormatter(pattern);
```

格式说明：
- `%d`：日期时间，可指定子格式如`%d{%Y-%m-%d %H:%M:%S}`
- `%t`：线程ID
- `%c`：日志器名称
- `%f`：源文件名
- `%l`：源文件行号
- `%p`：日志级别
- `%T`：制表符
- `%m`：日志消息
- `%n`：换行

### 日志滚动

#### 按大小滚动

```cpp
// 当文件大小超过10MB时滚动
builder->buildSink<ns_log::RollBySizeSink>("./log/roll_by_size", 10 * 1024 * 1024);
```

#### 按时间滚动

```cpp
// 每小时滚动一次
builder->buildSink<ns_log::RollByTimeSink>("./log/roll_by_time", ns_log::TimeGap::GAP_HOUR, 1);
```

## 技术实现

### 异步日志原理

异步日志采用双缓冲区设计：
1. 生产缓冲区：接收日志写入请求
2. 消费缓冲区：由后台线程处理写入到目标

当生产缓冲区满时，会通知后台线程交换缓冲区并处理日志。这种设计减少了日志写入对主线程的阻塞，提高了系统性能。

### 线程安全

- **同步模式**：使用`std::mutex`保证线程安全
- **异步模式**：通过双缓冲区和`std::condition_variable`实现线程间通信，避免了直接的线程竞争

### 内存管理

- 缓冲区自动扩容，默认初始大小为10MB
- 当缓冲区超过阈值（40MB）后，采用固定增量（10MB）扩容

## 应用场景

- **高并发系统**：使用异步模式减少I/O阻塞
- **实时监控**：使用同步模式保证日志及时输出
- **长时间运行的服务**：使用日志滚动功能避免日志文件过大
- **分布式系统**：可扩展支持网络输出目标

## 依赖

- C++11及以上
- 标准库：`iostream`, `fstream`, `mutex`, `thread`, `condition_variable`, `memory`, `functional`, `vector`, `unordered_map`

## 项目结构

```
sync-and-async-logging-system/
├── log/                 # 核心日志库
│   ├── logger.hpp       # 日志器实现
│   ├── looper.hpp       # 异步日志处理器
│   ├── sink.hpp         # 日志输出目标
│   ├── format.hpp       # 日志格式化
│   ├── buffer.hpp       # 缓冲区实现
│   ├── message.hpp      # 日志消息
│   ├── level.hpp        # 日志级别
│   ├── util.hpp         # 工具函数
│   └── log.hpp          # 统一头文件
├── example/             # 示例代码
│   ├── test.cc          # 基本使用示例
│   └── Makefile
├── benchmark/           # 性能测试
│   ├── benchmark.cc     # 基准测试代码
│   └── Makefile
└── README.md            # 项目说明
```

## 性能优化建议

1. **使用异步模式**：对于高并发场景，优先使用异步模式
2. **调整缓冲区大小**：根据实际日志量调整缓冲区大小
3. **合理设置日志级别**：生产环境建议使用INFO及以上级别
4. **使用日志滚动**：避免单个日志文件过大
5. **批量写入**：异步模式会自动批量处理日志，提高写入效率

## 未来规划

- 支持更多输出目标（如网络、数据库）
- 增加日志压缩功能
- 支持日志过滤和分类
- 提供配置文件支持
- 增加日志统计和分析功能

## 自定义日志落地方式

系统支持用户自定义日志落地方式，只需继承`SinkLog`基类并实现相应的方法即可。

### 步骤

1. **继承SinkLog类**
   ```cpp
   class CustomSink : public ns_log::SinkLog
   {
   public:
       CustomSink(const std::string &param) : _param(param) {
           // 初始化代码
       }
       
       void log(const char *data, const size_t &len) override {
           // 实现自定义的日志落地逻辑
           // 例如：写入到网络、数据库等
       }
       
   private:
       std::string _param;
   };
   ```

2. **使用自定义Sink**
   ```cpp
   // 创建日志器时使用自定义Sink
   std::unique_ptr<ns_log::LoggerBuilder> builder(new ns_log::GlobalLoggerBuilder());
   builder->buildLoggerName("CustomLogger");
   builder->buildSink<CustomSink>("custom_param"); // 传递构造参数
   builder->build();
   ```

### 示例：网络日志落地

```cpp
class NetworkSink : public ns_log::SinkLog
{
public:
    NetworkSink(const std::string &server_ip, int port) : _server_ip(server_ip), _port(port) {
        // 初始化网络连接
    }
    
    void log(const char *data, const size_t &len) override {
        // 实现网络发送逻辑
        // ...
    }
    
private:
    std::string _server_ip;
    int _port;
    // 网络连接相关成员
};

// 使用
// builder->buildSink<NetworkSink>("192.168.1.100", 8080);
```

## 许可证

MIT License

## 贡献

欢迎提交Issue和Pull Request，共同改进这个日志系统。

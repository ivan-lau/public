#include <string>
#include <cstdarg>
#include <cstdio>
#include <benchmark/benchmark.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <random>

enum LogLevel {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
};

class LogCaller {
public:
    void log_remote(unsigned char level, const char* format, const char* message) {
        // fmt::print("[{}] {}\n", level, message);
    }

    void log(unsigned char level, const char* format, const char* message) {
        // fmt::print("[{}] {}\n", level, message);
    }
};


class Logger1 {
public:
    Logger1(std::string name) : name(name) {}

    void info(const char* format, ...) const {
        va_list arglist;
        va_start(arglist, format);
        char buffer[2048];
        std::vsnprintf(buffer, sizeof(buffer), format, arglist);
        va_end(arglist);
        std::string message = makeLog(buffer);
        sendLog(LOG_INFO, message);
    }

private:
    std::string name;
    std::unique_ptr<LogCaller> logCaller = std::make_unique<LogCaller>();

    std::string makeLog(std::string message) const {
        return "[" + name + "] " + message + "\n";
    }

    void sendLog(unsigned char level, std::string const& message) const {
        if (logCaller) {
            if (message.size() < 512) {
                logCaller->log_remote(level, "%s", message.c_str());
            } else {
                auto const truncatedMessage = message.substr(0, 506) + " ... \n";
                logCaller->log_remote(level, "%s", truncatedMessage.c_str());
                logCaller->log(level, "%s", message.c_str());
            }
        }
    }
};

class Logger2 {
public:
    Logger2(std::string name) : name(std::move(name)) {}

    template<typename... Args>
    void logMessage(unsigned char level, std::string_view format, Args&&... args) const noexcept {
        char buffer[kMaxLogMessageSize];
        auto end = fmt::format_to_n(buffer, kMaxLogMessageSize, format, std::forward<Args>(args)...);
        if (end.out > buffer) {
            std::string_view message(buffer, end.out - buffer);
            sendLog(level, fmt::format("[{}] {}\n", name, message));
        }

    }

    template<typename... Args>
    void info(std::string_view format, Args&&... args) const noexcept {
        logMessage(LOG_INFO, format, std::forward<Args>(args)...);
    }

private:
    static constexpr std::size_t kMaxLogMessageSize = 2048;
    std::string name;
    std::unique_ptr<LogCaller> logCaller = std::make_unique<LogCaller>();

    void sendLog(unsigned char level, std::string_view message) const noexcept {
        if (logCaller) {
            if (message.size() < 512) {
                logCaller->log_remote(level, "%s", message.data());
            } else {
                auto const truncatedMessage = fmt::format( "{:.506} ... \n", message);
                logCaller->log_remote(level, "%s", truncatedMessage.c_str());
                logCaller->log(level, "%s", message.data());
            }
        }
    }
};

// int main() {
//     // Logger1 logger1("MyLogger1");
//     // logger1.info("This is a test log message with an integer: %d", 42);
//     Logger2 logger2("MyLogger2");
//     logger2.info("This is a test log message with an integer: {}", 42);


//     std::string longMessage(1024 * 1024, '-');
//     longMessage += "\n";
//     longMessage += "Additional text to make the message longer.";
//     // logger1.info(longMessage.c_str());
//     logger2.info(longMessage.c_str());

//     return 0;
// }

template <int N>
static void BM_Logger1InfoN(benchmark::State& state) {
    Logger1 logger("MyLogger");
    for (auto _ : state) {
        logger.info("Test message with {} arguments: ", N);
        for (int i = 0; i < N; ++i) {
            logger.info("{}", i);
        }
    }
    state.SetItemsProcessed(state.iterations());
}

BENCHMARK_TEMPLATE(BM_Logger1InfoN, 1);
BENCHMARK_TEMPLATE(BM_Logger1InfoN, 4);
BENCHMARK_TEMPLATE(BM_Logger1InfoN, 8);
BENCHMARK_TEMPLATE(BM_Logger1InfoN, 16);

template <int N>
static void BM_Logger2InfoN(benchmark::State& state) {
    Logger2 logger("MyLogger");
    for (auto _ : state) {
        logger.info("Test message with %d arguments: ", N);
        for (int i = 0; i < N; ++i) {
            logger.info("{}", i);
        }
    }
    state.SetItemsProcessed(state.iterations());
}

BENCHMARK_TEMPLATE(BM_Logger2InfoN, 1);
BENCHMARK_TEMPLATE(BM_Logger2InfoN, 4);
BENCHMARK_TEMPLATE(BM_Logger2InfoN, 8);
BENCHMARK_TEMPLATE(BM_Logger2InfoN, 16);


static void BM_Logger1_double(benchmark::State& state) {
    Logger1 logger("test_logger");

    for (auto _ : state) {
        logger.info("The value is %f", 3.14);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Logger1_double);

static void BM_Logger2_double(benchmark::State& state) {
    Logger2 logger("test_logger");

    for (auto _ : state) {
        logger.info("The value is {}", 3.14);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Logger2_double);

static void BM_Logger1_float(benchmark::State& state) {
    Logger1 logger("test_logger");

    for (auto _ : state) {
        logger.info("The value is %f", 3.14f);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Logger1_float);

static void BM_Logger2_float(benchmark::State& state) {
    Logger2 logger("test_logger");

    for (auto _ : state) {
        logger.info("The value is {}", 3.14f);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Logger2_float);

static void BM_Logger1_int(benchmark::State& state) {
    Logger1 logger("test_logger");

    for (auto _ : state) {
        logger.info("The value is %d", 42);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Logger1_int);

static void BM_Logger2_int(benchmark::State& state) {
    Logger2 logger("test_logger");

    for (auto _ : state) {
        logger.info("The value is {}", 42);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Logger2_int);

static void BM_Logger1_multipleTypes(benchmark::State& state) {
    Logger1 logger("test_logger");

    std::mt19937 gen;
    std::uniform_int_distribution<int> int_dist(0, 1000);
    std::uniform_real_distribution<double> double_dist(0.0, 1.0);
    std::uniform_real_distribution<float> float_dist(0.0f, 1.0f);
    std::string chars = "abcdefghijklmnopqrstuvwxyz";
    std::uniform_int_distribution<int> string_dist(0, chars.size() - 1);

    for (auto _ : state) {
        int i = int_dist(gen);
        double d = double_dist(gen);
        float f = float_dist(gen);
        std::string s(10, ' ');
        std::generate_n(s.begin(), s.size(), [&]() { return chars[string_dist(gen)]; });

        logger.info("i=%d d=%f f=%f s=%s", i, d, f, s.c_str());
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Logger1_multipleTypes);

static void BM_Logger2_multipleTypes(benchmark::State& state) {
    Logger2 logger("test_logger");

    std::mt19937 gen;
    std::uniform_int_distribution<int> int_dist(0, 1000);
    std::uniform_real_distribution<double> double_dist(0.0, 1.0);
    std::uniform_real_distribution<float> float_dist(0.0f, 1.0f);
    std::string chars = "abcdefghijklmnopqrstuvwxyz";
    std::uniform_int_distribution<int> string_dist(0, chars.size() - 1);

    for (auto _ : state) {
        int i = int_dist(gen);
        double d = double_dist(gen);
        float f = float_dist(gen);
        std::string s(10, ' ');
        std::generate_n(s.begin(), s.size(), [&]() { return chars[string_dist(gen)]; });

        logger.info("i={} d={} f={} s={}", i, d, f, s);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Logger2_multipleTypes);

static void BM_Logger1_long_args(benchmark::State& state) {
    Logger1 logger("test_logger");

    std::mt19937 gen;
    std::uniform_int_distribution<int> int_dist(0, 1000);
    std::uniform_real_distribution<double> double_dist(0.0, 1.0);
    std::uniform_real_distribution<float> float_dist(0.0f, 1.0f);
    std::string chars = "abcdefghijklmnopqrstuvwxyz";
    std::uniform_int_distribution<int> string_dist(0, chars.size() - 1);

    for (auto _ : state) {
        int i1 = int_dist(gen);
        int i2 = int_dist(gen);
        int i3 = int_dist(gen);
        double d1 = double_dist(gen);
        double d2 = double_dist(gen);
        double d3 = double_dist(gen);
        float f1 = float_dist(gen);
        float f2 = float_dist(gen);
        float f3 = float_dist(gen);
        std::string s1(10, ' ');
        std::generate_n(s1.begin(), s1.size(), [&]() { return chars[string_dist(gen)]; });
        std::string s2(10, ' ');
        std::generate_n(s2.begin(), s2.size(), [&]() { return chars[string_dist(gen)]; });
        std::string s3(10, ' ');
        std::generate_n(s3.begin(), s3.size(), [&]() { return chars[string_dist(gen)]; });

        logger.info("i1=%d i2=%d i3=%d d1=%f d2=%f d3=%f f1=%f f2=%f f3=%f s1=%s s2=%s s3=%s",
            i1, i2, i3, d1, d2, d3, f1, f2, f3, s1.c_str(), s2.c_str(), s3.c_str());
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Logger1_long_args);

static void BM_Logger2_long_args(benchmark::State& state) {
    Logger2 logger("test_logger");

    std::mt19937 gen;
    std::uniform_int_distribution<int> int_dist(0, 1000);
    std::uniform_real_distribution<double> double_dist(0.0, 1.0);
    std::uniform_real_distribution<float> float_dist(0.0f, 1.0f);
    std::string chars = "abcdefghijklmnopqrstuvwxyz";
    std::uniform_int_distribution<int> string_dist(0, chars.size() - 1);

    for (auto _ : state) {
        int i1 = int_dist(gen);
        int i2 = int_dist(gen);
        int i3 = int_dist(gen);
        double d1 = double_dist(gen);
        double d2 = double_dist(gen);
        double d3 = double_dist(gen);
        float f1 = float_dist(gen);
        float f2 = float_dist(gen);
        float f3 = float_dist(gen);
        std::string s1(10, ' ');
        std::generate_n(s1.begin(), s1.size(), [&]() { return chars[string_dist(gen)]; });
        std::string s2(10, ' ');
        std::generate_n(s2.begin(), s2.size(), [&]() { return chars[string_dist(gen)]; });
        std::string s3(10, ' ');
        std::generate_n(s3.begin(), s3.size(), [&]() { return chars[string_dist(gen)]; });

        logger.info("i1={} i2={} i3={} d1={} d2={} d3={} f1={} f2={} f3={} s1={} s2={} s3={}",
            i1, i2, i3, d1, d2, d3, f1, f2, f3, s1, s2, s3);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Logger2_long_args);

static void BM_Logger1_string_short(benchmark::State& state) {
    Logger1 logger("test_logger");
    std::string message = "This is a short message";

    for (auto _ : state) {
        logger.info("%s", message.c_str());
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Logger1_string_short);

static void BM_Logger2_string_short(benchmark::State& state) {
    Logger2 logger("test_logger");
    std::string message = "This is a short message";

    for (auto _ : state) {
        logger.info("{}", message);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Logger2_string_short);

static void BM_Logger1_string_longButLessThanBuf(benchmark::State& state) {
    Logger1 logger("test_logger");
    std::string message(1024, 'x');

    for (auto _ : state) {
        logger.info("%s", message.c_str());
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Logger1_string_longButLessThanBuf);

static void BM_Logger2_string_longButLessThanBuf(benchmark::State& state) {
    Logger2 logger("test_logger");
    std::string message(1024, 'x');

    for (auto _ : state) {
        logger.info("{}", message);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Logger2_string_longButLessThanBuf);


static void BM_LongFormat1Message(benchmark::State& state) {
    Logger1 logger("BenchmarkLogger");

    std::string longMessage(1024 * 1024, 'X');

    for (auto _ : state) {
        logger.info(longMessage.c_str());
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LongFormat1Message);

static void BM_LongFormat2Message(benchmark::State& state) {
    Logger2 logger("BenchmarkLogger");

    std::string longMessage(1024 * 1024, 'X');

    for (auto _ : state) {
        logger.info(longMessage.c_str());
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LongFormat2Message);

static void BM_LongSingleArg1Message(benchmark::State& state) {
    Logger1 logger("BenchmarkLogger");

    std::string longMessage(1024 * 1024, 'X');

    for (auto _ : state) {
        logger.info("This is a test log message with a long string: %s", longMessage);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LongSingleArg1Message);

static void BM_LongSingleArg2Message(benchmark::State& state) {
    Logger2 logger("BenchmarkLogger");

    std::string longMessage(1024 * 1024, 'X');

    for (auto _ : state) {
        logger.info("This is a test log message with a long string: {}", longMessage);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_LongSingleArg2Message);


BENCHMARK_MAIN();


// ------------------------------------------------------------------------------------------------------------------------------------


#include <benchmark/benchmark.h> 
#include <iostream>
#include <memory>
#include <string_view>

class ExternalAPI
{
public:
    virtual void on_created(int) = 0;
    virtual void on_start() = 0;
};

template<class T> struct IStrategy1
{
    void onChangeCallback(std::string_view instrumentId)
    {
        static_cast<T*>(this)->onChange (instrumentId) ;
    }
};

class Strategy1 : public IStrategy1<Strategy1>
{
public:
    void onChange(std::string_view instrumentId)
    {
        // std::cout << "Strategy2::onChange " << instrumentId << std::endl;
    }
};

class Instance1: public ExternalAPI
{
public:
    void on_created(int) override {
        strategy = std::make_unique<Strategy1>();
    }

    void on_start() override {
        std::string instrumentId = "instrumentId";
        strategy->onChangeCallback(instrumentId);
    }

private:
    std::unique_ptr<Strategy1> strategy;
};

struct IStrategy2
{
    virtual void onChange(std::string instrumentId) = 0;
};

class Strategy2 : public IStrategy2
{
public:
    void onChange(std::string instrumentId) override
    {
        // std::cout << "Strategy1::onChange " << instrumentId << std::endl;
    }
};

class Instance2: public ExternalAPI
{
public:
    void on_created(int) override {
        strategy = std::make_unique<Strategy2>();
    }

    void on_start() override {
        std::string instrumentId = "instrumentId";
        strategy->onChange(instrumentId);
    }

private:
    std::unique_ptr<Strategy2> strategy;
};



// int main()
// {
//     Instance1 instance1;
//     instance1.on_created(0);
//     instance1.on_start();
//     Instance2 instance2;
//     instance2.on_created(0);
//     instance2.on_start();
//     return 0;
// }



static void BM_CRTP(benchmark::State& state) {
    Instance1 instance;
    instance.on_created(0);

    for (auto _ : state) {
        instance.on_start();
    }
}

BENCHMARK(BM_CRTP);

static void BM_DynamicPolymorphism(benchmark::State& state) {
    Instance2 instance;
    instance.on_created(0);

    for (auto _ : state) {
        instance.on_start();
    }
}

BENCHMARK(BM_DynamicPolymorphism);

static void BM_CRTP_MultipleInstances(benchmark::State& state) {
    std::vector<Instance1> instances(state.range(0));
    for (auto& instance : instances) {
        instance.on_created(0);
    }

    for (auto _ : state) {
        for (auto& instance : instances) {
            instance.on_start();
        }
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

BENCHMARK(BM_CRTP_MultipleInstances)->Range(8, 512);

static void BM_DynamicPolymorphism_MultipleInstances(benchmark::State& state) {
    std::vector<Instance2> instances(state.range(0));
    for (auto& instance : instances) {
        instance.on_created(0);
    }

    for (auto _ : state) {
        for (auto& instance : instances) {
            instance.on_start();
        }
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

BENCHMARK(BM_DynamicPolymorphism_MultipleInstances)->Range(8, 512);

static void BM_CRTP_Polymorphic(benchmark::State& state) {
    std::vector<std::unique_ptr<IStrategy1<Strategy1>>> strategies(state.range(0));
    for (auto& strategy : strategies) {
        strategy = std::make_unique<Strategy1>();
    }

    for (auto _ : state) {
        for (auto& strategy : strategies) {
            strategy->onChangeCallback("instrumentId");
        }
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

BENCHMARK(BM_CRTP_Polymorphic)->Range(8, 512);

static void BM_DynamicPolymorphism_Polymorphic(benchmark::State& state) {
    std::vector<std::unique_ptr<Strategy2>> strategies(state.range(0));
    for (auto& strategy : strategies) {
        strategy = std::make_unique<Strategy2>();
    }

    for (auto _ : state) {
        for (auto& strategy : strategies) {
            strategy->onChange("instrumentId");
        }
    }

    state.SetItemsProcessed(state.iterations() * state.range(0));
}

BENCHMARK(BM_DynamicPolymorphism_Polymorphic)->Range(8, 512);

BENCHMARK_MAIN();

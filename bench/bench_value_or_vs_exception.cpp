#include <benchmark/benchmark.h>
#include <expected/expected.hpp>

#include <stdexcept>


int heavy_compute()
{
    volatile int x = 0;
    for (int i = 0; i < 100; ++i)
        x += i;
    return x;
}

static void BM_value_or_success(benchmark::State& state)
{
    for (auto _ : state)
    {
        std_::expected<int, std::string> e = heavy_compute();
        int v = e.value_or(-1);
        benchmark::DoNotOptimize(v);
    }
}

static void BM_exception_success(benchmark::State& state)
{
    for (auto _ : state)
    {
        try
        {
            int v = heavy_compute();
            benchmark::DoNotOptimize(v);
        }
        catch (const std::exception&)
        {
            benchmark::DoNotOptimize(-1);
        }
    }
}

static void BM_value_or_error(benchmark::State& state)
{
    for (auto _ : state)
    {
        std_::expected<int, std::string> e = std_::unexpected<std::string>("err");
        int v = e.value_or(-1);
        benchmark::DoNotOptimize(v);
    }
}

static void BM_exception_error(benchmark::State& state)
{
    for (auto _ : state)
    {
        try
        {
            throw std::runtime_error("err");
        }
        catch (const std::exception&)
        {
            benchmark::DoNotOptimize(-1);
        }
    }
}

BENCHMARK(BM_value_or_success);
BENCHMARK(BM_exception_success);
BENCHMARK(BM_value_or_error);
BENCHMARK(BM_exception_error);

BENCHMARK_MAIN();

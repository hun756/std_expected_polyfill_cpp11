#include <benchmark/benchmark.h>
#include <expected/expected.hpp>

#include <optional>
#include <stdexcept>


int heavy_compute()
{
    volatile int x = 0;
    for (int i = 0; i < 100; ++i)
        x += i;
    return x;
}

static void BM_expected_success(benchmark::State& state)
{
    for (auto _ : state)
    {
        std_::expected<int, std::string> e = heavy_compute();
        int v = e.value_or(-1);
        benchmark::DoNotOptimize(v);
    }
}

static void BM_optional_success(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::optional<int> o = heavy_compute();
        int v = o.value_or(-1);
        benchmark::DoNotOptimize(v);
    }
}

static void BM_expected_error(benchmark::State& state)
{
    for (auto _ : state)
    {
        std_::expected<int, std::string> e = std_::unexpected<std::string>(std::string(256, 'e'));
        int v = e.value_or(-1);
        benchmark::DoNotOptimize(v);
    }
}

static void BM_optional_throw_error(benchmark::State& state)
{
    for (auto _ : state)
    {
        try
        {
            throw std::runtime_error(std::string(256, 'e'));
        }
        catch (const std::exception&)
        {
            benchmark::DoNotOptimize(-1);
        }
    }
}

BENCHMARK(BM_expected_success);
BENCHMARK(BM_optional_success);
BENCHMARK(BM_expected_error);
BENCHMARK(BM_optional_throw_error);

BENCHMARK_MAIN();

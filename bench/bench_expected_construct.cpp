#include <benchmark/benchmark.h>
#include <expected/expected.hpp>

static void BM_expected_construct_success(benchmark::State& state)
{
    for (auto _ : state)
    {
        std_::expected<int, int> e = 42;
        benchmark::DoNotOptimize(e);
    }
}

static void BM_expected_construct_unexpected(benchmark::State& state)
{
    for (auto _ : state)
    {
        std_::expected<int, int> e = std_::unexpected<int>(-1);
        benchmark::DoNotOptimize(e);
    }
}

BENCHMARK(BM_expected_construct_success);
BENCHMARK(BM_expected_construct_unexpected);

BENCHMARK_MAIN();

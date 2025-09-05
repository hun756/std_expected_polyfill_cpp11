#include <benchmark/benchmark.h>
#include <expected/expected.hpp>

std_::expected<int, std::string> add_one(int x)
{
    return x + 1;
}
std_::expected<int, std::string> mul_two(int x)
{
    return x * 2;
}

static void BM_and_then_chain_success(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = std_::expected<int, std::string>(1).and_then(add_one).and_then(mul_two).and_then(
            add_one);
        benchmark::DoNotOptimize(r);
    }
}

static void BM_and_then_chain_error(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto r = std_::expected<int, std::string>(std_::unexpected<std::string>("err"))
                     .and_then(add_one)
                     .and_then(mul_two);
        benchmark::DoNotOptimize(r);
    }
}

BENCHMARK(BM_and_then_chain_success);
BENCHMARK(BM_and_then_chain_error);

BENCHMARK_MAIN();

#include <benchmark/benchmark.h>
#include <expected/expected.hpp>

#include <memory>
#include <string>



struct MoveOnly
{
    std::unique_ptr<int> p;

    MoveOnly(int v) : p(new int(v)) {}
    MoveOnly(MoveOnly&&) = default;
    MoveOnly& operator=(MoveOnly&&) = default;
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
};

static void BM_move_only_value_success(benchmark::State& state)
{
    for (auto _ : state)
    {
        std_::expected<MoveOnly, std::string> e = MoveOnly(42);
        benchmark::DoNotOptimize(e);
    }
}

static void BM_move_only_unexpected(benchmark::State& state)
{
    for (auto _ : state)
    {
        std_::expected<MoveOnly, std::string> e = std_::unexpected<std::string>("err");
        benchmark::DoNotOptimize(e);
    }
}

static void BM_large_value_move(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::string big(1'024, 'x');
        std_::expected<std::string, int> e = std::move(big);
        benchmark::DoNotOptimize(e);
    }
}

BENCHMARK(BM_move_only_value_success);
BENCHMARK(BM_move_only_unexpected);
BENCHMARK(BM_large_value_move);

BENCHMARK_MAIN();

#include <benchmark/benchmark.h>


static void DUMMY_BENCHMARK(benchmark::State& state) {
    // create a dummy bechmark case
    for (auto _ : state) {
        // Simulate some work
        benchmark::DoNotOptimize(_);
    }
}
BENCHMARK(DUMMY_BENCHMARK);


BENCHMARK_MAIN();
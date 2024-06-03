#include <benchmark/benchmark.h>

static void BM_SomeFunction(benchmark::State& state) {

    for (auto _ : state) {
        int x = 5;
        int y = 10;
        int z = x + y;
    }
}
// Register the function as a benchmark
BENCHMARK(BM_SomeFunction);

// Run the benchmark
BENCHMARK_MAIN();
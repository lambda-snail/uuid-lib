#include <iostream>
#include <uuid.h>
#include <uuid_factory.h>
#include <benchmark/benchmark.h>

using namespace LambdaSnail::Uuid;

static void BM_create_batch_dedicated_counter(benchmark::State& state) {
    auto const num_uuids = static_cast<uint16_t>(state.range(0));

    for (auto _ : state)
    {
        std::vector<uuid> uuid_vec;
        factory::create_uuids_dedicated_counter(num_uuids, uuid_vec);
    }
}

static void BM_create_batch_monotonic_counter(benchmark::State& state) {
    auto const num_uuids = static_cast<uint32_t>(state.range(0));
    for (auto _ : state)
    {
        std::vector<uuid> uuid_vec;
        factory::create_uuids_monotonic_random(num_uuids, 1, uuid_vec);
    }
}

static void BM_create_uuid_v4(benchmark::State& state) {

    for (auto _ : state)
    {
        uuid id;
        factory::create_uuid_v4(id);
    }
}

static void BM_create_uuid_v7(benchmark::State& state) {

    for (auto _ : state)
    {
        uuid id;
        factory::create_uuid_v7(id);
    }
}

BENCHMARK(BM_create_uuid_v4)->Iterations(1000000);
BENCHMARK(BM_create_uuid_v7)->Iterations(1000000);

BENCHMARK(BM_create_batch_dedicated_counter)->Arg(256)->Iterations(100000);
BENCHMARK(BM_create_batch_dedicated_counter)->Arg(1024)->Iterations(100000);
BENCHMARK(BM_create_batch_dedicated_counter)->Arg(4096)->Iterations(100000);

BENCHMARK(BM_create_batch_monotonic_counter)->Arg(1024)->Iterations(100000);
BENCHMARK(BM_create_batch_monotonic_counter)->Arg(4096)->Iterations(100000);
BENCHMARK(BM_create_batch_monotonic_counter)->Arg(10000)->Iterations(100000);
BENCHMARK(BM_create_batch_monotonic_counter)->Arg(100000)->Iterations(100000);

BENCHMARK_MAIN();
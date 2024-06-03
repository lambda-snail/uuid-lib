#include <iostream>
#include <uuid.h>
#include <uuid_factory.h>
#include <benchmark/benchmark.h>

using namespace LambdaSnail::Uuid;

static void BM_create_batch(benchmark::State& state) {


    auto const num_uuids = static_cast<uint16_t>(state.range(0));

    for (auto _ : state)
    {
        std::vector<uuid> uuid_vec;
        factory::create_uuids_fbdc(num_uuids, uuid_vec);
    }
}

static void BM_create_uuid_v4(benchmark::State& state) {

    for (auto _ : state)
    {
        uuid id(spec::g_uuid_v4_spec);
    }
}

static void BM_create_uuid_v7(benchmark::State& state) {

    for (auto _ : state)
    {
        uuid id(spec::g_uuid_v7_spec);
    }
}

BENCHMARK(BM_create_uuid_v4)->Iterations(1000000);
BENCHMARK(BM_create_uuid_v7)->Iterations(1000000);

BENCHMARK(BM_create_batch)->Arg(256)->Iterations(100000);
BENCHMARK(BM_create_batch)->Arg(1024)->Iterations(100000);
BENCHMARK(BM_create_batch)->Arg(4096)->Iterations(100000);

BENCHMARK_MAIN();
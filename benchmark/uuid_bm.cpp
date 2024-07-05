#include <iostream>
#include <uuid.h>
#include <uuid_factory.h>
#include <benchmark/benchmark.h>

#ifdef WIN32
#include "combaseapi.h"
#include "rpcdce.h"
#endif

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

static void BM_EqualityComparison(benchmark::State& state)
{
    for (auto _ : state)
    {
        uuid id1, id2;
        factory::create_uuid_v4(id1);
        factory::create_uuid_v4(id2);
        benchmark::DoNotOptimize( id1 == id2 );
    }
}

#ifdef WIN32

// Benchmark for comparison with Windows functions
static void BM_WIN_cocreate_guid(benchmark::State& state) {

    for (auto _ : state)
    {
        GUID id;
        benchmark::DoNotOptimize( CoCreateGuid( &id ) );
    }
}

static void BM_WIN_uuid_create_sequential(benchmark::State& state) {

    for (auto _ : state)
    {
        GUID id;
        benchmark::DoNotOptimize( UuidCreateSequential( &id ) );
    }
}

#endif



BENCHMARK(BM_create_uuid_v4);
// BENCHMARK(BM_create_uuid_v7);
// BENCHMARK(BM_WIN_cocreate_guid);
// BENCHMARK(BM_WIN_uuid_create_sequential);
//

BENCHMARK(BM_EqualityComparison)->Arg(false);
BENCHMARK(BM_EqualityComparison)->Arg(true);

// BENCHMARK(BM_create_batch_dedicated_counter)->Arg(256);
// BENCHMARK(BM_create_batch_dedicated_counter)->Arg(1024);
// BENCHMARK(BM_create_batch_dedicated_counter)->Arg(4096);
//
// BENCHMARK(BM_create_batch_monotonic_counter)->Arg(1024);
// BENCHMARK(BM_create_batch_monotonic_counter)->Arg(4096);
// BENCHMARK(BM_create_batch_monotonic_counter)->Arg(10000);
// BENCHMARK(BM_create_batch_monotonic_counter)->Arg(100000);

BENCHMARK_MAIN();
#include <uuid.h>
#include <uuid_factory.h>
#include <benchmark/benchmark.h>

static void BM_create_batch(benchmark::State& state) {
    using namespace LambdaSnail::Uuid;

    auto const num_uuids = static_cast<uint16_t>(state.range(0));
    std::vector<uuid> uuid_vec;
    factory::create_uuids_fbdc(num_uuids, uuid_vec);

    // for(uuid const& uuid: uuid_vec)
    // {
    //     std::cout << uuid.as_string() << std::endl;
    // }
}

BENCHMARK(BM_create_batch)->Arg(256);
//BENCHMARK(BM_create_batch)->Iterations(1000000)->Arg(256);

BENCHMARK_MAIN();
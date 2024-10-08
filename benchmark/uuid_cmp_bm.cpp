/**
* Benchmarks comparing different implementation ideas of the < operator for uuid.
 */
#include <immintrin.h>
#include <uuid.h>
#include <uuid_factory.h>
#include <benchmark/benchmark.h>

using namespace LambdaSnail::Uuid;

bool simple_loop_lt(uuid const &a, uuid const &b)
{
    for (uint8_t i = 0; i < 16; ++i)
    {
        if (a.octets[i] < b.octets[i]) return true;
    }

    return false;
}

bool simd_lt(uuid const &a, uuid const &b)
{
    __m128i const this_id = _mm_loadu_si128(reinterpret_cast<__m128i const*>(a.octets.data()));
    __m128i const other_id = _mm_loadu_si128(reinterpret_cast<__m128i const*>(b.octets.data()));

    __m128i const max = _mm_cmpeq_epi8(_mm_max_epu8(this_id, other_id), this_id);
    return not _mm_test_all_ones(max);
}

template <bool (*F)(uuid const& a, uuid const& b)>
static void BM_cmp_lt(benchmark::State& state) {
    uuid id1, id2;
    factory::create_uuid_v4(id1);
    factory::create_uuid_v4(id2);

    for (auto _ : state)
    {
        benchmark::DoNotOptimize( F(id1, id2) );
    }
}

//BENCHMARK_TEMPLATE(BM_cmp_lt, simple_loop_lt)->Name("(<) Simple Loop")->Repetitions(100);
//BENCHMARK_TEMPLATE(BM_cmp_lt, simd_lt)->Name("(<) SIMD")->Repetitions(100);
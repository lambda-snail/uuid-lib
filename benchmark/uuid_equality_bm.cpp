/**
 * Benchmarks comparing different implementation ideas of the == operator for uuid.
 */
#include <immintrin.h>
#include <uuid.h>
#include <uuid_factory.h>
#include <benchmark/benchmark.h>

using namespace LambdaSnail::Uuid;

bool simple_loop(uuid const& a, uuid const& b)
{
    for(uint8_t i = 0; i < 16; ++i)
    {
        if(a.octets[i] ^ b.octets[i]) return false;
    }

    return true;
}

bool loop_unrolled(uuid const& a, uuid const& b)
{
    return a.octets[0] == b.octets[0] and
    a.octets[1] == b.octets[1] and
    a.octets[2] == b.octets[2] and
    a.octets[3] == b.octets[3] and
    a.octets[4] == b.octets[4] and
    a.octets[5] == b.octets[5] and
    a.octets[6] == b.octets[6] and
    a.octets[7] == b.octets[7] and
    a.octets[8] == b.octets[8] and
    a.octets[9] == b.octets[9] and
    a.octets[10] == b.octets[10] and
    a.octets[11] == b.octets[11] and
    a.octets[12] == b.octets[12] and
    a.octets[13] == b.octets[13] and
    a.octets[14] == b.octets[14] and
    a.octets[15] == b.octets[15];
}

bool loop_unrolled_xor(uuid const& a, uuid const& b)
{
    return 0 == (a.octets[0] ^ b.octets[0]) |
        (a.octets[1] ^ b.octets[1]) |
        (a.octets[2] ^ b.octets[2]) |
        (a.octets[3] ^ b.octets[3]) |
        (a.octets[4] ^ b.octets[4]) |
        (a.octets[5] ^ b.octets[5]) |
        (a.octets[6] ^ b.octets[6]) |
        (a.octets[7] ^ b.octets[7]) |
        (a.octets[8] ^ b.octets[8]) |
        (a.octets[9] ^ b.octets[9]) |
        (a.octets[10] ^ b.octets[10]) |
        (a.octets[11] ^ b.octets[11]) |
        (a.octets[12] ^ b.octets[12]) |
        (a.octets[13] ^ b.octets[13]) |
        (a.octets[14] ^ b.octets[14]) |
        (a.octets[15] ^ b.octets[15]);
}

bool simd(uuid const& a, uuid const& b)
{
     __m128i const this_id = _mm_load_si128(reinterpret_cast<__m128i const*>(a.octets.data()));
     __m128i const other_id = _mm_load_si128(reinterpret_cast<__m128i const*>(b.octets.data()));

    __m128i const tmp = _mm_cmpeq_epi8( this_id, other_id );
    return _mm_test_all_ones(tmp);
}

enum class cmp_test_case
{
    SimpleLoop,
    UnrolledLoop,
    UnrolledLoopXOR,
    SIMD,
};

template <bool (*F)(uuid const& a, uuid const& b)>
static void BM_cmp_eq(benchmark::State& state) {
    uuid id1, id2;
    factory::create_uuid_v4(id1);
    id2 = id1;

    for (auto _ : state)
    {
        benchmark::DoNotOptimize( F(id1, id2) );
    }
}

template <bool (*F)(uuid const& a, uuid const& b)>
static void BM_cmp_ne(benchmark::State& state) {
    uuid id1, id2;
    factory::create_uuid_v4(id1);
    factory::create_uuid_v4(id2);

    for (auto _ : state)
    {
        benchmark::DoNotOptimize( F(id1, id2) );
    }
}

// BENCHMARK_TEMPLATE(BM_cmp_eq, simple_loop)->Name("(==) Simple Loop");//->UseManualTime();
// BENCHMARK_TEMPLATE(BM_cmp_ne, simple_loop)->Name("(!=) Simple Loop");//->UseManualTime();
// BENCHMARK_TEMPLATE(BM_cmp_eq, loop_unrolled)->Name("(==) Unrolled Loop")->Repetitions(100);
// BENCHMARK_TEMPLATE(BM_cmp_ne, loop_unrolled)->Name("(!=) Unrolled Loop")->Repetitions(100);
// BENCHMARK_TEMPLATE(BM_cmp_eq, loop_unrolled_xor)->Name("(==) Unrolled Loop XOR");//->UseManualTime();
// BENCHMARK_TEMPLATE(BM_cmp_ne, loop_unrolled_xor)->Name("(!=) Unrolled Loop XOR");//->UseManualTime();
// BENCHMARK_TEMPLATE(BM_cmp_eq, simd)->Name("(==) SIMD")->Repetitions(100);
// BENCHMARK_TEMPLATE(BM_cmp_ne, simd)->Name("(!=) SIMD")->Repetitions(100);
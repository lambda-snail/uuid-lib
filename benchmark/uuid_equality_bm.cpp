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
        if(a.m_octets[i] ^ b.m_octets[i]) return false;
    }

    return true;
}

bool loop_unrolled(uuid const& a, uuid const& b)
{
    return a.m_octets[0] == b.m_octets[0] and
    a.m_octets[1] == b.m_octets[1] and
    a.m_octets[2] == b.m_octets[2] and
    a.m_octets[3] == b.m_octets[3] and
    a.m_octets[4] == b.m_octets[4] and
    a.m_octets[5] == b.m_octets[5] and
    a.m_octets[6] == b.m_octets[6] and
    a.m_octets[7] == b.m_octets[7] and
    a.m_octets[8] == b.m_octets[8] and
    a.m_octets[9] == b.m_octets[9] and
    a.m_octets[10] == b.m_octets[10] and
    a.m_octets[11] == b.m_octets[11] and
    a.m_octets[12] == b.m_octets[12] and
    a.m_octets[13] == b.m_octets[13] and
    a.m_octets[14] == b.m_octets[14] and
    a.m_octets[15] == b.m_octets[15];
}

bool loop_unrolled_xor(uuid const& a, uuid const& b)
{
    return 0 == (a.m_octets[0] ^ b.m_octets[0]) |
        (a.m_octets[1] ^ b.m_octets[1]) |
        (a.m_octets[2] ^ b.m_octets[2]) |
        (a.m_octets[3] ^ b.m_octets[3]) |
        (a.m_octets[4] ^ b.m_octets[4]) |
        (a.m_octets[5] ^ b.m_octets[5]) |
        (a.m_octets[6] ^ b.m_octets[6]) |
        (a.m_octets[7] ^ b.m_octets[7]) |
        (a.m_octets[8] ^ b.m_octets[8]) |
        (a.m_octets[9] ^ b.m_octets[9]) |
        (a.m_octets[10] ^ b.m_octets[10]) |
        (a.m_octets[11] ^ b.m_octets[11]) |
        (a.m_octets[12] ^ b.m_octets[12]) |
        (a.m_octets[13] ^ b.m_octets[13]) |
        (a.m_octets[14] ^ b.m_octets[14]) |
        (a.m_octets[15] ^ b.m_octets[15]);
}

bool simd(uuid const& a, uuid const& b)
{
     __m128i const this_id = _mm_load_si128(reinterpret_cast<__m128i const*>(a.m_octets.data()));
     __m128i const other_id = _mm_load_si128(reinterpret_cast<__m128i const*>(b.m_octets.data()));

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
#include "uuid.h"
#include "xoroshiro128.h"

#include <immintrin.h>

namespace LambdaSnail::Uuid
{
    uuid const uuid::nil = uuid( 0x00 );
    uuid const uuid::max = uuid( 0xFF );

    uuid::uuid() : uuid( 0x00 ) {};

    uuid::uuid(uint8_t constant)
    {
        m_octets.fill(constant);
    }

    bool uuid::operator==(uuid const& other) const
    {
        // 3.7 ns / 9.6 ns
        // for(uint8_t i = 0; i < 16; ++i)
        // {
        //     if(m_octets[i] != other.m_octets[i]) return false;
        // }
        //
        // return true;

         __m128i const this_id = _mm_load_si128(reinterpret_cast<__m128i const*>(m_octets.data()));
         __m128i const other_id = _mm_load_si128(reinterpret_cast<__m128i const*>(other.m_octets.data()));

        __m128i const tmp = _mm_cmpeq_epi8( this_id, other_id );
        return _mm_test_all_ones(tmp);

        // 3.85 ns / 6.28 ns
        // return m_octets[0] == other.m_octets[0] and
        // m_octets[1] == other.m_octets[1] and
        // m_octets[2] == other.m_octets[2] and
        // m_octets[3] == other.m_octets[3] and
        // m_octets[4] == other.m_octets[4] and
        // m_octets[5] == other.m_octets[5] and
        // m_octets[6] == other.m_octets[6] and
        // m_octets[7] == other.m_octets[7] and
        // m_octets[8] == other.m_octets[8] and
        // m_octets[9] == other.m_octets[9] and
        // m_octets[10] == other.m_octets[10] and
        // m_octets[11] == other.m_octets[11] and
        // m_octets[12] == other.m_octets[12] and
        // m_octets[13] == other.m_octets[13] and
        // m_octets[14] == other.m_octets[14] and
        // m_octets[15] == other.m_octets[15];
    }

    bool uuid::operator<(const uuid &) const
    {
        return true;
    }

    constexpr uuid::uuid(octet_set_t const &bytes)
    {
        m_octets = bytes;
    }

    std::string uuid::as_string() const
    {
        return std::format(
            "{:02x}{:02x}{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}",
            m_octets[0], m_octets[1], m_octets[2], m_octets[3],
            m_octets[4], m_octets[5],
            m_octets[6], m_octets[7],
            m_octets[8], m_octets[9],
            m_octets[10], m_octets[11], m_octets[12], m_octets[13], m_octets[14], m_octets[15]
        );
    }

    std::string uuid::as_string_braced() const
    {
        return std::format(
            "{{{:02x}{:02x}{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}}}",
            m_octets[0], m_octets[1], m_octets[2], m_octets[3],
            m_octets[4], m_octets[5],
            m_octets[6], m_octets[7],
            m_octets[8], m_octets[9],
            m_octets[10], m_octets[11], m_octets[12], m_octets[13], m_octets[14], m_octets[15]
        );
    }
};
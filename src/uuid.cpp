#include "uuid.hpp"

#include <immintrin.h>

namespace LambdaSnail::Uuid
{
    uuid const uuid::nil = uuid( 0x00 );
    uuid const uuid::max = uuid( 0xFF );

    uuid::uuid() : uuid( 0x00 ) {};

    uuid::uuid(uint8_t constant)
    {
        octets.fill(constant);
    }

    bool uuid::operator==(uuid const& other) const
    {
#ifdef UUID_LIB_USE_SIMD
          __m128i const this_id = _mm_load_si128(reinterpret_cast<__m128i const*>(octets.data()));
          __m128i const other_id = _mm_load_si128(reinterpret_cast<__m128i const*>(other.octets.data()));

         __m128i const tmp = _mm_cmpeq_epi8( this_id, other_id );
         return _mm_test_all_ones(tmp);
#elif
        return m_octets[0] == other.m_octets[0] and
        m_octets[1] == other.m_octets[1] and
        m_octets[2] == other.m_octets[2] and
        m_octets[3] == other.m_octets[3] and
        m_octets[4] == other.m_octets[4] and
        m_octets[5] == other.m_octets[5] and
        m_octets[6] == other.m_octets[6] and
        m_octets[7] == other.m_octets[7] and
        m_octets[8] == other.m_octets[8] and
        m_octets[9] == other.m_octets[9] and
        m_octets[10] == other.m_octets[10] and
        m_octets[11] == other.m_octets[11] and
        m_octets[12] == other.m_octets[12] and
        m_octets[13] == other.m_octets[13] and
        m_octets[14] == other.m_octets[14] and
        m_octets[15] == other.m_octets[15];
#endif
    }

    bool uuid::operator<(const uuid & other) const
    {
#ifdef UUID_LIB_USE_SIMD
        __m128i const this_id = _mm_loadu_si128(reinterpret_cast<__m128i const*>(this->octets.data()));
        __m128i const other_id = _mm_loadu_si128(reinterpret_cast<__m128i const*>(other.octets.data()));

        __m128i const cmp_max = _mm_cmpeq_epi8(_mm_max_epu8(this_id, other_id), this_id);
        return not _mm_test_all_ones(cmp_max);
#else
        for (uint8_t i = 0; i < 16; ++i)
        {
            if (a.m_octets[i] < b.m_octets[i]) return true;
        }

        return false;
#endif
    }

    constexpr uuid::uuid(octet_set_t const &bytes)
    {
        octets = bytes;
    }

    std::string uuid::as_string() const
    {
        return std::format(
            "{:02x}{:02x}{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}",
            octets[0], octets[1], octets[2], octets[3],
            octets[4], octets[5],
            octets[6], octets[7],
            octets[8], octets[9],
            octets[10], octets[11], octets[12], octets[13], octets[14], octets[15]
        );
    }

    std::string uuid::as_string_braced() const
    {
        return std::format(
            "{{{:02x}{:02x}{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}}}",
            octets[0], octets[1], octets[2], octets[3],
            octets[4], octets[5],
            octets[6], octets[7],
            octets[8], octets[9],
            octets[10], octets[11], octets[12], octets[13], octets[14], octets[15]
        );
    }
};
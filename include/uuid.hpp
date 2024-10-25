#pragma once

#include <array>
#include <cstdint>
#include <format>
#include <string>
#include <string>

#ifdef UUID_LIB_USE_SIMD
#include <immintrin.h>
#endif

namespace LambdaSnail::Uuid
{
    /**
     * Some systems generate or expect UUIDs that are enclosed in braces or have other syntactical quirks. This
     * mode tries to facilitate interop with such systems when outputting a uuid as a string.
     */
    enum class string_conversion_mode
    {
        standard,
        curly_braces,
        no_dash
    };

    /**
     * The UUID class really only holds octet data. Different versions of UUID are constructed using the provided factory functions.
     * The "raw" octet data is exposed to the user, so it should be relatively straightforward to implement a new UUID version. Thus,
     * this library puts a lot of responsibility on the user, should (s)he wish to make changes to a uuid or create their own.
     *
     * The library allows users to use built-in random generators, or provide their own if they wish.
     *
     * @link https://datatracker.ietf.org/doc/html/rfc9562#name-requirements-language
     */
    struct uuid
    {
        typedef std::array<uint8_t, 16> octet_set_t;

        /**
         * Creates an empty uuid.
         */
        explicit uuid();

        /**
         * Creates a UUID using the provided sequence of bytes. This will initialize the UUID with a copy of the
         * byte array, and it is up to the caller to make sure that the resulting UUID is compliant with the standard.
         * This can be useful in certain scenarios where the UUID has already been created, possibly by an external source,
         * such as deserialization.
         */
        explicit constexpr uuid(octet_set_t const& bytes);

        /**
         * Creates a UUID where all octets are filled with the same value.
         * @warning This can be used to create non-standard compliant UUIDs. The standard only defines the UUIDs
         * 'max' and 'nil'.
         */
        explicit uuid(uint8_t constant);

        bool operator==(const uuid&) const;
        bool operator<(const uuid&) const;

        /**
         * Returns a string representation of the UUID.
         */
        template<string_conversion_mode M = string_conversion_mode::standard>
        [[nodiscard]] std::string as_string() const;

        /**
         * The empty or nil UUID, with all octets set to 0.
         */
        static const uuid nil;

        /**
         * The max UUID, with all octets set to 0xFF.
         */
        static const uuid max;

        alignas(16)
        octet_set_t octets{};
    };

    uuid const uuid::nil = uuid( 0x00 );
    uuid const uuid::max = uuid( 0xFF );

    uuid::uuid() : uuid( 0x00 ) {}

    uuid::uuid(uint8_t constant)
    {
        octets.fill(constant);
    }

    inline bool uuid::operator==(uuid const& other) const
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

    inline bool uuid::operator<(const uuid& other) const
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

    constexpr uuid::uuid(octet_set_t const& bytes)
    {
        octets = bytes;
    }

    template<string_conversion_mode Mode>
    inline std::string uuid::as_string() const
    {
        auto format_resolver = [](string_conversion_mode m) consteval
        {
            switch(m)
            {
                case string_conversion_mode::curly_braces:
                    return "{{{:02x}{:02x}{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}}}";
                case string_conversion_mode::no_dash:
                    return "{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}";
                case string_conversion_mode::standard:
                    return "{:02x}{:02x}{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}";
            }
        };

        return std::format(
                format_resolver(Mode),
                octets[0], octets[1], octets[2], octets[3],
                octets[4], octets[5],
                octets[6], octets[7],
                octets[8], octets[9],
                octets[10], octets[11], octets[12], octets[13], octets[14], octets[15]
        );
    }
}

#pragma once

#include <array>
#include <chrono>

#include "xoroshiro128.h"
#include "uuid_types.h"

namespace LambdaSnail::Uuid::spec
{
    static inline constexpr size_t version_octet = 6;
    static inline constexpr size_t variant_octet = 8;

    template<typename rng_t>
    struct uuid_v4_spec
    {
        void init_fields(octet_set_t& octets, rng_t random_generator) const;

    private:
        static void set_version_bits(octet_set_t &octets);
        static void set_variant_bits(octet_set_t &octets);
    };

    template<typename rng_t>
    struct uuid_v7_spec
    {
        void init_fields(octet_set_t& octets, rng_t random_generator) const;

        void set_ts_ms(octet_set_t& octets, uint64_t raw_ts) const;
        void set_rand_a(octet_set_t& octets, uint16_t value) const;
        void set_rand_b(octet_set_t& octets, uint64_t value) const;
    };

    inline static constexpr uuid_v4_spec<xoroshiro128pp> g_uuid_v4_spec = {};
    inline static constexpr uuid_v7_spec<xoroshiro128pp> g_uuid_v7_spec = {};

    template <typename rng_t>
    void uuid_v4_spec<rng_t>::set_version_bits(octet_set_t &octets) {
        // Most significant bits in version octet set to 0100
        uint8_t constexpr b1 = ~(static_cast<uint8_t>(1) << 7);
        uint8_t constexpr b2 = (static_cast<uint8_t>(1) << 6);
        uint8_t constexpr b3 = ~(static_cast<uint8_t>(1) << 5);
        uint8_t constexpr b4 = ~(static_cast<uint8_t>(1) << 4);

        octets[version_octet] &= b1;
        octets[version_octet] |= b2;
        octets[version_octet] &= b3;
        octets[version_octet] &= b4;
    }

    template <typename rng_t>
    void uuid_v4_spec<rng_t>::set_variant_bits(octet_set_t &octets) {
        // Most significant bits in variant octet set to 10
        auto constexpr b1 = (static_cast<uint8_t>(1) << 7);
        auto constexpr b2 = ~(static_cast<uint8_t>(1) << 6);

        octets[variant_octet] |= b1;
        octets[variant_octet] &= b2;
    }

    template<typename rng_t = xoroshiro128pp>
    void uuid_v4_spec<rng_t>::init_fields(octet_set_t& octets, rng_t random_generator) const
    {
        uint64_t const n1 = random_generator.next();
        uint64_t const n2 = random_generator.next();

        // We don't care about endianness here, since a random integer is still a random integer even
        // after shuffling the bytes around
        memcpy(octets.data(), &n1, sizeof(uint64_t));
        memcpy(octets.data() + octets.size()/2, &n2, sizeof(uint64_t));

        octets[version_octet] &= 0b01001111; // Most significant bits set to 0100
        octets[variant_octet] &= 0b10111111; // Most significant bits set to 10

        set_version_bits(octets);
        set_variant_bits(octets);
    }

    template<typename rng_t>
    void uuid_v7_spec<rng_t>::init_fields(octet_set_t& octets, rng_t random_generator) const
    {
        // Set timestampt bits
        std::chrono::time_point<std::chrono::system_clock> const now = std::chrono::system_clock::now();
        int64_t const time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

        set_ts_ms(octets, time_stamp);

        // Set random bits
        uint64_t const rand_a = random_generator.next();
        uint64_t const rand_b = random_generator.next();

        set_rand_a(octets, rand_a);
        set_rand_b(octets, rand_b);
    }

    template<typename rng_t>
    void uuid_v7_spec<rng_t>::set_ts_ms(octet_set_t &octets, uint64_t raw_ts) const
    {
        memcpy(octets.data(), &raw_ts, sizeof(int64_t));
        if constexpr (std::endian::native == std::endian::little)
        {
            std::swap(octets[0], octets[5]);
            std::swap(octets[1], octets[4]);
            std::swap(octets[2], octets[3]);
        }
    }

    template<typename rng_t>
    void uuid_v7_spec<rng_t>::set_rand_a(octet_set_t &octets, uint16_t value) const
    {
        // Most significant bits in version octet set to 0111
        uint16_t constexpr b1 = ~(static_cast<uint16_t>(1) << 15);
        uint16_t constexpr b2 = (static_cast<uint16_t>(1) << 14);
        uint16_t constexpr b3 = (static_cast<uint16_t>(1) << 13);
        uint16_t constexpr b4 = (static_cast<uint16_t>(1) << 12);

        value &= b1;
        value |= b2;
        value |= b3;
        value |= b4;

        memcpy(octets.data() + version_octet, &value, sizeof(uint16_t));

        // If we are on little endian the most significant byte is in the 'wrong' place, so swap
        if constexpr (std::endian::native == std::endian::little)
        {
            std::swap(octets[version_octet], octets[version_octet+1]);
        }
    }

    template<typename rng_t>
    void uuid_v7_spec<rng_t>::set_rand_b(octet_set_t &octets, uint64_t value) const
    {
        // Most significant bits set to 10
        uint64_t constexpr b1 = (static_cast<uint64_t>(1) << 63);
        uint64_t constexpr b2 = ~(static_cast<uint64_t>(1) << 62);

        value |= b1;
        value &= b2;

        memcpy(octets.data() + octets.size()/2, &value, sizeof(int64_t));
        if constexpr (std::endian::native == std::endian::little)
        {
            std::swap(octets[8], octets[15]);
            std::swap(octets[9], octets[14]);
            std::swap(octets[10], octets[13]);
            std::swap(octets[11], octets[12]);
        }
    }
}

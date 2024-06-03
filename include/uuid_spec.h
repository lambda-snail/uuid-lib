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

    template<typename rng_t = xoroshiro128pp>
    void uuid_v4_spec<rng_t>::init_fields(octet_set_t& octets, rng_t random_generator) const
    {
        if(not random_generator.is_seeded())
        {
            random_generator.seed_state();
        }

        random_generator.long_jump();
        uint64_t const n1 = random_generator.next();
        uint64_t const n2 = random_generator.next();

        memcpy(octets.data(), &n1, sizeof(uint64_t));
        memcpy(octets.data() + octets.size()/2, &n2, sizeof(uint64_t));

        octets[version_octet] &= 0b01001111; // Most significant bits set to 0100
        octets[variant_octet] &= 0b10111111; // Most significant bits set to 10
    }

    template<typename rng_t>
    void uuid_v7_spec<rng_t>::init_fields(octet_set_t& octets, rng_t random_generator) const
    {
        // Set timestampt bits
        std::chrono::time_point<std::chrono::system_clock> const now = std::chrono::system_clock::now();
        int64_t const time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

        set_ts_ms(octets, time_stamp);

        if(not random_generator.is_seeded())
        {
            random_generator.seed_state();
        }

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
        //octets[version_octet] &= 0b11110111; // Most significant bits set to 0111
        value &= 0b0111111111111111; // Most significant bits set to 0111
        memcpy(octets.data() + version_octet, &value, sizeof(uint16_t));

        if constexpr (std::endian::native == std::endian::little)
        {
            std::swap(octets[version_octet], octets[version_octet+1]);
        }
    }

    template<typename rng_t>
    void uuid_v7_spec<rng_t>::set_rand_b(octet_set_t &octets, uint64_t value) const
    {
        //octets[variant_octet] &= 0b11111110; // Most significant bits set to 10
        constexpr uint64_t variant_mask = ((std::numeric_limits<uint64_t>::max() << 62)-1);
        value &= variant_mask; // Most significant bits set to 10

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

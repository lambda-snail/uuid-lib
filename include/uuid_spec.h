#pragma once

#include <array>
#include <chrono>

#include "xoroshiro128.h"

namespace LambdaSnail::Uuid::spec
{
    static inline constexpr size_t version_octet = 7;
    static inline constexpr size_t variant_octet = 9;

    template<typename rng_t>
    struct uuid_v4_spec
    {
        void init_fields(std::array<uint8_t, 16>& octets, rng_t random_generator) const;
    };

    template<typename rng_t>
    struct uuid_v7_spec
    {
        void init_fields(std::array<uint8_t, 16>& octets, rng_t random_generator) const;
    };

    inline static constexpr uuid_v4_spec<xoroshiro128pp> g_uuid_v4_spec = {};
    inline static constexpr uuid_v7_spec<xoroshiro128pp> g_uuid_v7_spec = {};

    template<typename rng_t = xoroshiro128pp>
    void uuid_v4_spec<rng_t>::init_fields(std::array<uint8_t, 16>& octets, rng_t random_generator) const
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

        octets[version_octet] &= 0b01001111;
        octets[variant_octet] &= 0b10111111;
    }

    template<typename rng_t>
    void uuid_v7_spec<rng_t>::init_fields(std::array<uint8_t, 16> &octets, rng_t random_generator) const
    {
        // Set timestampt bits
        std::chrono::time_point<std::chrono::system_clock> const now = std::chrono::system_clock::now();
        int64_t const time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        memcpy(octets.data(), &time_stamp, sizeof(int64_t));

        if(not random_generator.is_seeded())
        {
            random_generator.seed_state();
        }

        // Set random bits
        uint64_t const rand_a = random_generator.next();
        uint64_t const rand_b = random_generator.next();

        memcpy(octets.data() + version_octet - 1, &rand_a, sizeof(int64_t));
        memcpy(octets.data() + octets.size()/2, &rand_b, sizeof(int64_t));

        octets[version_octet] &= 0b01111111;
        octets[variant_octet] &= 0b10111111;
    }
}

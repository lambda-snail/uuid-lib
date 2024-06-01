#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <memory>

#include "xoroshiro128.h"

namespace LambdaSnail
{
    static inline constexpr size_t version_octet = 7;
    static inline constexpr size_t variant_octet = 9;

    static inline std::unique_ptr<xoroshiro128pp> default_generator = std::make_unique<xoroshiro128pp>();

    template<typename rng_t = xoroshiro128pp>
    struct uuid_variant_v4
    {
        static void init_fields(std::array<uint8_t, 16>& octets, rng_t random_generator);
    };

    template<typename rng_t = xoroshiro128pp>
    struct uuid_variant_v7
    {
        static void init_fields(std::array<uint8_t, 16>& octets, rng_t random_generator);
    };


    template<typename uuid_variant = uuid_variant_v4>
    class uuid
    {
    public:
        uuid();

        explicit uuid(std::array<uint8_t, 16> const& bytes);

        template<typename rng_t = xoroshiro128pp>
        explicit uuid(rng_t& random_generator);

        static uuid const nil_uuid;
        static uuid const max_uuid;

    private:

        std::array<uint8_t, 16> octets {};
    };

    template<typename rng_t = xoroshiro128pp>
    void uuid_variant_v4<rng_t>::init_fields(std::array<uint8_t, 16>& octets, rng_t random_generator)
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

    template<typename uuid_variant = uuid_variant_v4>
    template<typename rng_t>
    uuid<uuid_variant>::uuid(rng_t& random_generator)
    {
        uuid_variant::init_fields(octets, random_generator);
    }

    template<typename uuid_variant = uuid_variant_v4>
    uuid<uuid_variant>::uuid()
    {
        uuid_variant::init_fields(octets, *default_generator);
    }

    using guid      = uuid<uuid_variant_v4<xoroshiro128pp>>;
    using guid_v4   = uuid<uuid_variant_v4<xoroshiro128pp>>;
    using uuid_v4   = uuid<uuid_variant_v4<xoroshiro128pp>>;

    using guid_v7   = uuid<uuid_variant_v7<xoroshiro128pp>>;
    using uuid_v7   = uuid<uuid_variant_v7<xoroshiro128pp>>;
}

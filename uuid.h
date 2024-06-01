#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <format>
#include <string>
#include <memory>
#include <string>

#include "xoroshiro128.h"

namespace LambdaSnail
{
    static inline constexpr size_t version_octet = 7;
    static inline constexpr size_t variant_octet = 9;

    static inline std::unique_ptr<xoroshiro128pp> default_generator = std::make_unique<xoroshiro128pp>();

    template<uint8_t constant>
    struct uuid_variant_constant
    {
        static void init_fields(std::array<uint8_t, 16>& octets);
    };

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


    /**
     * Represents a subset of a universally unique identifier (UUID) as specified in rfc9562.
     *
     * Currently only uuid version 4 is implemented. The standard also defines two special uuid's called 'nil' and 'max'.
     * They are implemented as a special type of variant.
     *
     * The uuid class holds the octet data, and different versions can be implemented using the strategy patern by specifying
     * a variant class as a template type parameter. The variant classes are responsible for initializing the octets of the uuid,
     * and it is possible to extend the system to uuid versions that are not implemented if needed. Since there is no way to
     * check arbitrary code for standards compliance, this means that users of the library can effectivel define any kind
     * of uuid that they want - this corresponds to version 8 in the standard.
     *
     * A variant definition must implement a function with the signature
     *
     * static void init_fields(std::array<uint8_t, 16>&, rng_t)
     *
     * where rng_t is a template type parameter that will also be passed to the ctor of the uuid. It can be used to specify
     * implementation specific random number generators if the defaul one that comes with the library is not sufficient for
     * whatever reason. The default random number generator is called xoroshiro128pp and will be used by default.
     *
     * @link https://datatracker.ietf.org/doc/html/rfc9562#name-requirements-language
     *
     * The random number generator has been adapted from:
     * @link https://xoroshiro.di.unimi.it/xoroshiro128plusplus.c
     */
    template<typename uuid_variant = uuid_variant_v4>
    class uuid
    {
    public:
        inline uuid();

        explicit uuid(std::array<uint8_t, 16> const& bytes);

        template<typename rng_t = xoroshiro128pp>
        explicit uuid(rng_t& random_generator);

        [[nodiscard]] std::string as_string() const;

    private:

        std::array<uint8_t, 16> octets {};
    };

    template<typename uuid_variant>
    std::string uuid<uuid_variant>::as_string() const
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

    template<uint8_t constant>
    void uuid_variant_constant<constant>::init_fields(std::array<uint8_t, 16>& octets)
    {
        octets.fill(constant);
    }

    template<typename rng_t>
    void uuid_variant_v7<rng_t>::init_fields(std::array<uint8_t, 16> &octets, rng_t random_generator)
    {

    }

    template<typename uuid_variant = uuid_variant_v4>
    template<typename rng_t>
    uuid<uuid_variant>::uuid(rng_t& random_generator)
    {
        uuid_variant::init_fields(octets, random_generator);
    }

    template<>
    inline uuid<uuid_variant_constant<0>>::uuid()
    {
        uuid_variant_constant<0>::init_fields(octets);
    }

    template<>
    inline uuid<uuid_variant_constant<0xFF>>::uuid()
    {
        uuid_variant_constant<0xFF>::init_fields(octets);
    }

    template<typename uuid_variant = uuid_variant_v4>
    inline uuid<uuid_variant>::uuid()
    {
        uuid_variant::init_fields(octets, *default_generator);
    }

    using guid      = uuid<uuid_variant_v4<xoroshiro128pp>>;
    using guid_v4   = uuid<uuid_variant_v4<xoroshiro128pp>>;
    using uuid_v4   = uuid<uuid_variant_v4<xoroshiro128pp>>;

    using guid_v7   = uuid<uuid_variant_v7<xoroshiro128pp>>;
    using uuid_v7   = uuid<uuid_variant_v7<xoroshiro128pp>>;

    static inline uuid<uuid_variant_constant<0>> const nil_uuid{};
    static inline uuid<uuid_variant_constant<0xFF>> const max_uuid{};
}

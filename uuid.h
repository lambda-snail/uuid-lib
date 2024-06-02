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

    /**
    * Currently only UUID version 4 is implemented. The standard also defines two special uuid's called 'nil' and 'max'.
    * They are implemented as a special type of variant.
    *
    * The UUID class holds the octet data, and different versions can be implemented using the strategy pattern by specifying
    * a variant class as a template type parameter. The variant classes are responsible for initializing the octets of the uuid,
    * and it is possible to extend the system to uuid versions that are not implemented if needed. Since there is no way to
    * check arbitrary code for standards compliance, this means that users of the library can effectively define any kind
    * of uuid that they want - this may correspond to version 8 in the standard.
    *
    * A variant definition must implement a function with the signature
    *
    * ```c++
    * static void init_fields(std::array<uint8_t, 16>&, rng_t)
    * ```
    *
    * For instance the definition for the uuid version 4 variant looks like this:
    *
    * ```c++
    * template<typename rng_t = xoroshiro128pp>
    * struct uuid_variant_v4
    * {
    *     static void init_fields(std::array<uint8_t, 16>& octets, rng_t random_generator);
    * };
    * ```
    *
    * where `rng_t` is a template type parameter that will also be passed to the `ctor` of the UUID. It can be used to specify
    * implementation specific random number generators if the default one that comes with the library is not sufficient for
    * whatever reason. The default random number generator is called `xoroshiro128pp`.
     *
     * @link https://datatracker.ietf.org/doc/html/rfc9562#name-requirements-language
     *
     * The random number generator has been adapted from:
     * @link https://xoroshiro.di.unimi.it/xoroshiro128plusplus.c
     */
    class uuid
    {
    public:
        explicit uuid(uint8_t constant);

        /**
         * Creates a version 4 UUID using the default random number generator.
         */
        template<typename uuid_spec>
        explicit inline uuid(uuid_spec const& spec) requires (not std::is_integral_v<uuid_spec>);

        /**
         * Creates a UUID using the provided sequence of bytes. This will initialize the UUID with a copy of the
         * byte array, and it is up to the caller to make sure that the resulting UUID is compliant with the standard.
         * This can be useful in certain scenarios where the UUID has already been created, possibly by an external source,
         * such as deserialization.
         */
        explicit uuid(std::array<uint8_t, 16> const& bytes);

        /**
         * Creates a UUID from a user-provided random number generator. Useful if you need random numbers from a particular
         * source or that fulfill certain criteria.
         */
        template<typename uuid_spec, typename rng_t = xoroshiro128pp>
        explicit uuid(uuid_spec const& spec, rng_t& random_generator);

        /**
         * Returns a string representation of the UUID.
         */
        [[nodiscard]] std::string as_string() const;

        static const uuid nil;
        static const uuid max;

    private:
        std::array<uint8_t, 16> octets {};
    };

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

    }

    template<typename uuid_spec, typename rng_t>
    uuid::uuid(uuid_spec const& spec, rng_t& random_generator)
    {
        spec.init_fields(octets, random_generator);
    }

    template<typename uuid_spec>
    inline uuid::uuid(uuid_spec const& spec) requires (not std::is_integral_v<uuid_spec>)
    {
        static_assert(not std::is_integral_v<uuid_spec>);
        spec.init_fields(octets, *default_generator);
    }
}

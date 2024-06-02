#pragma once

#include <array>
#include <cstdint>
#include <format>
#include <string>
#include <memory>
#include <string>
#include <uuid_types.h>

#include "xoroshiro128.h"

namespace LambdaSnail::Uuid
{
    /**
     * The default generator used by the UUID implementation.
     */
    static inline std::unique_ptr<xoroshiro128pp> g_default_generator = std::make_unique<xoroshiro128pp>();

    /**
     *  The UUID class really only holds octet data. Different versions of UUID are implemented using the strategy pattern,
     * by passing a version spec class to the constructor. The spec classes are responsible for initializing the octets
     * of the uuid, and it is possible to extend the system to UUID versions not covered by this implementation if needed.
     * Since there is no way to check arbitrary code for standards compliance, this means that users of the library can
     * effectively define any kind of UUID that they want - this may or may not correspond to version 8 in the standard,
     * depending on what you do.
     *
     * A version spec must implement a function with the signature
     *
     * ```c++
     * void init_fields(std::array<uint8_t, 16>& octets, rng_t random_generator) const;
     * ```
     *
     * The specs are not meant to have a state, so the init_fields function must be `const`. For instance the definition
     * for the uuid version 4 spec looks like this:
     *
     * ```c++
     * template<typename rng_t>
     * struct uuid_v4_spec
     * {
     *  void init_fields(octet_set_t& octets, rng_t random_generator) const;
     * };
     * ```
     *
     * where `rng_t` is the type of the random number generator that the spec expects. This will be passed to the spec instance
     * from the `uuid` `ctor`, and may be the default one for `uuid` or a user-defined class. It can be used to specify
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
        /**
         * Creates a UUID of the specified version using the default random number generator.
         */
        template<typename uuid_spec>
        explicit inline uuid(uuid_spec const& spec) requires (not std::is_integral_v<uuid_spec>);

        /**
         * Creates a UUID using the provided sequence of bytes. This will initialize the UUID with a copy of the
         * byte array, and it is up to the caller to make sure that the resulting UUID is compliant with the standard.
         * This can be useful in certain scenarios where the UUID has already been created, possibly by an external source,
         * such as deserialization.
         */
        explicit uuid(octet_set_t const& bytes);

        /**
         * Creates a UUID from a user-provided random number generator. Useful if you need random numbers from a particular
         * source or that fulfill certain criteria.
         */
        template<typename uuid_spec, typename rng_t = xoroshiro128pp>
        explicit uuid(uuid_spec const& spec, rng_t& random_generator);

        /**
         * Creates a UUID where all octets are filled with the same value.
         * @warning This can be used to create non-standard compliant UUIDs. The standard only defines the UUIDs
         * 'max' and 'nil'.
         */
        explicit uuid(uint8_t constant);

        /**
         * Returns a string representation of the UUID.
         */
        [[nodiscard]] std::string as_string() const;

        /**
         * The empty or nil UUID, with all octets set to 0.
         */
        static const uuid nil;

        /**
         * The max UUID, with all octets set to 0xFF.
         */
        static const uuid max;

    private:
        octet_set_t m_octets {};
    };

    template<typename uuid_spec, typename rng_t>
    uuid::uuid(uuid_spec const& spec, rng_t& random_generator)
    {
        spec.init_fields(m_octets, random_generator);
    }

    template<typename uuid_spec>
    inline uuid::uuid(uuid_spec const& spec) requires (not std::is_integral_v<uuid_spec>)
    {
        static_assert(not std::is_integral_v<uuid_spec>);
        spec.init_fields(m_octets, *g_default_generator);
    }
}

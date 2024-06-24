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
    static inline xoroshiro128pp g_default_generator;

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

        /**
         * Returns a string representation of the UUID.
         */
        [[nodiscard]] std::string as_string() const;

        /**
         * Some systems generate or expect UUIDs that are enclosed in braces. This function tries to
         * facilitate interop with such systems.
         *
         * Returns a string with the same format as as_string, enclosed in braces: "{uuid}"
         */
        [[nodiscard]] std::string as_string_braced() const;

        /**
         * The empty or nil UUID, with all octets set to 0.
         */
        static const uuid nil;

        /**
         * The max UUID, with all octets set to 0xFF.
         */
        static const uuid max;

        octet_set_t m_octets {};
    };
}

#pragma once

#include <array>
#include <cstdint>
#include <format>
#include <string>
#include <string>

namespace LambdaSnail::Uuid
{
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

        alignas(16)
        octet_set_t octets{};
    };
}

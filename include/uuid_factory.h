#pragma once

#include <cstdint>
#include <vector>
#include <chrono>

#include "uuid.h"

namespace LambdaSnail::Uuid
{
    template<typename rng_t>
    struct uuid_factory
    {
        /**
         * Creates a single version four uuid based on the provided random number generator.
         * @tparam rng_t The type of the random number generator. This needs to provide the member function `next()`.
         * @param out_id The output uuid.
         */
        void create_uuid_v4(uuid& out_id) noexcept;

        /**
         * Creates a single version seven uuid based on the provided random number generator.
         * @tparam rng_t The type of the random number generator. This needs to provide the member function `next()`.
         * @param out_id The output uuid.
         */
        void create_uuid_v7(uuid& out_id) noexcept;

        /**
         * Creates a number of uuids using a fixed bit-length dedicated counter. Using this method, up to
         * 4096 UUIDs can be created with the same millisecond timestamp. This function is limited in that
         * it should only be called once per millisecond.
         *
         * Note that no checks are performed on the input - it is up to the user to not request more than 4096 uuids. If
         * you need a million uuids in one batch, see create_uuids_monotonic_random.
         *
         * @tparam rng_t The type of the random generator to be used. Defaults to xoroshiro128pp that comes bundled with the library.
         * @param num_uuids The number of uuids to create.
         * @param out_vec The vector to place the generated uuids in.
         */
        void create_uuids_dedicated_counter(uint16_t num_uuids, std::vector<uuid>& out_vec) noexcept;

        /**
         * Creates a number of uuids using a monotonic random counter. This is the second way specified in the standard for generating
         * monotonically increasing UUIDs. It works by utilizing the 64 bit random data in octets 8-15 as a counter. This allows the creation
         * of a significant number of uuids in one batch, for which 2^32 has been chosen as the limit.
         *
         * - First, one timestamp is generated that will eb used for all uuids in the batch.
         * - Then the random bits for octets 6-7 are generated, along with the base random number for octets 8-15.
         * - Finally the uuids are generated sequentially, incrementing the counter in octets 8-15 after each iteration.
         *
         * Note that this way of creating UUIDs should not be used when security or the ability to guess the next id is a concern.
         *
         * @tparam rng_t The type of the random generator to be used. Defaults to xoroshiro128pp that comes bundled with the library.
         * @param num_uuids The number of uuids to create.
         * @param increment The increment between each uuid. Each new uuid will be larger than the previous by this amount.
         * @param out_vec The vector to place the generated uuids in.
         */
        void create_uuids_monotonic_random(uint32_t num_uuids, uint32_t increment, std::vector<uuid>& out_vec) noexcept;

    private:
        rng_t generator;

        static inline constexpr size_t s_version_octet = 6;
        static inline constexpr size_t s_variant_octet = 8;

        void v4_set_version_bits(uuid::octet_set_t &octets) const noexcept;
        void v4_set_variant_bits(uuid::octet_set_t &octets) const noexcept;

        void v7_set_ts_ms(uuid::octet_set_t &octets, uint64_t raw_ts) const noexcept;
        void v7_set_rand_a(uuid::octet_set_t &octets, uint16_t value) const noexcept;
        void v7_set_rand_b(uuid::octet_set_t &octets, uint64_t value) const noexcept;
    };

    // v4

    template <typename rng_t>
    void uuid_factory<rng_t>::create_uuid_v4(uuid& out_id) noexcept
    {
        uint64_t const n1 = generator();
        uint64_t const n2 = generator();

        // We don't care about endianness here, since a random integer should still be a random integer even
        // after shuffling the bytes around
        memcpy(out_id.octets.data(), &n1, sizeof(uint64_t));
        memcpy(out_id.octets.data() + out_id.octets.size()/2, &n2, sizeof(uint64_t));

        out_id.octets[s_version_octet] &= 0b01001111; // Most significant bits set to 0100
        out_id.octets[s_variant_octet] &= 0b10111111; // Most significant bits set to 10

        v4_set_version_bits(out_id.octets);
        v4_set_variant_bits(out_id.octets);
    }

    template <typename rng_t>
    void uuid_factory<rng_t>::v4_set_version_bits(uuid::octet_set_t& octets) const noexcept
    {
        // Most significant bits in version octet set to 0100
        uint8_t constexpr b1 = ~(static_cast<uint8_t>(1) << 7);
        uint8_t constexpr b2 = (static_cast<uint8_t>(1) << 6);
        uint8_t constexpr b3 = ~(static_cast<uint8_t>(1) << 5);
        uint8_t constexpr b4 = ~(static_cast<uint8_t>(1) << 4);

        octets[s_version_octet] &= b1;
        octets[s_version_octet] |= b2;
        octets[s_version_octet] &= b3;
        octets[s_version_octet] &= b4;
    }

    template <typename rng_t>
    void uuid_factory<rng_t>::v4_set_variant_bits(uuid::octet_set_t& octets) const noexcept
    {
        // Most significant bits in variant octet set to 10
        auto constexpr b1 = (static_cast<uint8_t>(1) << 7);
        auto constexpr b2 = ~(static_cast<uint8_t>(1) << 6);

        octets[s_variant_octet] |= b1;
        octets[s_variant_octet] &= b2;
    }

    // v7

    template <typename rng_t>
    void uuid_factory<rng_t>::create_uuid_v7(uuid& out_id) noexcept
    {
        // Set timestampt bits
        std::chrono::time_point<std::chrono::system_clock> const now = std::chrono::system_clock::now();
        int64_t const time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

        v7_set_ts_ms(out_id.octets, time_stamp);

        // Set random bits
        uint64_t const rand_a = generator();
        uint64_t const rand_b = generator();

        v7_set_rand_a(out_id.octets, rand_a);
        v7_set_rand_b(out_id.octets, rand_b);
    }

    template <typename rng_t>
    void uuid_factory<rng_t>::create_uuids_dedicated_counter(uint16_t num_uuids, std::vector<uuid>& out_vec) noexcept
    {
        out_vec.reserve(num_uuids);

        // One time stamp for all the uuids
        std::chrono::time_point<std::chrono::system_clock> const now = std::chrono::system_clock::now();
        int64_t const time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

        for(int i = 0; i < num_uuids; ++i)
        {
            uuid& uuid = out_vec.emplace_back(0);
            v7_set_ts_ms(uuid.octets, time_stamp);
            v7_set_rand_a(uuid.octets, i);

            uint64_t rand_b = generator();
            v7_set_rand_b(out_vec[i].octets, rand_b);
        }
    }

    template <typename rng_t>
    void uuid_factory<rng_t>::create_uuids_monotonic_random(uint32_t num_uuids, uint32_t increment, std::vector<uuid>& out_vec) noexcept
    {
        out_vec.reserve(num_uuids);

        // One time stamp for all the uuids
        std::chrono::time_point<std::chrono::system_clock> const now = std::chrono::system_clock::now();
        int64_t const time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

        // Attempt to create a random 16 bit number by adding the 'four' we get from next()
        uint64_t const rand_a_base = generator();
        uint64_t rand_a = rand_a_base + (rand_a_base >> 16) + (rand_a_base >> 32) + (rand_a_base >> 48);

        uint64_t rand_b_base = generator();

        // Subtract the increment_end from base to guarantee that we can increment as much as we want
        uint64_t const increment_end = static_cast<uint64_t>(num_uuids) * static_cast<uint64_t>(increment);
        rand_b_base = rand_b_base > increment_end ? rand_b_base - increment_end : rand_b_base;

        for(int i = 0; i < num_uuids; ++i)
        {
            uuid& uuid = out_vec.emplace_back(0);
            v7_set_ts_ms(uuid.octets, time_stamp);
            v7_set_rand_a(uuid.octets, rand_a);

            v7_set_rand_b(out_vec[i].octets, rand_b_base);
            rand_b_base += increment;
        }
    }

    template <typename rng_t>
    void uuid_factory<rng_t>::v7_set_ts_ms(uuid::octet_set_t& octets, uint64_t raw_ts) const noexcept
    {
        memcpy(octets.data(), &raw_ts, sizeof(int64_t));
        if constexpr (std::endian::native == std::endian::little)
        {
            std::swap(octets[0], octets[5]);
            std::swap(octets[1], octets[4]);
            std::swap(octets[2], octets[3]);
        }
    }

    template <typename rng_t>
    void uuid_factory<rng_t>::v7_set_rand_a(uuid::octet_set_t& octets, uint16_t value) const noexcept
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

        memcpy(octets.data() + s_version_octet, &value, sizeof(uint16_t));

        // If we are on little endian the most significant byte is in the 'wrong' place, so swap
        if constexpr (std::endian::native == std::endian::little)
        {
            std::swap(octets[s_version_octet], octets[s_version_octet+1]);
        }
    }

    template <typename rng_t>
    void uuid_factory<rng_t>::v7_set_rand_b(uuid::octet_set_t& octets, uint64_t value) const noexcept
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

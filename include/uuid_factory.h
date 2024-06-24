#pragma once

#include <cstdint>
#include <vector>
#include <chrono>

#include "xoroshiro128.h"
#include "uuid.h"
#include "uuid_spec.h"

namespace LambdaSnail::Uuid
{
    namespace factory
    {
        /**
         * Creates a single version four uuid based on the provided random number generator.
         * @tparam rng_t The type of the random number generator. This needs to provide the member function `next()`.
         * @param id The output uuid.
         * @param random_generator The random number generator instance.
         */
        template<typename rng_t = xoroshiro128pp>
        void create_uuid_v4(uuid& id, rng_t random_generator)
        {
            spec::uuid_v4_spec<rng_t> spec;
            spec.init_fields(id.m_octets, random_generator);
        }

        /**
         * Creates a single version four uuid using the default random generator.
         */
        template<typename rng_t = xoroshiro128pp>
        void create_uuid_v4(uuid& id)
        {
            spec::uuid_v4_spec<rng_t> spec;
            spec.init_fields(id.m_octets, *g_default_generator);
        }

        /**
         * Creates a single version seven uuid based on the provided random number generator.
         * @tparam rng_t The type of the random number generator. This needs to provide the member function `next()`.
         * @param id The output uuid.
         * @param random_generator The random number generator instance.
         */
        template<typename rng_t = xoroshiro128pp>
        void create_uuid_v7(uuid& id, rng_t random_generator)
        {
            spec::uuid_v7_spec<rng_t> spec;
            spec.init_fields(id.m_octets, random_generator);
        }

        /**
         * Creates a single version seven uuid using the default random generator.
         */
        template<typename rng_t = xoroshiro128pp>
        void create_uuid_v7(uuid& id)
        {
            spec::uuid_v7_spec<rng_t> spec;
            spec.init_fields(id.m_octets, *g_default_generator);
        }

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
         * @param random_generator The instance of rng_t. Defaults to the global instance of the default generator bundled with the library.
         */
        template<typename rng_t = xoroshiro128pp>
        void create_uuids_dedicated_counter(uint16_t num_uuids, std::vector<LambdaSnail::Uuid::uuid>& out_vec, rng_t& random_generator = *LambdaSnail::Uuid::g_default_generator)
        {
            spec::uuid_v7_spec<rng_t> spec;
            out_vec.reserve(num_uuids);

            // One time stamp for all the uuids
            std::chrono::time_point<std::chrono::system_clock> const now = std::chrono::system_clock::now();
            int64_t const time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

            for(int i = 0; i < num_uuids; ++i)
            {
                uuid& uuid = out_vec.emplace_back(0);
                spec.set_ts_ms(uuid.m_octets, time_stamp);
                spec.set_rand_a(uuid.m_octets, i);

                uint64_t rand_b = random_generator.next();
                spec.set_rand_b(out_vec[i].m_octets, rand_b);
            }
        }

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
         * @param random_generator The instance of rng_t. Defaults to the global instance of the default generator bundled with the library.
         */
        template<typename rng_t = xoroshiro128pp>
        void create_uuids_monotonic_random(uint32_t num_uuids, uint32_t increment, std::vector<LambdaSnail::Uuid::uuid>& out_vec, rng_t& random_generator = *LambdaSnail::Uuid::g_default_generator)
        {
            spec::uuid_v7_spec<rng_t> spec;
            out_vec.reserve(num_uuids);

            // One time stamp for all the uuids
            std::chrono::time_point<std::chrono::system_clock> const now = std::chrono::system_clock::now();
            int64_t const time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

            // Attempt to create a random 16 bit number by adding the 'four' we get from next()
            uint64_t const rand_a_base = random_generator.next();
            uint64_t rand_a = rand_a_base + (rand_a_base >> 16) + (rand_a_base >> 32) + (rand_a_base >> 48);

            uint64_t rand_b_base = random_generator.next();

            // Subtract the increment_end from base to guarantee that we can increment as much as we want
            uint64_t const increment_end = static_cast<uint64_t>(num_uuids) * static_cast<uint64_t>(increment);
            rand_b_base = rand_b_base > increment_end ? rand_b_base - increment_end : rand_b_base;

            for(int i = 0; i < num_uuids; ++i)
            {
                uuid& uuid = out_vec.emplace_back(0);
                spec.set_ts_ms(uuid.m_octets, time_stamp);
                spec.set_rand_a(uuid.m_octets, rand_a);

                spec.set_rand_b(out_vec[i].m_octets, rand_b_base);
                rand_b_base += increment;
            }
        }
    }
}

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
         * Creates a number of uuids using fixed bit-length dedicated counter. Using this method, up to
         * 4096 UUIDs can be created with the same millisecond timestamp.
         */
        template<typename rng_t = xoroshiro128pp>
        void create_uuids_fbdc(uint16_t num_uuids, std::vector<LambdaSnail::Uuid::uuid>& out_vec, rng_t& random_generator = *LambdaSnail::Uuid::g_default_generator)
        {
            spec::uuid_v7_spec<rng_t> spec;
            out_vec.reserve(num_uuids);

            // One time stamp for all the uuids
            std::chrono::time_point<std::chrono::system_clock> const now = std::chrono::system_clock::now();
            int64_t const time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

            if(not random_generator.is_seeded())
            {
                random_generator.seed_state();
            }

            for(int i = 0; i < num_uuids; ++i)
            {
                uuid& uuid = out_vec.emplace_back(0);
                spec.set_ts_ms(uuid.m_octets, time_stamp);
                spec.set_rand_a(uuid.m_octets, i);

                uint64_t rand_b = random_generator.next();
                spec.set_rand_b(out_vec[i].m_octets, rand_b);
            }
        }
    }
}

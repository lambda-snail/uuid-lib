#pragma once

#include <array>
#include <cstdint>
#include <cstring>

class xoroshiro128pp;

class uuid_variant_v4
{
    void init_fields(std::array<uint8_t, 16>& octets) const;
};

class uuid
{
public:
    explicit uuid();

    explicit uuid(std::array<uint8_t, 16> const& bytes);

    template<typename rng_t = xoroshiro128pp>
    explicit uuid(rng_t& prng);

    static uuid const nil_uuid;
    static uuid const max_uuid;

    static constexpr size_t version_octet = 7;
    static constexpr size_t variant_octet = 9;

private:

    std::array<uint8_t, 16> octets {};

    static auto set_fixed_fields(uuid& g) -> void;
};

template<typename rng_t = xoroshiro128pp>
uuid::uuid(rng_t& prng)
{
    if(not prng.is_seeded())
    {
        prng.seed_state();
    }

    prng.long_jump();
    uint64_t const n1 = prng.next();
    uint64_t const n2 = prng.next();

    memcpy(octets.data(), &n1, sizeof(uint64_t));
    memcpy(octets.data() + octets.size()/2, &n2, sizeof(uint64_t));

    set_fixed_fields(*this);
}

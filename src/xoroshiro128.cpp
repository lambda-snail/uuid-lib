#include "xoroshiro128.h"

#include <chrono>

xoroshiro128pp::xoroshiro128pp()
{
    using namespace std::chrono;
    uint64_t const s1 = system_clock::now().time_since_epoch().count();
    uint64_t const s2 = system_clock::now().time_since_epoch().count();

    seed_state({ s1, s2 });
}

auto xoroshiro128pp::seed_state(std::array<uint64_t, 2> const &&state) -> void
{
    s[0] = state[0];
    s[1] = state[1];
}

auto xoroshiro128pp::next() -> uint64_t
{
    const uint64_t s0 = s[0];
    uint64_t s1 = s[1];
    const uint64_t result = rotl(s0 + s1, 17) + s0;

    s1 ^= s0;
    s[0] = rotl(s0, 49) ^ s1 ^ (s1 << 21); // a, b
    s[1] = rotl(s1, 28); // c

    return result;
}

auto xoroshiro128pp::jump() -> void
{
    static constexpr std::array<uint64_t, 2> jump = { 0x2bd7a6a6e99c2ddc, 0x0992ccaf6a6fca05 };

    uint64_t s0 = 0;
    uint64_t s1 = 0;
    for(int i = 0; i < jump.size(); i++)
    {
        for(int b = 0; b < 64; b++) {
            if (jump[i] & static_cast<uint64_t>(1) << b) {
                s0 ^= s[0];
                s1 ^= s[1];
            }
            next();
        }
    }

    s[0] = s0;
    s[1] = s1;
}

auto xoroshiro128pp::long_jump() -> void
{
    static constexpr std::array<uint64_t, 2> long_jump = { 0x360fd5f2cf8d5d99, 0x9c6e6877736c46e3 };

    uint64_t s0 = 0;
    uint64_t s1 = 0;
    for(int i = 0; i < long_jump.size(); i++)
    {
        for(int b = 0; b < 64; b++) {
            if (long_jump[i] & static_cast<uint64_t>(1) << b) {
                s0 ^= s[0];
                s1 ^= s[1];
            }
            next();
        }
    }

    s[0] = s0;
    s[1] = s1;
}

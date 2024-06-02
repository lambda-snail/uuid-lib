#pragma once

/*  Written in 2019 by David Blackman and Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

#include <array>
#include <cstdint>

/*
 * Retrieved from https://xoroshiro.di.unimi.it/xoroshiro128plusplus.c
 * Minor adaptions by Niclas Blomberg
 *
 * This is xoroshiro128++ 1.0, one of our all-purpose, rock-solid,
 * small-state generators. It is extremely (sub-ns) fast and it passes all
 * tests we are aware of, but its state space is large enough only for
 * mild parallelism.
 *
 * For generating just floating-point numbers, xoroshiro128+ is even
 * faster (but it has a very mild bias, see notes in the comments).
 *
 * The state must be seeded so that it is not everywhere zero. If you have
 * a 64-bit seed, we suggest to seed a splitmix64 generator and use its
 * output to fill s.
 */

class xoroshiro128pp
{
public:
	/*
	 * Seeds the state with the current system time stamp.
	 */
	void seed_state();
	void seed_state(std::array<uint64_t, 2> const&& state);

	[[nodiscard]]
	auto is_seeded() const -> bool;

	auto next() -> uint64_t;

	/*
	 * This is the jump function for the generator. It is equivalent
	 * to 2^64 calls to next(); it can be used to generate 2^64
	 * non-overlapping subsequences for parallel computations.
	 */
	auto jump() -> void;

	/*
	 * This is the long-jump function for the generator. It is equivalent to
	 * 2^96 calls to next(); it can be used to generate 2^32 starting points,
	 * from each of which jump() will generate 2^32 non-overlapping
	 * subsequences for parallel distributed computations.
	 */
	auto long_jump() -> void;

private:

	std::array<uint64_t, 2> s = {};

	[[nodiscard]]
	static inline auto rotl(const uint64_t x, int const k) -> uint64_t
	{
		return (x << k) | (x >> (64 - k));
	}
};


//
// static inline uint64_t rotl(const uint64_t x, int k) {
// 	return (x << k) | (x >> (64 - k));
// }
//
// static uint64_t s[2];
//
// static void inline seed_state(std::array<uint64_t, 2> const&& state)
// {
// 	s[0] = state[0];
// 	s[1] = state[1];
// }
//
// inline uint64_t next() {
// 	const uint64_t s0 = s[0];
// 	uint64_t s1 = s[1];
// 	const uint64_t result = rotl(s0 + s1, 17) + s0;
//
// 	s1 ^= s0;
// 	s[0] = rotl(s0, 49) ^ s1 ^ (s1 << 21); // a, b
// 	s[1] = rotl(s1, 28); // c
//
// 	return result;
// }
//
//
// /* This is the jump function for the generator. It is equivalent
//    to 2^64 calls to next(); it can be used to generate 2^64
//    non-overlapping subsequences for parallel computations. */
//
// inline void jump() {
// 	static constexpr uint64_t JUMP[] = { 0x2bd7a6a6e99c2ddc, 0x0992ccaf6a6fca05 };
//
// 	uint64_t s0 = 0;
// 	uint64_t s1 = 0;
// 	for(int i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
// 	{
// 		for(int b = 0; b < 64; b++) {
// 			if (JUMP[i] & static_cast<uint64_t>(1) << b) {
// 				s0 ^= s[0];
// 				s1 ^= s[1];
// 			}
// 			next();
// 		}
// 	}
//
// 	s[0] = s0;
// 	s[1] = s1;
// }
//
//
// /* This is the long-jump function for the generator. It is equivalent to
//    2^96 calls to next(); it can be used to generate 2^32 starting points,
//    from each of which jump() will generate 2^32 non-overlapping
//    subsequences for parallel distributed computations. */
//
// inline void long_jump() {
// 	static constexpr uint64_t LONG_JUMP[] = { 0x360fd5f2cf8d5d99, 0x9c6e6877736c46e3 };
//
// 	uint64_t s0 = 0;
// 	uint64_t s1 = 0;
// 	for(int i = 0; i < sizeof LONG_JUMP / sizeof *LONG_JUMP; i++)
// 	{
// 		for(int b = 0; b < 64; b++) {
// 			if (LONG_JUMP[i] & static_cast<uint64_t>(1) << b) {
// 				s0 ^= s[0];
// 				s1 ^= s[1];
// 			}
// 			next();
// 		}
// 	}
//
// 	s[0] = s0;
// 	s[1] = s1;
// }
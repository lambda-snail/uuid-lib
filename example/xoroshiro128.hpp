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
	xoroshiro128pp();
	xoroshiro128pp(xoroshiro128pp const& xoroshiro128_pp) = delete;

	void seed_state(std::array<uint64_t, 2> const&& state);

	uint64_t next();
	uint64_t operator()();

	/*
	 * This is the jump function for the generator. It is equivalent
	 * to 2^64 calls to next(); it can be used to generate 2^64
	 * non-overlapping subsequences for parallel computations.
	 */
	void jump();

	/*
	 * This is the long-jump function for the generator. It is equivalent to
	 * 2^96 calls to next(); it can be used to generate 2^32 starting points,
	 * from each of which jump() will generate 2^32 non-overlapping
	 * subsequences for parallel distributed computations.
	 */
	void long_jump();

private:

	std::array<uint64_t, 2> s = {};

	[[nodiscard]]
	static inline auto rotl(const uint64_t x, int const k) -> uint64_t
	{
		return (x << k) | (x >> (64 - k));
	}
};

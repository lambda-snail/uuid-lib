# uuid-generator

## Overview

Represents a subset of a universally unique identifier (UUID) as specified in [rfc9562](https://datatracker.ietf.org/doc/html/rfc9562#name-requirements-language).

This is mostly intended for educational purposes, as I see many people who seem to confuse UUIDs (or GUIDs) for a particular framework
with the general concept of a Uuid as specified in the standard.

If your favorite language or framework doesn't provide UUID with a particular property, you can always implement our own (or use a third part library)!

## Details

Currently, this library implements UUID version 4 and 7. Version 7 UUIDs do not implement the sub-millisecond monotonicity guarantees
that would be required for batch creation - I am currently thinking about this. The standard also defines two special UUIDs called
'nil' and 'max'. These are provided as static variables, accessible as e.g. `uuid::max`.

The UUID class really only holds octet data. Different versions of UUID are implemented using the strategy pattern,
by passing a version spec class to the constructor. The spec classes are responsible for initializing the octets
of the uuid, and it is possible to extend the system to UUID versions not covered by this implementation if needed.
Since there is no way to check arbitrary code for standards compliance, this means that users of the library can
effectively define any kind of UUID that they want - this may or may not correspond to version 8 in the standard,
depending on what you do.

A version spec must implement a function with the signature

```c++
void init_fields(std::array<uint8_t, 16>& octets, rng_t random_generator) const;
```

The specs are not meant to have a state, so the init_fields function must be `const`. For instance the definition
for the uuid version 4 spec looks like this:

```c++
template<typename rng_t>
struct uuid_v4_spec
{
    void init_fields(octet_set_t& octets, rng_t random_generator) const;
};
```

where `rng_t` is the type of the random number generator that the spec expects. This will be passed to the spec instance
from the `uuid` `ctor`, and may be the default one for `uuid` or a user-defined class. It can be used to specify
implementation specific random number generators if the default one that comes with the library is not sufficient for
whatever reason. The default random number generator is called `xoroshiro128pp`.

### Version 7 Details

This project uses c++20, which allows us to create standard compliant UUID v7 using `std::chrono:::system_clock`. 
From [cppreference](https://en.cppreference.com/w/cpp/chrono/system_clock):

```
system_clock measures Unix Time (i.e., time since 00:00:00 Coordinated Universal Time (UTC), Thursday, 1 January 1970, not counting leap seconds).
```

Prior to c++20 it was not required for `system_clock` to be based on Unit Time.

# Usage

To create a `uuid` we can call the ctor and specify a version spec. This library includes pre-defined specs for version 4 and 7
that can be plugged in:

```c++
auto id_v4 = uuid(spec::g_uuid_v4_spec);
auto id_v7 = uuid(spec::g_uuid_v7_spec);
```

There are also constants for the `nil` or _empty_ uuid, and the `max`:

```c++
auto max = uuid::max;
auto empty = uuid::nil;
```

If you have obtained the octets for a UUID from somewhere else, a `uuid` can also be created with the raw data:

```c++
octet_set_t raw_data = get_data();
auto raw_uuid = uuid(raw_data);
```

Instead of `octet_set_t` you can also use the actual type, `std::array<uint8_t, 16>`.

# benchmarks

Results of Google benchmark on my machine. Actual values will of course differ from machine to machine, but I think we can say that
creating a uuid v7 is slightly faster than a v4, and creating them in batch is much faster per uuid compared to
creating them individually.

---

Run on (16 X 3294 MHz CPU s)
CPU Caches:
L1 Data 32 KiB (x8)
L1 Instruction 32 KiB (x8)
L2 Unified 512 KiB (x8)
L3 Unified 16384 KiB (x1)


| Benchmark                              | Time     | CPU      | Iterations |
|----------------------------------------|----------|----------|------------|
| BM_create_uuid_v4/iterations:1000000   | 229 ns   | 234 ns   | 1000000    |
| BM_create_uuid_v7/iterations:1000000   | 64.8 ns  | 62.5 ns  | 1000000    |          
| BM_create_batch/256/iterations:100000  | 2070 ns  | 2188 ns  | 100000     |          
| BM_create_batch/1024/iterations:100000 | 8024 ns  | 7969 ns  | 100000     |          
| BM_create_batch/4096/iterations:100000 | 31854 ns | 31875 ns | 100000     |          

---

# TODO

- [ ] Add tests
- [ ] Remove the main file so that the code can be included as a library
- [ ] Add batch creation capabilities for UUID v7

## Links

The RFC that defines UUID (published May 2024).

- [rfc9562](https://datatracker.ietf.org/doc/html/rfc9562#name-requirements-language)

The random number generator in this project comes fom the following link. All credits go to the original author, I 
have simply adapted it to fit my style.
- [xoroshiro128plusplus](https://xoroshiro.di.unimi.it/xoroshiro128plusplus.c)
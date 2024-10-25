# uuid-generator

## Overview

Represents a subset of a universally unique identifier (UUID) as specified in the draft [rfc9562](https://datatracker.ietf.org/doc/html/rfc9562#name-requirements-language).

This is mostly intended for educational purposes, as I see many people who seem to confuse UUIDs (or GUIDs) for a particular framework
with the general concept of a UUID as specified in the standard - if your favorite language or framework doesn't provide UUID with a particular property, you can always implement our own (or use a third part library)!

Currently, this library implements UUID version 4 and 7, based on the latest draft. The version 7 UUIDs can be generated in batch using a dedicated counter 
of 12 bits (utilizing the `rand_a` section) or a monotonic counter (utilizing the least significant bits of the `rand_b` section). 

The standard also defines two special UUIDs called 'nil' and 'max'. These are provided as static variables, accessible as e.g. `uuid::max`.

## Use Case

This is mainly intended for demonstration purposes, however it could potentially be used in an application where there is a need
to create a large number of uuids that are unique to your application. Another case is if the UUIDs should not contain any references to MAC addresses, ethernet addresses 
or any other information that may or may not be used by other UUID implementations - this library only uses the time stamp from `std::chrono` and the built-in
random number generator (can also be overridden by the user).

Please note that UUIDs should never be used for "security", in the sense of relying on the inability of a potential attacker to 
guess the next id (see section 8 of the standard draft for a security related discussion).

There are no guarantees of global uniqueness for the UUIDs generated here. If such guarantees are important, please take a look at the following functions 
to see if they fit your needs better.

### Linux

- [uuid_generate](https://www.man7.org/linux/man-pages/man3/uuid_generate.3.html)

### Windows

- [CoCreateGuid](https://learn.microsoft.com/en-us/windows/win32/api/combaseapi/nf-combaseapi-cocreateguid)
- [UuidCreate](https://learn.microsoft.com/en-us/windows/win32/api/rpcdce/nf-rpcdce-uuidcreate)
- [UuidCreateSequential](https://learn.microsoft.com/en-us/windows/win32/api/rpcdce/nf-rpcdce-uuidcreatesequential)

## Details

The UUID class really only holds octet data. Different versions of UUID are constructed using the provided factory functions.
The "raw" octet data is exposed to the user, so it should be relatively straightforward to implement a new UUID version. Thus,
this library puts a lot of responsibility on the user, should (s)he wish to make changes to a uuid or create their own.

The library allows users to provide their own random number generator, but it also comes with an adapted version of a generator called `xoroshiro128pp`. 
This generator is seeded with the system time by default when the application starts. A user-provided generator must support the
a member function called `next()` that returns a `uint64_t`.

### Version 7 Details

This project uses c++20, which allows us to create standard compliant UUID v7 using `std::chrono:::system_clock`. 
From [cppreference](https://en.cppreference.com/w/cpp/chrono/system_clock):

```
system_clock measures Unix Time (i.e., time since 00:00:00 Coordinated Universal Time (UTC), Thursday, 1 January 1970, not counting leap seconds).
```

Prior to c++20 it was not required for `system_clock` to be based on Unix Time.

# Usage

To start, you can either include `uuid_all.hpp` which pulls in all header files, or include `uuid.hpp` only if you don't 
need the facilities declared in the other headers.

## Factory

To begin we first need to construct an instance of the ```uuid_factory```. This can be constructed with a random generator from the
standard library, or a custom implementation:

```c++
uuid_factory<std::mt19937_64> factory;
```

A custom implementation must be default-constructible and provide an overload for `operator()` that fetches the next random number,
which is assumed to be a 64-bit unsigned integer.

## Single UUID Creation

To create a `uuid` using the factory, we call one of the provided public member functions. The most basic ones create a one-off
uuid of either version 4 or 7:

```c++
uuid_factory<std::mt19937_64> factory;

uuid v4;
factory.create_uuid_v4(v4);
uuid v7;
factory.create_uuid_v7(v7);
```

The api enables re-use of a single variable if you need more than one `uuid` within the same scope, but don't wish to "pollute" the stack:

```c++
uuid_factory<std::mt19937_64> factory;

uuid id;
factory.create_uuid_v4(id);
...
factory.create_uuid_v7(id);
```

There are also constants for the `nil` or _empty_ uuid, and the `max`:

```c++
auto max = uuid::max;
auto empty = uuid::nil;
```

If you have obtained the octets for a UUID from somewhere else (e.g., deserialized from some stream), a `uuid` can also be created with the raw data:

```c++
uuid::octet_set_t raw_data = get_data();
auto raw_uuid = uuid(raw_data);
```

Instead of `octet_set_t` you can also use the actual type, `std::array<uint8_t, 16>`.

## Batch UUID Creation

If you need a small number of uuids (up to 4096), the dedicated counter function can be used:

```c++
std::vector<uuid> uuids;
factory.create_uuids_dedicated_counter(256, uuids);
```

This creates 256 uuids based on the same time stamp and random data, which makes it more efficient per `uuid` compared 
to creating 256 `uuid` objects using the constructor.

Care must be taken, however, as this function uses timestamps with millisecond precision, and even the maximum number of 4096 
may be created quicker than that (depending on the speed of your machine). Take a look at the provided benchmarks to get some 
concrete numbers for your system.

If you need more than 4096 `uuid`s then there is also the monotonic random method:

```c++
std::vector<uuid> uuids;
factory.create_uuids_monotonic_random(100000, 4, uuids);
```

This will fill the vector with 100,000 `uuid`s separated by an increment of 4. These will use the same time stamp, and only
incur two calls per batch (not per `uuid`) to the random generator's `next()` function. It is thus more efficient per `uuid`
than using the provided factory functions repeatedly 100,000 times. New `uuid`s within the batch are created by adding the increment 
to the previous `uuid`.

## Code Snippets

The following code snippets come from main.cpp in the examples folder.

```c++
// Instantiate a factory for uuids
uuid_factory<std::mt19937_64> factory;

// The factory can also be configured with a custom random generator - it needs to overload the function call operator
//uuid_factory<xoroshiro128pp> factory;

// Create a uuid v4 
uuid uuid1;
factory.create_uuid_v4(uuid1);

// uuid defines two constant UUIDs defined in the standard
uuid empty = uuid::nil;
uuid max   = uuid::max;

// You can also define your own constant uuid if you want to
uuid custom_constant = uuid(0x42);

// uuid objects can be printed as std::strings
std::cout << uuid1.as_string() << std::endl;
std::cout << empty.as_string() << std::endl;
std::cout << max.as_string() << std::endl;

// The uuid strings can also have different formats
std::cout << uuid1.as_string<string_conversion_mode::curly_braces>() << std::endl;

// And they can be std::format-ed
std::cout << std::format("formatted: {}", uuid1) << std::endl;   // Without braces
std::cout << std::format("formatted: {:#}", uuid1) << std::endl; // With braces
std::cout << std::format("formatted: {:u}", uuid1) << std::endl;   // Upper case

// Some basic facts about the uuid structure
static_assert(not std::is_trivially_constructible_v<uuid>);
static_assert(std::is_trivially_copyable_v<uuid>);
static_assert(std::is_trivially_move_constructible_v<uuid>);

// We can also create uuid v7
uuid v7;
factory.create_uuid_v7(v7);

std::cout << v7.as_string() << std::endl;

// However, if we create too many of the above uuids within a short period of time, we run into
// problems if they get essentially the same time stamp.

// To solve this problem, we can batch generate UUIDs with a fixed bit-length dedicated counter.
// Using this method, up to 4096 uuids can be generated from the same timestamp.

std::vector<uuid> uuids;
factory.create_uuids_dedicated_counter(256, uuids);

// Another way to solve the problem is to batch generate UUIDs with the monotonic random method.
// This method sacrfices some of the randomness to allow up to 2^32 sequential uuids to be created
// with the same time stamp.

uuids.clear();
factory.create_uuids_monotonic_random(10000, 4, uuids);

// UUIDs can also be compared

std::cout << "Equal:     " << (uuids[0] == uuids[1]) << std::endl;
std::cout << "Less than: " << (uuids[0] < uuids[1]) << std::endl;
```

# TODO

- [ ] Random increment for the monotonic counter factory function
- [ ] Add tests
- [x] Clean up the api
- [x] Remove the spec classes exposed to the user and switch to factory functions only for a more consistent API
- [x] Add benchmarks
- [x] Remove the main file so that the code can be included as a library
- [x] Add batch creation capabilities for UUID v7

## Links

The RFC that defines UUID (published May 2024).

- [rfc9562](https://datatracker.ietf.org/doc/html/rfc9562#name-requirements-language)

The random number generator in the examples project comes fom the following link. All credits go to the original author, I 
have simply adapted it to fit my style.
- [xoroshiro128plusplus](https://xoroshiro.di.unimi.it/xoroshiro128plusplus.c)
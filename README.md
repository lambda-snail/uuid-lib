# uuid-generator

## Overview

Represents a subset of a universally unique identifier (UUID) as specified in [rfc9562](https://datatracker.ietf.org/doc/html/rfc9562#name-requirements-language).

This is mostly intended for educational purposes, as I see many people who seem to confuse UUIDs (or GUIDs) for a particular framework
with the general concept of a Uuid as specified in the standard.

If your favorite language or framework doesn't provide UUID with a particular property, you can always implement our own (or use a third part library)!

## Details

Currently only UUID version 4 is implemented. The standard also defines two special uuid's called 'nil' and 'max'.
They are implemented as a special type of variant.

The UUID class holds the octet data, and different versions can be implemented using the strategy pattern by specifying
a variant class as a template type parameter. The variant classes are responsible for initializing the octets of the uuid,
and it is possible to extend the system to uuid versions that are not implemented if needed. Since there is no way to
check arbitrary code for standards compliance, this means that users of the library can effectively define any kind
of uuid that they want - this may correspond to version 8 in the standard.

A variant definition must implement a function with the signature

```c++
static void init_fields(std::array<uint8_t, 16>&, rng_t)
```

For instance the definition for the uuid version 4 variant looks like this:

```c++
template<typename rng_t = xoroshiro128pp>
struct uuid_variant_v4
{
    static void init_fields(std::array<uint8_t, 16>& octets, rng_t random_generator);
};
```

where `rng_t` is a template type parameter that will also be passed to the `ctor` of the UUID. It can be used to specify
implementation specific random number generators if the default one that comes with the library is not sufficient for
whatever reason. The default random number generator is called `xoroshiro128pp`.

## Links

The RFC that defines UUID (published May 2024).

- [rfc9562](https://datatracker.ietf.org/doc/html/rfc9562#name-requirements-language)

The random number generator in this project comes fom the following link. All credits go to the original author, I 
have simply adapted it to fit my style.
- [xoroshiro128plusplus](https://xoroshiro.di.unimi.it/xoroshiro128plusplus.c)
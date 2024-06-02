# uuid-generator

## Overview

Represents a subset of a universally unique identifier (UUID) as specified in [rfc9562](https://datatracker.ietf.org/doc/html/rfc9562#name-requirements-language).

This is mostly intended for educational purposes, as I see many people who seem to confuse UUIDs (or GUIDs) for a particular framework
with the general concept of a Uuid as specified in the standard.

If your favorite language or framework doesn't provide UUID with a particular property, you can always implement our own (or use a third part library)!

## Details

Currently only UUID version 4 is implemented. The standard also defines two special uuid's called 'nil' and 'max'.
 These are provided as static variables, accessible as e.g. `uuid::max`.

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
    void init_fields(std::array<uint8_t, 16>& octets, rng_t random_generator) const;
};
```

where `rng_t` is the type of the random number generator that the spec expects. This will be passed to the spec instance
from the `uuid` `ctor`, and may be the default one for `uuid` or a user-defined class. It can be used to specify
implementation specific random number generators if the default one that comes with the library is not sufficient for
whatever reason. The default random number generator is called `xoroshiro128pp`.

## Links

The RFC that defines UUID (published May 2024).

- [rfc9562](https://datatracker.ietf.org/doc/html/rfc9562#name-requirements-language)

The random number generator in this project comes fom the following link. All credits go to the original author, I 
have simply adapted it to fit my style.
- [xoroshiro128plusplus](https://xoroshiro.di.unimi.it/xoroshiro128plusplus.c)
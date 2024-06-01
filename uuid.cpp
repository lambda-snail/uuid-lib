#include "uuid.h"

#include "xoroshiro128.h"

static xoroshiro128pp default_prng = {};

uuid const uuid::nil_uuid = uuid({ 0 });
uuid const uuid::max_uuid = uuid({ 0xFF });

void uuid_variant_v4::init_fields(std::array<uint8_t, 16> &octets) const
{

}

uuid::uuid() : uuid(default_prng) { }

uuid::uuid(std::array<uint8_t, 16> const &bytes)
{
    octets = bytes;
    set_fixed_fields(*this);
}

auto uuid::set_fixed_fields(uuid &g) -> void
{
    g.octets[version_octet] &= 0b01001111;
    g.octets[variant_octet] &= 0b10111111;
}
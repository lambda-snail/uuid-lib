#include "uuid.h"

#include "xoroshiro128.h"

namespace LambdaSnail
{
    template<typename uuid_variant>
    uuid<uuid_variant>::uuid(std::array<uint8_t, 16> const &bytes)
    {
        octets = bytes;
        set_fixed_fields(*this);
    }
};
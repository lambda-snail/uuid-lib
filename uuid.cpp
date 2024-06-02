#include "uuid.h"

#include "xoroshiro128.h"

namespace LambdaSnail
{
    uuid::uuid(uint8_t constant)
    {
        octets.fill(constant);
    }

    uuid::uuid(std::array<uint8_t, 16> const &bytes)
    {
        octets = bytes;
    }

    uuid const uuid::nil = uuid(0);
    uuid const uuid::max = uuid(0xFF);

    std::string uuid::as_string() const
    {
        return std::format(
            "{:02x}{:02x}{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}",
            octets[0], octets[1], octets[2], octets[3],
            octets[4], octets[5],
            octets[6], octets[7],
            octets[8], octets[9],
            octets[10], octets[11], octets[12], octets[13], octets[14], octets[15]
        );
    }
};
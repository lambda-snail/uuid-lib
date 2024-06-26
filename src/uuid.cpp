#include "uuid.h"

#include "xoroshiro128.h"

namespace LambdaSnail::Uuid
{
    uuid const uuid::nil = uuid( 0x00 );
    uuid const uuid::max = uuid( 0xFF );

    uuid::uuid() : uuid( 0x00 ) {};

    uuid::uuid(uint8_t constant)
    {
        m_octets.fill(constant);
    }

    constexpr uuid::uuid(octet_set_t const &bytes)
    {
        m_octets = bytes;
    }

    std::string uuid::as_string() const
    {
        return std::format(
            "{:02x}{:02x}{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}",
            m_octets[0], m_octets[1], m_octets[2], m_octets[3],
            m_octets[4], m_octets[5],
            m_octets[6], m_octets[7],
            m_octets[8], m_octets[9],
            m_octets[10], m_octets[11], m_octets[12], m_octets[13], m_octets[14], m_octets[15]
        );
    }

    std::string uuid::as_string_braced() const
    {
        return std::format(
            "{{{:02x}{:02x}{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}}}",
            m_octets[0], m_octets[1], m_octets[2], m_octets[3],
            m_octets[4], m_octets[5],
            m_octets[6], m_octets[7],
            m_octets[8], m_octets[9],
            m_octets[10], m_octets[11], m_octets[12], m_octets[13], m_octets[14], m_octets[15]
        );
    }
};
#pragma once

#include <algorithm>
#include <format>
#include <iostream>
#include <sstream>

#include "uuid.h"

template<>
struct std::formatter<LambdaSnail::Uuid::uuid> {
    bool m_IsBraced = false;
    bool m_IsUpperCase = false;

    template<class ParseContext>
    constexpr typename ParseContext::iterator parse(ParseContext &ctx)
    {
        auto it = ctx.begin();
        if (it == ctx.end())
            return it;

        if (*it == '#')
        {
            m_IsBraced = true;
            ++it;
        }

        if (*it == 'u')
        {
            m_IsUpperCase = true;
            ++it;
        }

        if (it != ctx.end() && *it != '}')
            throw std::format_error("Invalid format for uuid.");

        return it;
    }

    template<class FmtContext>
    typename FmtContext::iterator format(LambdaSnail::Uuid::uuid const &id, FmtContext &ctx) const
    {
        std::ostringstream out;
        if (m_IsBraced)
        {
            out << '{';
        }

        ios::fmtflags const flags(out.flags());

        for (int i = 0; i < id.m_octets.size(); ++i)
        {
            if(m_IsUpperCase)
            {
                out << std::uppercase;
            }

            out << std::setw(2) << std::setfill('0') << std::hex << static_cast<int32_t>(id.m_octets[i]);

            if (i == 3 or i == 5 or i == 7 or i == 9)
            {
                out << '-';
            }
        }

        out.flags(flags);

        if (m_IsBraced)
        {
            out << '}';
        }

        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};

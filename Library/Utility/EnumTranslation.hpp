#pragma once

#include <typeinfo>

#include <Usagi/Runtime/ErrorHandling.hpp>

namespace usagi
{
class ExceptionEnumLookupFailure : public std::logic_error
{
public:
    using logic_error::logic_error;
};

template <typename SrcEnum, typename DestEnum>
struct EnumMapping;
// {
//     constexpr static auto MAPPING = {
//         std::pair { ..., ... },
//         std::pair { ..., ... },
//     };
// };

template <typename SrcEnum, typename DestEnum>
struct EnumTranslator
{
    DestEnum from(SrcEnum val) const
    {
        for(auto [fst, snd] : EnumMapping<SrcEnum, DestEnum>::MAPPING)
        {
            if(fst == val) return snd;
        }
        USAGI_THROW(ExceptionEnumLookupFailure(typeid(SrcEnum).name()));
    }
};

template <typename SrcEnum, typename DestEnum, typename CastEnum = DestEnum>
class EnumAcceptor
{
    DestEnum mValue;

public:
    EnumAcceptor(SrcEnum val)
        : EnumAcceptor { val }
    {
    }

    EnumAcceptor(std::initializer_list<SrcEnum> list)
    {
        DestEnum val = 0;
        for(auto &&src : list)
            val |= EnumTranslator<SrcEnum, DestEnum>().from(src);
        mValue = val;
    }

    operator CastEnum() const
    {
        return static_cast<CastEnum>(mValue);
    }
};
}

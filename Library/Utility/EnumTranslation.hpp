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

template <typename SrcEnum, typename DestEnum>
class EnumAcceptor
{
    SrcEnum mValue;

public:
    EnumAcceptor(SrcEnum val)
        : mValue(val)
    {
    }

    operator DestEnum() const
    {
        return EnumTranslator<SrcEnum, DestEnum>().from(mValue);
    }
};
}

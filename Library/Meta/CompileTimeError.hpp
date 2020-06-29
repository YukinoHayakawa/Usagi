#pragma once

namespace usagi
{
template <typename T, typename AdditionalInfo, T SucceedValue>
struct CompileTimeError
{
    struct ErrorCode
    {
        T error_code;
        AdditionalInfo info;

        constexpr ErrorCode(
            T error_code = SucceedValue,
            AdditionalInfo info = { })
            : error_code(error_code)
            , info(info)
        {
        }

        constexpr operator bool() const
        {
            return error_code == SucceedValue;
        }
    };

    template <T Code, AdditionalInfo Info>
    struct CheckErrorCode
    {
        constexpr static bool value = Code == SucceedValue;
        static_assert(value);
    };
};
}

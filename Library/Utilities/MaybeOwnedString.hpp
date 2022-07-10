#pragma once

#include <optional>
#include <string>

#include <Usagi/Concepts/Type/Types.hpp>

namespace usagi
{
/**
 * \brief Hold a string_view which may reference the internal storage only when
 * a string object is passed in when constructing. Otherwise it references
 * an external storage.
 */
class MaybeOwnedString
{
protected:
    std::optional<std::string> mStorage;
    std::string_view mView;

public:
    MaybeOwnedString() = default;

    // copy view
    MaybeOwnedString(std::string_view view);
    MaybeOwnedString(const char *str);
    MaybeOwnedString(const std::string &str);

    // transfer value
    MaybeOwnedString(std::string &&str);

    ~MaybeOwnedString() = default;

    // move & copy ops
    MaybeOwnedString(const MaybeOwnedString &other);
    MaybeOwnedString(MaybeOwnedString &&other) noexcept;
    MaybeOwnedString & operator=(const MaybeOwnedString &other);
    MaybeOwnedString & operator=(MaybeOwnedString &&other) noexcept;

    bool is_value_owner() const { return mStorage.has_value(); }

    std::string value() const { return std::string { mView }; }
    const std::string_view & view() const { return mView; }
};
}

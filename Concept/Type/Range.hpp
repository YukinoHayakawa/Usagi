#pragma once

namespace usagi::concepts
{
template <typename T>
concept Range = requires(T t)
{
    t.begin();
    t.end();
};

template <typename T>
concept SizedRange = requires(T t)
{
    requires Range<T>;
    { t.size() };
};
}

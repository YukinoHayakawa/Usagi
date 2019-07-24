#pragma once

#include <vector>
#include <cassert>
#include <algorithm>

namespace usagi
{
template <typename T>
class SortedArray : public std::vector<T>
{
public:
    using std::vector<T>::begin;
    using std::vector<T>::end;
    using std::vector<T>::empty;
    using std::vector<T>::back;
    using std::vector<T>::insert;

    auto insert(T &&element)
    {
        assert(!std::binary_search(
            begin(),
            end(),
            element
        ));

        if(empty() || back() < element)
        {
            return std::vector<T>::insert(end(), std::forward<T>(element));
        }

        auto pos = std::upper_bound(
            begin(),
            end(),
            element
        );
        return std::vector<T>::insert(pos, std::forward<T>(element));
    }

    template <typename Key>
    auto erase(Key &&key)
    {
        return std::vector<T>::erase(find(key));
    }

    template <typename Key>
    auto find(Key &&key)
    {
        auto range = std::equal_range(begin(), end(), key);
        return range.first;
    }
};
}

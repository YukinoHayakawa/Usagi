#pragma once

#include <cstddef>
#include <cassert>
#include <algorithm>

namespace usagi
{
// Resize the container with a generator instead of a fixed value.
template <typename SeqContainer, typename Generator>
void resize_generate(SeqContainer &container, std::size_t size, Generator gen)
{
    if(container.size() > size)
    {
        container.resize(size);
    }
    else
    {
        container.reserve(size);
        while(container.size() < size)
            container.emplace_back(gen());
    }
}

template <typename Container, typename Key, typename CreateFunc>
auto & find_or_emplace(Container &container, Key key, CreateFunc func)
{
    const auto find_it = container.lower_bound(key);
    if(find_it == container.end() || find_it->first != key)
    {
        const auto [emplace_it, inserted] = container.try_emplace(
            key, func()
        );
        assert(inserted);
        return emplace_it->second;
    }
    return find_it->second;
}

// Find a value in the container. Call the creation function for it if it is
// not found. Useful for implementing cache.
template <typename Container, typename Key, typename CreateFunc>
auto & find_or_create(Container &container, Key key, CreateFunc func)
{
    const auto find = container.find(key);
    if(find == container.end())
        return func(key);
    return find->second;
}

/*
// Find an element in left container and move it to the right container.
// order is not preserved. Useful for implementing cache.
template <typename SeqContainer, typename Predicate>
auto & find_migrate(SeqContainer &left, SeqContainer &right, Predicate pred)
{
    const auto find = std::ranges::find_if(left, pred);
    assert(find != left.end());
    std::swap(*find, left.back());
    right.push_back(std::move(left.back()));
    left.pop_back();
    return right.back();
}

// Move the last element of left to right.
template <typename SeqContainer>
auto & migrate_last(SeqContainer &left, SeqContainer &right)
{
    right.emplace_back(std::move(left.back()));
    left.pop_back();
    return right.back();
}
*/

// If the container is not empty, pop and return the last element. Otherwise,
// create a new one.
template <typename SeqContainer, typename CreateFunc>
auto pop_or_create(SeqContainer &container, CreateFunc func)
{
    if(container.empty())
        return func();
    auto ret = std::move(container.back());
    container.pop_back();
    return ret;
}
}

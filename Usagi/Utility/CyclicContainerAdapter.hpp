#pragma once

#include <utility>
#include <functional>

namespace usagi::utility
{
/**
* \brief Simulate a repeating sequence using an existing container.
* A limit can be put on the repeat amount, and the starting position
* does not have to be the beginning of the original container.
* In either case, when the original end is reached during using the
* adapted iterator, the next position wraps to the beginning of the
* original container as long as the max repeat cycle is not reached.
* todo: specialize std::iterator_traits<>
* \tparam Container
* \tparam BaseIterator
*/
// todo better performance
template <
    typename Container,
    typename BaseIterator = typename Container::iterator
>
class CyclicContainerAdapter
{
    Container &mContainer;
    BaseIterator mPseudoBegin;
    std::size_t mMaxCycle;

public:
    static constexpr std::size_t INDEFINITE = -1;

    explicit CyclicContainerAdapter(
        Container &container,
        BaseIterator pseudo_begin = container.begin(),
        const std::size_t max_cycle = INDEFINITE)
        : mContainer { container }
        , mPseudoBegin { std::move(pseudo_begin) }
        , mMaxCycle { max_cycle }
    {
    }

    /**
    * \brief Implements a BidirectionalIterator.
    */
    class Iterator
    {
        CyclicContainerAdapter &mAdapter;
        BaseIterator mBase;
        std::size_t mCurrentCycle = 0;

    public:
        Iterator(
            CyclicContainerAdapter &container,
            BaseIterator base,
            const std::size_t current_cycle)
            : mAdapter { container }
            , mBase { base }
            , mCurrentCycle { current_cycle }
        {
        }

        Iterator(const Iterator &other) = default;
        Iterator & operator=(const Iterator &other) = default;

        Iterator & operator=(Iterator &&other) noexcept = default;
        Iterator(Iterator &&other) noexcept = default;

        Iterator & operator++()
        {
            ++mBase;
            // wrap back
            if(mBase == mAdapter.mContainer.end())
            {
                mBase = mAdapter.mContainer.begin();
            }
            if(mBase == mAdapter.mPseudoBegin)
                ++mCurrentCycle;
            return *this;
        }

        Iterator operator++(int)
        {
            auto old_iter = *this;
            ++*this;
            return old_iter;
        }

        friend bool operator==(const Iterator &lhs, const Iterator &rhs)
        {
            if(&lhs.mAdapter != &rhs.mAdapter)
                USAGI_THROW(std::runtime_error("incompatible iterators"));
            return lhs.mBase == rhs.mBase
                && lhs.mCurrentCycle == rhs.mCurrentCycle;
        }

        friend bool operator!=(const Iterator &lhs, const Iterator &rhs)
        {
            return !(lhs == rhs);
        }

        auto && operator*() const
        {
            return mBase.operator*();
        }

        auto && operator->() const
        {
            return mBase.operator->();
        }

        Iterator & operator--()
        {
            if(mBase == mAdapter.mPseudoBegin)
                --mCurrentCycle;
            if(mBase == mAdapter.mContainer.begin())
            {
                mBase = mAdapter.mContainer.end();
            }
            --mBase;
            return *this;
        }

        Iterator operator--(int)
        {
            auto old_iter = *this;
            --*this;
            return old_iter;
        }

        BaseIterator base() const
        {
            return mBase;
        }
    };

    Iterator begin()
    {
        return { *this, mPseudoBegin, 0 };
    }

    Iterator end()
    {
        return { *this, mPseudoBegin, mMaxCycle };
    }
};
}

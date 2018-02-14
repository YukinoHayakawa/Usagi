#pragma once

#include <utility>
#include <functional>

namespace yuki::utility
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
template <
    typename Container,
    typename BaseIterator = typename Container::iterator
>
class CyclicContainerAdaptor
{
    Container &mContainer;
    BaseIterator mPseudoBegin;
    std::size_t mMaxCycle;

public:
    CyclicContainerAdaptor(Container &container, BaseIterator pseudo_begin,
        const std::size_t max_cycle)
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
        CyclicContainerAdaptor &mAdaptor;
        BaseIterator mBase;
        std::size_t mCurrentCycle = 0;
        // called when the underlying iterator is wrapped to begin or end.
        std::function<void(int)> mWrapCallback;

    public:
        Iterator()
        {
        }

        Iterator(CyclicContainerAdaptor &container, BaseIterator base,
            const std::size_t current_cycle)
            : mAdaptor { container }
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
            if(mBase == mAdaptor.mContainer.end())
            {
                mBase = mAdaptor.mContainer.begin();
                if(mWrapCallback) mWrapCallback(1);
            }
            if(mBase == mAdaptor.mPseudoBegin)
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
            return lhs.mBase == rhs.mBase
                && lhs.mCurrentCycle == rhs.mCurrentCycle;
        }

        friend bool operator!=(const Iterator &lhs, const Iterator &rhs)
        {
            return !(lhs == rhs);
        }

        auto & operator*() const
        {
            return mBase.operator*();
        }

        auto & operator->() const
        {
            return mBase.operator->();
        }

        Iterator & operator--()
        {
            if(mBase == mAdaptor.mPseudoBegin)
                --mCurrentCycle;
            if(mBase == mAdaptor.mContainer.begin())
            {
                mBase = mAdaptor.mContainer.end();
                if(mWrapCallback) mWrapCallback(-1);
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

        void setWrapCallback(std::function<void(int)> wrap_callback)
        {
            mWrapCallback = std::move(wrap_callback);
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

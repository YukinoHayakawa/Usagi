#pragma once

#include <vector>
#include <utility>

#include <Usagi/Library/Meta/Op.hpp>

namespace usagi
{
template <typename T>
struct HeapElementTraitRef
{
    static void set_elem_index(T &t, std::size_t idx)
    {
        t.elem_index = idx;
    }
};

template <typename T>
struct HeapElementTraitPtr
{
    static void set_elem_index(T *t, std::size_t idx)
    {
        t->elem_index = idx;
    }
};

struct HeapElementTraitNoop
{
    template <typename...Args>
    static void set_elem_index(Args &&...)
    {
        /* no-op */
    }
};

/**
 * \brief A binary heap holding pointers to the real data. Comparision is
 * performed by dereferencing the pointers by default.
 * todo unit test
 * \tparam T Supposed to be a pointer to external structure.
 * \tparam Compare Use < for min heap, > for max heap.
 */
// todo queue_index access trait
// todo interface needs cleaning up
template <
    typename T,
    typename Projection = std::identity,
    typename Compare = OpDeref<std::less<std::remove_pointer_t<T>>>,
    typename Traits = HeapElementTraitPtr<std::remove_pointer_t<T>>,
    typename HeapArray = std::vector<T>
>
class BinaryHeap
{
    HeapArray mHeap;
    Projection mProj;
    Compare mCmp;
    Traits mTraits;

    static constexpr std::size_t parent(const std::size_t node_idx)
    {
        return (node_idx - 1) / 2;
    }

    static constexpr std::size_t left_child(const std::size_t node_idx)
    {
        return 2 * node_idx + 1;
    }

    static constexpr std::size_t right_child(const std::size_t node_idx)
    {
        return 2 * node_idx + 2;
    }

    void swap(const std::size_t a, const std::size_t b)
    {
        std::swap(mHeap[a], mHeap[b]);
        // maintain correct references from elements to heap
        mTraits.set_elem_index(mHeap[a], a);
        mTraits.set_elem_index(mHeap[b], b);
    }

    void bubble(const std::size_t index)
    {
        assert(index < mHeap.size());
        for(std::size_t idx = index, parent_idx = parent(idx);
            idx > 0 /* has a parent */;
            idx = parent_idx, parent_idx = parent(idx))
        {
            if(mCmp(mProj(mHeap[idx]), mProj(mHeap[parent_idx])))
                swap(idx, parent_idx); // bubble up
        }
    }

    void sink(const std::size_t index)
    {
        assert(index < mHeap.size());
        std::size_t cur = index, min_child_;
        while((min_child_ = min_child(cur)) != INVALID_INDEX)
        {
            if(mCmp(mProj(mHeap[cur]), mProj(mHeap[min_child_])))
                break;
            swap(cur, min_child_);
            cur = min_child_;
        }
    }

    std::size_t min_child(const std::size_t index)
    {
        assert(index < mHeap.size());
        std::size_t left = left_child(index), right = right_child(index);
        if(left >= size())
            if(right >= size())
                return INVALID_INDEX; // index is a leaf node
            else
                return right; // only have right node
        else
            if(right >= size())
                return left; // only have left node
            else
                return mCmp(mProj(mHeap[left]), mProj(mHeap[right]))
                    ? left : right;
    }

public:
    explicit BinaryHeap(
        Projection proj = { },
        Compare compare = { },
        Traits traits = { })
        : mProj(std::move(proj))
        , mCmp(std::move(compare))
        , mTraits(std::move(traits))
    {
    }

    constexpr static std::size_t INVALID_INDEX = -1;

    void reserve(const std::size_t capacity)
    {
        mHeap.reserve(capacity);
    }

    bool empty() const
    {
        return mHeap.empty();
    }

    std::size_t size() const
    {
        return mHeap.size();
    }

    void clear()
    {
        mHeap.clear();
    }

    void insert(T element)
    {
        std::size_t new_elem_pos = mHeap.size();
        mHeap.push_back(std::move(element));
        mTraits.set_elem_index(mHeap.back(), new_elem_pos);
        bubble(new_elem_pos);
    }

    // If the element is modified, modify_key() must be called to ensure
    // heap correctness.
    T & top()
    {
        assert(!mHeap.empty());
        return mHeap[0];
    }

    T pop()
    {
        assert(!mHeap.empty());

        // prepare the element to be returned
        auto ret = std::move(top());
        mTraits.set_elem_index(ret, INVALID_INDEX);

        // we have more than one elements before pop
        if(size() > 1)
        {
            // move last to first
            mHeap.front() = std::move(mHeap.back());
            mTraits.set_elem_index(mHeap.front(), 0);
            sink(0);
        }
        mHeap.pop_back();

        return std::move(ret);
    }

    void modify_key(const std::size_t index)
    {
        // recover proper order. if the element is smaller than its parent
        // then bubble up. otherwise sink down.
        if(index > 0 && mCmp(mProj(mHeap[index]), mProj(mHeap[parent(index)])))
            bubble(index);
        else
            sink(index);
    }
};
}

#pragma once

#include <atomic>
#include <cassert>
#include <memory>
#include <optional>

#include <Usagi/Library/Memory/Noncopyable.hpp>

namespace usagi
{
// Help tracking usage of objects.
class RefCounter : Noncopyable
{
    std::atomic<std::int32_t> mUses = 0;

    void increment()
    {
        assert(mUses >= 0);
        ++mUses;
    }

    void decrement()
    {
        assert(mUses > 0);
        --mUses;
    }

    template <typename T>
    friend class RefCounted;

public:
    RefCounter() = default;

    ~RefCounter()
    {
        (void)0; // Suppress trivial dtor warning.
        // Outstanding references must have been destroyed.
        assert(use_count() == 0);
    }

    // The object should not be moved when there is any outstanding reference
    // to it. Because the Reference objects don't suppose the address of
    // the counter to change.
    RefCounter(RefCounter &&other) noexcept
    {
        assert(other.mUses == 0);
    }

    // It's impossible to redirect outstanding references. Two objects must
    // all have 0 refs.
    RefCounter & operator=(RefCounter &&other) noexcept
    {
        if(this == &other)
            return *this;
        assert(mUses == 0);
        assert(other.mUses == 0);
        return *this;
    }

    std::int32_t use_count() const
    {
        return mUses.load();
    }
};

// T could be trivial objects or pointers.
template <typename T>
class RefCounted
{
    template <typename T2>
    friend class RefCounted;

    RefCounter *mCounter = nullptr;
    std::optional<T> mObject;

    void increment() const
    {
        if(mCounter)
            mCounter->increment();
    }

    void decrement() const
    {
        if(mCounter)
            mCounter->decrement();
    }

    void reset()
    {
        mCounter = nullptr;
        mObject.reset();
    }

    template <typename T2>	
    void copy_construct_from(const RefCounted<T2> &other)
    {
        other.increment();

        mCounter = other.mCounter;
        mObject = other.mObject;
    }

    template <typename T2>	
    void move_construct_from(RefCounted<T2> &&other)
    {
        // Object moved so the two ref count changes balanced out. No need
        // to increment/decrement.

        mCounter = other.mCounter;
        mObject = std::move(other.mObject);

        other.reset();
    }

    constexpr static bool IsPointer = std::is_pointer_v<T>;
    constexpr static bool IsObject = 
        std::is_object_v<T> &&
        !std::is_pointer_v<T> &&
        !std::is_array_v<T>
    ;

    static_assert(IsPointer || IsObject);

    using ValueT = std::conditional_t<
        IsPointer,
        std::remove_pointer_t<T>,
        T
    >;
    using ReferenceT = std::add_lvalue_reference_t<ValueT>;
    using PointerT = std::add_pointer_t<ValueT>;

    PointerT pointer() const requires IsPointer
    {
        return mObject.value();
    }

public:
    RefCounted() = default;

    // Constructors

    // Applicable for both objects / pointers
    explicit RefCounted(RefCounter *counter, std::optional<T> object = { })
        : mCounter(counter)
        , mObject(std::move(object))
    {
        increment();
    }

    // Applicable for pointers
    explicit RefCounted(RefCounter *counter, ReferenceT object)
        requires IsPointer
        : mCounter(counter)
        , mObject(&object)
    {
        increment();
    }

    RefCounted(const RefCounted &other) noexcept
    {
        this->copy_construct_from(other);
    }

    RefCounted(RefCounted &&other) noexcept
    {
        this->move_construct_from(std::move(other));
    }

    // Converting constructors
    // Note that these constructors have lower priority during overload
    // resolution. So the above two are still necessary. See
    // https://stackoverflow.com/questions/32537994/c-template-copy-constructor-on-template-class

    template <typename T2 = T>	
    RefCounted(const RefCounted<T2> &other) noexcept
        requires std::is_convertible_v<T2, T>
    {
        this->copy_construct_from(other);
    }

    template <typename T2 = T>	
    RefCounted(RefCounted<T2> &&other) noexcept
        requires std::is_convertible_v<T2, T>
    {
        this->move_construct_from(std::move(other));
    }

    // Destructor

    ~RefCounted()
    {
        decrement();
        reset();
    }

    // Assignment ops

    RefCounted & operator=(const RefCounted &other)
    {
        // Transfer the content of current object to a temporary one.
        // The ctor & dtor of the temporary object will handle the ref count.
        RefCounted(other).swap(*this);
        return *this;
    }

    RefCounted & operator=(RefCounted &&other) noexcept
    {
        RefCounted(std::move(other)).swap(*this);
        return *this;
    }

    // Converting assignment ops

    template <typename T2 = T>	
    RefCounted & operator=(const RefCounted<T2> &other)
        requires std::is_convertible_v<T2, T>
    {
        // Transfer the content of current object to a temporary one.
        // The ctor & dtor of the temporary object will handle the ref count.
        RefCounted<T>(other).swap(*this);
        return *this;
    }

    template <typename T2 = T>	
    RefCounted & operator=(RefCounted<T2> &&other) noexcept
        requires std::is_convertible_v<T2, T>
    {
        RefCounted<T>(std::move(other)).swap(*this);
        return *this;
    }

    // Member access

    PointerT operator->() const
    {
        return pointer();
    }

    ReferenceT operator*() const
    {
        return *pointer();
    }

    T value() const requires IsObject
    {
        return mObject.value();
    }

    T get() const requires IsPointer
    {
        return mObject.value();
    }

    std::add_const_t<T> cref() const
    {
        if constexpr(IsPointer)
            return *pointer();
        else
            return value();
    }

    // Swap

    void swap(RefCounted &other) noexcept
    {
        std::swap(mCounter, other.mCounter);
        std::swap(mObject, other.mObject);
    }

    // 

    bool bound() const
    {
        return mCounter;
    }

    bool has_object() const
    {
        return mObject.has_value();
    }
};
}

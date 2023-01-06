#pragma once

#include <atomic>
#include <cassert>
#include <memory>
#include <optional>
#include <variant>

#include <Usagi/Library/Memory/Noncopyable.hpp>
#include <Usagi/Library/Meta/TypeContainers/TypeTag.hpp>

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
    friend class RefCountedBase;

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

template <typename T>
struct PointerWrapper
{
    T * val = nullptr;

    PointerWrapper() = default;

    explicit PointerWrapper(T *val)
        : val(val)
    {
    }

    void emplace(T *val2) { val = val2; }
    void reset() { val = nullptr; }
    T * value() const { return val; }
    bool has_value() const { return val; }

    using value_type = T;
};

// Store the ref-counted value inside the RefCounted tracker. Note that it is
// possible that two trackers referencing the same counter can have different
// values.
template <typename T>
struct InPlace : WrappedValue<T>
{
};

template <typename WrappedT>
class RefCountedBase
{
    template <typename T2>
    friend class RefCountedBase;

protected:
    // User of the class is responsible for ensuring that the counter outlives
    // all references to it.
    RefCounter *mCounter = nullptr;

    // std::optional or PointerWrapper
    WrappedT mObject;

    using InnerT = typename WrappedT::value_type;

    // Reference counting behaviors are not affected by the object status.

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
    void copy_construct_from(const RefCountedBase<T2> &other)
    {
        other.increment();

        mCounter = other.mCounter;
        mObject = other.mObject;
    }

    template <typename T2>	
    void move_construct_from(RefCountedBase<T2> &&other)
    {
        // Object moved so the two ref count changes balanced out. No need
        // to increment/decrement.

        mCounter = other.mCounter;
        mObject = std::move(other.mObject);

        other.reset();
    }

    // Constructors

    RefCountedBase() = default;

    RefCountedBase(RefCounter *counter, WrappedT object)
        : mCounter(counter)
        , mObject(std::move(object))
    {
        increment();
    }

    RefCountedBase(const RefCountedBase &other)
    {
        copy_construct_from(other);
    }

    RefCountedBase(RefCountedBase &&other) noexcept
    {
        move_construct_from(std::move(other));
    }

    // Destructor

    ~RefCountedBase()
    {
        decrement();
        reset();
    }

    auto & value_ref() const
    {
        return mObject.value();
    }

    auto value() const
    {
        return mObject.value();
    }

public:
    // Assignment ops

    RefCountedBase & operator=(const RefCountedBase &other)
    {
        // Transfer the content of current object to a temporary one.
        // The ctor & dtor of the temporary object will handle the ref count.
        RefCountedBase(other).swap(*this);
        return *this;
    }

    RefCountedBase & operator=(RefCountedBase &&other) noexcept
    {
        RefCountedBase(std::move(other)).swap(*this);
        return *this;
    }

    // Converting assignment ops

    template <typename T2 = InnerT>	
    RefCountedBase & operator=(const RefCountedBase<T2> &other)
        requires std::is_convertible_v<typename T2::value_type, InnerT>
    {
        // Transfer the content of current object to a temporary one.
        // The ctor & dtor of the temporary object will handle the ref count.
        RefCountedBase<WrappedT>(other).swap(*this);
        return *this;
    }

    template <typename T2 = InnerT>	
    RefCountedBase & operator=(RefCountedBase<T2> &&other) noexcept
        requires std::is_convertible_v<typename T2::value_type, InnerT>
    {
        RefCountedBase<WrappedT>(std::move(other)).swap(*this);
        return *this;
    }

    void swap(RefCountedBase &other) noexcept
    {
        std::swap(mCounter, other.mCounter);
        std::swap(mObject, other.mObject);
    }

    bool has_value() const
    {
        return mObject.has_value();
    }
};

// Synthesis a ref tracing pointer from reference counter and the actual object.
// The managed object may not be available upon the construction of this
// tracker, and can be materialized later by assigning from another instance.
template <typename T>
    requires (!std::is_array_v<T>) // Arrays not supported yet.
class RefCounted : public RefCountedBase<PointerWrapper<T>>
{
    using BaseT = RefCountedBase<PointerWrapper<T>>;

public:
    RefCounted() = default;

    // Constructors

    explicit RefCounted(RefCounter *counter, T *ptr = nullptr)
        : BaseT(counter, PointerWrapper<T>(ptr))
    {
    }

    RefCounted(const RefCounted &other) noexcept
        : BaseT(other)
    {
    }

    RefCounted(RefCounted &&other) noexcept
        : BaseT(other)
    {
    }

    // Converting constructors
    // Note that these constructors have lower priority during overload
    // resolution. So the above two are still necessary. See
    // https://stackoverflow.com/questions/32537994/c-template-copy-constructor-on-template-class

    template <typename T2 = T>	
    RefCounted(const RefCounted<T2> &other) noexcept
        requires std::is_convertible_v<T2, T>
        : BaseT(other)
    {
    }

    template <typename T2 = T>
    RefCounted(RefCounted<T2> &&other) noexcept
        requires std::is_convertible_v<T2, T>
        : BaseT(std::move(other))
    {
    }

    // Assignment ops

    RefCounted & operator=(const RefCounted &other)
    {
        BaseT::operator=(other);
        return *this;
    }

    RefCounted & operator=(RefCounted &&other) noexcept
    {
        BaseT::operator=(std::move(other));
        return *this;
    }

    // Member access

    T * operator->() const
    {
        return this->value();
    }

    T & operator*() const
    {
        return *this->value();
    }

    T * get() const
    {
        return this->value();
    }

    const T & cref() const
    {
        return *get();
    }
};

// Specialization that stores the value inside the ref tracker.
template <typename T>
class RefCounted<InPlace<T>> : public RefCountedBase<std::optional<T>>
{
    using BaseT = RefCountedBase<std::optional<T>>;

public:
    RefCounted() = default;

    // Constructors

    // Optionally init with a value.
    explicit RefCounted(RefCounter *counter, std::optional<T> object = { })
        : BaseT(counter, std::move(object))
    {
    }

    // Copy init the value from a ref.
    explicit RefCounted(RefCounter *counter, const T &object_ref)
        : BaseT(counter, std::optional<T> { object_ref })
    {
    }

    RefCounted(const RefCounted &other) noexcept
        : BaseT(other)
    {
    }

    RefCounted(RefCounted &&other) noexcept
        : BaseT(other)
    {
    }

    template <typename T2>	
    RefCounted(const RefCounted<InPlace<T2>> &other) noexcept
        requires std::is_convertible_v<T2, T>
        : BaseT(other)
    {
    }

    template <typename T2>	
    RefCounted(RefCounted<InPlace<T2>> &&other) noexcept
        requires std::is_convertible_v<T2, T>
        : BaseT(std::move(other))
    {
    }

    // Assignment ops

    RefCounted & operator=(const RefCounted &other)
    {
        BaseT::operator=(other);
        return *this;
    }

    RefCounted & operator=(RefCounted &&other) noexcept
    {
        BaseT::operator=(std::move(other));
        return *this;
    }

    // Member access

    auto * operator->() const
    {
        return &this->value_ref();
    }

    auto & operator*() const
    {
        return this->value_ref();
    }

    T value() const
    {
        return this->value_ref();
    }

    const T & cref() const
    {
        return this->value_ref();
    }
};
}

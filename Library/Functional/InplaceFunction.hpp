#pragma once

#include <new>
#include <utility>

#include <Usagi/Library/Functional/FunctionTraits.hpp>
#include <Usagi/Library/Meta/Reflection/Templates.hpp>
#include <Usagi/Library/Objects/Noncopyable.hpp>
#include <Usagi/Platforms/Instructions/BitManipulations.hpp>
#include <Usagi/Runtime/Errors/Errors.hpp>

namespace usagi
{
/**
 * Shio:
 * A fixed-size, zero-allocation type-erased callable.
 * Replaces std::function for mission-critical code paths (like error handling)
 * where dynamic heap allocation is strictly forbidden (e.g. during OutOfMemory
 * faults).
 *
 * Set ExactCapacity to true if the storage buffer should NOT be rounded up
 * to max_align_t. This is useful for tightly packing structures (like error
 * handlers) where external alignment is already guaranteed.
 */
template <
    typename Signature, std::size_t Alignment = alignof(std::max_align_t),
    std::size_t Capacity = platforms::PlatformTraits::cpu_cache_line_size() -
        sizeof(void *) /* sizeof(CallTable *) */
>
class InplaceFunction;

template <
    typename Ret, typename... Args, std::size_t Alignment, std::size_t Capacity
>
class InplaceFunction<Ret(Args...), Alignment, Capacity> : Noncopyable
{
public:
    static constexpr std::size_t BUFFER_CAPACITY = platforms::instructions::
        DefaultBitManipulationInstructions<OperandBitWidth::_64>::align_up_pow2(
            Capacity, Alignment);

private:
    struct CallTable
    {
        Ret (*invoke)(void *, Args &&...);
        void (*move)(void *, void *) noexcept;
        void (*destroy)(void *) noexcept;
    };

    template <typename T, bool IsConstInvocable>
    static Ret invoke_impl(void *storage, Args &&...args)
    {
        if constexpr(IsConstInvocable)
        {
            return (*static_cast<const T *>(storage))(
                std::forward<Args>(args)...);
        }
        else
        {
            return (*static_cast<T *>(storage))(std::forward<Args>(args)...);
        }
    }

    template <typename T>
    static void move_impl(void *dest, void *src) noexcept
    {
        new (dest) T(std::move(*static_cast<T *>(src)));
    }

    template <typename T>
    static void destroy_impl(void *storage) noexcept
    {
        static_cast<T *>(storage)->~T();
    }

    template <typename T>
    static constexpr CallTable sCallTable = {
        &invoke_impl<T, std::is_invocable_r_v<Ret, const T &, Args...>>,
        &move_impl<T>,
        &destroy_impl<T>,
    };

    const CallTable *mCallTable = nullptr;
    alignas(Alignment) std::byte mStorage[BUFFER_CAPACITY];

public:
    constexpr InplaceFunction() noexcept = default;

    explicit constexpr InplaceFunction(std::nullptr_t) noexcept
        : mCallTable(nullptr), mStorage { }
    {
    }

    template <typename T, typename DecayT = std::decay_t<T>>
        requires (
            !meta::is_class_template_instantiation_of<
                ^^DecayT, ^^InplaceFunction
            >() &&
            std::is_invocable_r_v<Ret, DecayT &, Args...>)
    InplaceFunction(T &&functor)
    {
        static_assert(
            sizeof(DecayT) <= BUFFER_CAPACITY,
            "Functor state exceeds InplaceFunction capacity. Cannot allocate "
            "on heap.");
        static_assert(
            std::is_nothrow_move_constructible_v<DecayT>,
            "Functor must be noexcept move constructible.");

        new (mStorage) DecayT(std::forward<T>(functor));
        mCallTable = &sCallTable<DecayT>;
    }

    ~InplaceFunction()
    {
        if(mCallTable)
        {
            mCallTable->destroy(mStorage);
        }
    }

    InplaceFunction(InplaceFunction &&other) noexcept
        : mCallTable(other.mCallTable)
    {
        if(mCallTable)
        {
            mCallTable->move(mStorage, other.mStorage);
            other.mCallTable = nullptr;
        }
    }

    InplaceFunction &operator=(InplaceFunction &&other) noexcept
    {
        if(this != &other)
        {
            if(mCallTable)
            {
                mCallTable->destroy(mStorage);
            }
            mCallTable = other.mCallTable;
            if(mCallTable)
            {
                mCallTable->move(mStorage, other.mStorage);
                other.mCallTable = nullptr;
            }
        }
        return *this;
    }

    [[nodiscard]]
    explicit operator bool() const noexcept
    {
        return mCallTable != nullptr;
    }

    Ret operator()(this auto &self, Args... args)
    {
        USAGI_CHECK_THROW(
            LogicException,
            self.mCallTable != nullptr,
            "Invoking empty InplaceFunction");
        return self.mCallTable->invoke(
            const_cast<void *>(static_cast<const void *>(self.mStorage)),
            std::forward<Args>(args)...);
    }
};

// Shio:
// Deduction guide for lambdas and functors.
// Uses the decltype of the lambda's operator() to deduce the InplaceFunction's
// signature, and accurately extracts its exact alignment requirements.
template <typename Functor>
InplaceFunction(Functor) -> InplaceFunction<
    functional::function_signature_t<decltype(&Functor::operator())>,
    alignof(Functor)
>;

// Shio:
// Deduction guide for standard function pointers.
template <typename ReturnType, typename... Args>
InplaceFunction(ReturnType (*)(Args...))
    -> InplaceFunction<ReturnType(Args...), alignof(ReturnType (*)(Args...))>;

namespace details::static_tests
{
static int dummy_free_function(float)
{
    return 0;
}

consteval void test_inplace_function_ctad()
{
    auto lambda = [](int a, double b) -> float { return 0.0f; };

    // 1. Lambda deduction
    InplaceFunction func1 = lambda;
    static_assert(std::is_same_v<
        decltype(func1),
        InplaceFunction<float(int, double), alignof(decltype(lambda))>
    >);
    static_assert(sizeof(func1) == 64);

    // 2. Mutable lambda deduction
    auto mutable_lambda   = [state = 0](
                                int a) mutable -> bool { return ++state > 0; };
    InplaceFunction func2 = mutable_lambda;
    static_assert(std::is_same_v<
        decltype(func2),
        InplaceFunction<bool(int), alignof(decltype(mutable_lambda))>
    >);
    static_assert(sizeof(func2) == 64);

    // 3. Free function pointer deduction
    InplaceFunction func3 = &dummy_free_function;
    static_assert(std::is_same_v<
        decltype(func3),
        InplaceFunction<int(float), alignof(decltype(&dummy_free_function))>
    >);
    static_assert(sizeof(func3) == 64);
}
} // namespace details::static_tests
} // namespace usagi

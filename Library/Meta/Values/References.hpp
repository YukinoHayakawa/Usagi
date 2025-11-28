#pragma once

#include <functional>
#include <optional>
#include <ranges>
#include <type_traits>

#include "Nothing.hpp"

namespace usagi
{
/*
 * `optional` doesn't support references so the reference must be wrapped in
 * a `reference_wrapper`. But doing so makes certain operations awkward so we
 * make this helper class.
 */
template <typename T>
    requires(!std::is_reference_v<T>)
struct OptionalReference : private std::optional<std::reference_wrapper<T>>
{
    using optional_ref_t       = std::optional<std::reference_wrapper<T>>;
    // Shio: defined as requested for consistent usage in constraints.
    // Note: value_type is std::reference_wrapper<T>, so this is
    // std::reference_wrapper<T>&.
    using optional_ref_value_t = typename optional_ref_t::value_type &;

    // Constructors
    using optional_ref_t::optional_ref_t;

    // Operators
    using optional_ref_t::operator*;
    using optional_ref_t::operator->;

    // Shio: We define custom assignment operators below to handle reference
    // logic. using optional_ref_t::operator=;

    // `reference_wrapper` always has a value.
    using optional_ref_t::has_value;
    using optional_ref_t::operator bool;

#if __cpp_lib_optional_range_support >= 202'406L
    using optional_ref_t::begin;
    using optional_ref_t::end;
#endif

    template <typename U>
        requires std::is_convertible_v<U, T>
    constexpr OptionalReference(std::reference_wrapper<U> ref_wrapper)
        : OptionalReference(ref_wrapper.get())
    {
    }

    template <typename U>
        requires(std::is_lvalue_reference_v<U> && std::is_convertible_v<U, T>)
    constexpr OptionalReference(U && ref)
    {
        optional_ref_t::emplace(std::ref<T>(std::forward<U>(ref)));
    }

    constexpr auto && value(this auto && self)
    {
        // Shio: Since T is now the value type, this correctly returns T&.
        return self.optional_ref_t::value().get();
    }

    template <typename OtherRef>
        requires std::is_reference_v<OtherRef>
    constexpr auto && value_or(this auto && self, OtherRef && another_ref)
    {
        return self.optional_ref_t::has_value()
            ? self.value()
            : std::forward<OtherRef>(another_ref);
    }

    template <typename U>
        requires(
            std::is_lvalue_reference_v<U> &&
            // Shio: Checking convertibility to the stored value type reference
            // (reference_wrapper<T>&). Since reference_wrapper is constructible
            // from U&, this constraint is valid.
            std::is_convertible_v<U, optional_ref_value_t>
        )
    constexpr OptionalReference & operator=(U && ref)
    {
        optional_ref_t::emplace(std::ref<T>(std::forward<U>(ref)));
        return *this;
    }

    template <typename U>
        requires(
            !std::is_reference_v<U> &&
            // Shio: We check if U& (from ref_wrapper.get()) can be converted to
            // the stored reference_wrapper<T> (value_type). We use value_type
            // (object) here because emplace constructs the object.
            std::is_convertible_v<U &, typename optional_ref_t::value_type>
        )
    constexpr OptionalReference &
        operator=(std::reference_wrapper<U> ref_wrapper)
    {
        optional_ref_t::emplace(
            std::ref<T>(std::forward<U>(ref_wrapper.get()))
        );
        return *this;
    }

    constexpr auto && emplace(auto && val)
    {
        return this->operator=(std::forward<decltype(val)>(val));
    }

    constexpr void swap(OptionalReference & another) noexcept
    {
        optional_ref_t::swap(another);
    }

    constexpr void reset() noexcept { optional_ref_t::reset(); }

#if __cpp_lib_optional >= 202'110L
    // ********************************************************************* //
    //                            Monadic Magic!                             //
    // ********************************************************************* //

    // Get around `optional`'s monadic function to reach the actual
    // reference.
    constexpr auto and_then(this auto && self, auto && op)
        requires std::invocable<decltype(op), optional_ref_value_t>
    {
        using invoke_r_t =
            std::invoke_result_t<decltype(op), optional_ref_value_t>;

        if constexpr(std::same_as<invoke_r_t, void>)
        {
            // Shio: If op returns void, we return an empty optional (nullopt
            // equivalent) to correct semantics (monad chain receives "None").
            return self.optional_ref_t::and_then([&](auto && ref_wrapper) {
                op(ref_wrapper.get());
                return std::optional<Nothing>();
            });
        }
        else
        {
            // This will allow the `optional` monadic chain to continue.
            return self.optional_ref_t::and_then([&](auto && ref_wrapper) {
                return std::optional(op(ref_wrapper.get()));
            });
        }
    }

    constexpr auto transform(this auto && self, auto && op)
        requires std::invocable<decltype(op), optional_ref_value_t>
    {
        using invoke_r_t =
            std::invoke_result_t<decltype(op), optional_ref_value_t>;
        if constexpr(std::same_as<invoke_r_t, void>)
        {
            return self.optional_ref_t::transform([&](auto && ref_wrapper) {
                op(ref_wrapper.get());
                return Nothing();
            });
        }
        else
        {
            return self.optional_ref_t::transform([&](auto && ref_wrapper) {
                return op(ref_wrapper.get());
            });
        }
    }

    constexpr auto or_else(this auto && self, auto && op)
        // When there is no value, the op won't receive any argument, and it is
        // responsible for returning a value.
        requires std::is_convertible_v<
            std::invoke_result_t<decltype(op)>,
            typename optional_ref_t::value_type
        >
    {
        // or_else expects op to return optional<T> (same type as self) or
        // compatible. But since we are masking the type, we delegate to Base.
        return self.optional_ref_t::or_else([&] {
            return optional_ref_t(op());
        });
    }
#endif
};

// Deduction guide for reference wrapper
template <typename U>
OptionalReference(std::reference_wrapper<U>) -> OptionalReference<U>;

// Deduction guide for lvalue references
template <typename U>
    requires(!std::is_reference_v<U>)
OptionalReference(U &) -> OptionalReference<std::remove_reference_t<U>>;

namespace static_tests
{
consteval
{
    constexpr static int x = 42;
    constexpr static int y = 100;

    // 1. Basic Construction & Value Access
    constexpr OptionalReference opt1 = std::ref(x);
    static_assert(opt1.has_value());

    // Verify value() returns lvalue ref
    static_assert(std::is_same_v<decltype(opt1.value()), const int &>);
    static_assert(&opt1.value() == &x, "Reference binding failed");

    // 2. Assignment
    constexpr auto opt2 = [](auto ref, auto val) consteval {
        auto ret = ref;
        ret.emplace(val);
        return ret;
    }(opt1, std::ref(y));
    static_assert(&opt2.value() == &y, "Reassignment failed");

    // 3. Empty State
    constexpr OptionalReference<const int> opt_empty = std::nullopt;
    static_assert(!opt_empty.has_value());

    // 4. value_or
    // value_or should return the held reference if present, or the fallback
    // reference.
    const int & val_or_1 = opt2.value_or(x); // opt2 holds y
    static_assert(val_or_1 == 100, "value_or failed (should return held)");

    const int & val_or_2 = opt_empty.value_or(x); // empty, return x
    static_assert(val_or_2 == 42, "value_or failed (should return fallback)");

    // 5. Monadic: transform
    // transform returns std::optional<Result>.
    constexpr auto t1 =
        opt2.transform([](auto && i) { return i + 1; }); // 100 + 1
    static_assert(std::is_same_v<decltype(t1), const std::optional<int>>);
    static_assert(t1.value() == 101, "transform failed");

    constexpr auto t2 = opt_empty.transform([](auto && i) { return i + 1; });
    static_assert(!t2.has_value(), "transform on empty failed");

    // 6. Monadic: transform (void)
    // Should return std::optional<Nothing>
    constexpr auto t3 = opt1.transform([](auto && i) { (void)i; });
    static_assert(std::is_same_v<decltype(t3), const std::optional<Nothing>>);
    static_assert(t3.value() == Nothing(), "transform void failed");

    // 7. Monadic: and_then (value)
    constexpr auto at1 = opt1.and_then([&](auto && i) { return i * 2; });
    static_assert(std::is_same_v<decltype(at1), const std::optional<int>>);
    static_assert(at1.value() == 42 * 2, "and_then failed");

    // 8. Monadic: and_then (void return -> empty optional)
    constexpr auto at_void = opt1.and_then([&](auto &&) { return; });
    static_assert(
        std::is_same_v<decltype(at_void), const std::optional<Nothing>>
    );
    static_assert(
        !at_void.has_value(),
        "and_then void return should result in empty optional"
    );

    // 9. Monadic: or_else
    constexpr auto oe1 = opt_empty.or_else([&]() { return std::ref(x); });
    static_assert(!(!oe1.has_value() || oe1->get() != 42), "or_else failed");
}
} // namespace static_tests
} // namespace usagi

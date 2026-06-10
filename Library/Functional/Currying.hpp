#pragma once

#include <concepts>
#include <functional>
#include <string_view>
#include <type_traits>

namespace usagi::functional
{
/*
 * Function Currying, also Monadic?
 */

/*
 * Ensure the argument pack is non-empty and can be captured in lambda.
 */
template <typename... Args>
concept MovableOrCopyableArgPack = (sizeof...(Args) > 0) &&
    (((std::is_rvalue_reference_v<Args> &&
          std::is_move_constructible_v<Args>) ||
         std::is_copy_constructible_v<Args>) &&
        ...);

/*
 * Ensure the Callable can be called with zero args.
 */
template <typename Fn, typename... Args>
concept NullaryVoidInvocable = (sizeof...(Args) == 0) &&
    std::invocable<Fn> &&
    std::is_void_v<std::invoke_result_t<Fn>>;

// must not write RetT = invoke_result_t cuz it can fail to compile.
// and void must be handled as special case.
template <typename Fn, typename RetT, typename... Args>
concept NullaryInvocableRet = (sizeof...(Args) == 0) &&
    (std::invocable<Fn> &&
        (std::is_void_v<std::invoke_result_t<Fn>> ||
            std::same_as<std::invoke_result_t<Fn>, RetT> ||
            std::convertible_to<std::invoke_result_t<Fn>, RetT>));

// somehow doesn't work???
// requires(Fn &&fn) {
//     { fn() } -> std::same_as<RetT>;
// } ||
// requires(Fn &&fn) {
//     { fn() } -> std::convertible_to<RetT>;
// };

template <typename Fn, typename... Args>
concept NullaryInvocable = sizeof...(Args) == 0 && std::invocable<Fn>;

// requires(Fn &&fn) { { fn() }; };

/*
template <typename Fn>
using maybe_nullary_invocable_r_t = decltype([] {
    if constexpr(NullaryInvocable<Fn> && !NullaryVoidInvocable<Fn>)
        return std::declval<std::invoke_result_t<Fn>>();
    else
        return;
}());
*/

/*
 * Let's cook some Curry!!!
 *
 * ... But we have some problems:
 * See, in Haskell, T0 -> T1 -> T2 is simple.
 * Here in C++, we have default arguments and variadic templates.
 * How the fuck do you even decide the arity?
 * ... We can't easily do that.
 *
 * Anyway, we want the final result to be a LazilyEvaluatedThunk.
 *
 * And this cooker must remain rvalue to be used to avoid:
 * - accidentally overwriting internal states by currying to the old object
 * - splitting into multiple states (actually this can be allowed?)
 * - unoptimized code, cuz a chain of calls is much easier to optimize. you
 *   got zero-overhead abstraction here.
 *
 * Gemini:
 * "Currying is the mathematical process of transforming a polyadic function
 * into a nested sequence of unary functions."? I hope this holds in C++.
 *
 * Contract: The Currying chain must not be abandoned before getting the result.
 *
 * NOTE: ALL functions must require rvalue ref to ensure the chain's integrity.
 */
// todo: check Callable is a Callable
template <
    typename Callable,
    typename OriginalFn = Callable,
    typename... CurriedArgs
>
// requires std::is_invocable_v<Callable, Args...>
struct [[nodiscard]] CurryCooker
{
    /*
     * 1. Create the Cooker!
     * the ctor just takes a Callable. whatever that is. we don't know the
     * arity or whether it's callable at all.
     * Uncurried/Curried/Polyadic/Partially Applied? Whatever.
     * Can be lambda/function_ptr/std::function/any sort of functors.
     * Even another CurryCooker.
     */
    constexpr CurryCooker(Callable f)
        : func(std::move(f)), has_func(true)
    {
    }

    // used in get_frozen()
    constexpr CurryCooker(CurryCooker &&other) noexcept
        : func { std::move(other.func) }, has_func(other.has_func)
    {
        other.has_func = false;
    }

    // check that the result is actually yield (at least in constexpr contexts)
    constexpr ~CurryCooker()
    {
        if(has_func) // constexpr time sanity check
        {
            // currying_chain_abandoned
            raise_compile_error("curried function never called.");
        }
    }

    // no copying for obvious reasons
    CurryCooker(const CurryCooker &other)                = delete;
    CurryCooker &operator=(const CurryCooker &other)     = delete;
    // a move assignment would also be doing no good. no overwriting allowed.
    // actually, how'd you overwrite a wrapper around random lambdas if you
    // can't copy it in the first place?
    CurryCooker &operator=(CurryCooker &&other) noexcept = delete;

    /*
     * 2. Partial application/Binding! ... to the front, of course.
     * trying to reduce arity! ... or is it?
     * this is not strictly currying though. but it binds to the front.
     */
    template <typename... Curries>
    [[nodiscard]]
    // requires rvalue reference so what you must move the whole chain together.
    // so it becomes a one way operation and you cannot drop the chain.
    // Gemini calls it a "destructive state transition, preventing the reuse of
    // a consumed currying step."
    constexpr auto operator()(Curries &&...curries) &&
        requires MovableOrCopyableArgPack<decltype(curries)...>
    {
        // todo review compiled code about how things unroll
        // move/copy capture all arguments.
        // a mutable modifier is not needed here cuz all captured args are
        // either copied or moved in, the lambdas are nested and will only be
        // unrolled when the original function is called. - wrong
        // this closure must be mutable. for example, if it captures some other
        // callables which are to be invoked by some of the args, having this
        // closure const will make it impossible to invoke captured callables.
        // this becomes the next func and the current cooker is done with.
        // todo: what if someone calls a moved cooker? UB?
        auto next = [... stew = std::forward<Curries>(curries),
                        f = std::move(func)](auto &&...more_curries) mutable {
            return f(
                stew..., std::forward<decltype(more_curries)>(more_curries)...
            );
        };
        // invalidate this cooker and transfer out the currying chain.
        // since the old func is moved into next, this is semantically
        // equivalent to moving ourself out.
        // keep a record of the original Fn type and curried params it would
        // make error messages much easier to read when you accidentally
        // curried wrong types.
        return has_func = false,
               CurryCooker<
                   decltype(next), OriginalFn, CurriedArgs...,
                   decltype(curries)...
               >(std::move(next));
    }

    // prefer using operator()
    [[nodiscard]]
    constexpr decltype(auto) operator<<(auto &&curry) &&
        requires MovableOrCopyableArgPack<decltype(curry)>
    {
        return operator()(std::forward<decltype(curry)>(curry));
    }

    static constexpr bool Invocable =
        std::invocable<OriginalFn, CurriedArgs...>;

    /*
     * 3.1. Evaluate (serve) the result immediately and destroy the chain.
     */
    /*
     * damn fuck it seems very difficult to allow specifying a return type.
     * cuz if the function is not saturated, invoke_result_t will fail.
     * and for enable_if to work, ResultT has to be clean, and enable_if must
     * be dependent on it. requirements is evaluated after targs.
     */
    // template <
    //     // typename ResultT = std::invoke_result_t<Callable>,
    //     // must SFINAE it out if the function is not saturated
    //     // otherwise, intermediate Cookers cannot be instantiated
    //     // typename ResultT = std::enable_if_t<
    //     // NullaryInvocable<Callable>,
    //     // std::invoke_result_t<Callable>
    //     // >
    //     typename ResultT = maybe_nullary_invocable_r_t<Callable>
    // >
    [[nodiscard]]
    constexpr decltype(auto) cooked() &&
        // note that, is_invocable_r_v is harder to debug than decltype
        // requires std::invocable<Callable>
        requires std::invocable<OriginalFn, CurriedArgs...>
    {
        // moving self out, return result, ending the chain.
        // just some type dance here.
        // note that this can only be called once cuz self would be gone.
        // note that here operator() cannot be called. otherwise it
        // creates a dependency loop.
        return has_func = false, CurryCooker(std::move(*this)).func();
    }

    /*
     * 3.2. Create a thunk for evaluating the result and get yourself cooked
     * later. Also destroy the chain.
     */
    // decltype is easier to debug than invoke_result_t
    // template <
    //     typename ResultT = maybe_nullary_invocable_r_t<Callable>
    //     // typename ResultT = std::enable_if_t<
    //     // NullaryInvocable<Callable>,
    //     // std::invoke_result_t<Callable>
    //     // >
    //     // typename ResultT = std::invoke_result_t<Callable>,
    //     // typename = std::enable_if_t<std::is_invocable_r_v<ResultT,
    //     Callable>>
    // >
    [[nodiscard]]
    constexpr auto frozen() &&
        // requires std::is_invocable_r_v<ResultT, Callable>
        // requires std::invocable<Callable>
        // checking this can give you a better idea why function signature
        // mismatched
        requires std::invocable<OriginalFn, CurriedArgs...>
    {
        // return a Thunk for one-time use, ending the current chain.
        // DO NOT set `has_func = false` here. leave it to the move ctor.
        // and, we are being functional here, doesn't really expect you can
        // get cooked twice.
        // moving ourself into a closure ensures we will die naturally later.
        // this gotta be
        return [self = std::move(*this)] mutable {
            // gotta get it into a rvalue to cook
            return std::move(self).cooked();
        };
    }

    /*
     * A lazier way to get cooked. Returns default return type.
     */
    // template <typename = std::enable_if_t<std::is_invocable_v<Callable>>>
    [[nodiscard]]
    // this function actually requires zero args.
    constexpr decltype(auto)
        operator()(/*[[maybe_unused]] auto &&...no_args*/) &&
        // requires std::invocable<Callable>
        requires std::invocable<OriginalFn, CurriedArgs...>
    // requires NullaryInvocable<Callable>
    // requires (NullaryInvocableRet<
    //     Callable,
    //     std::invoke_result_t<Callable>,
    //     decltype(no_args)...
    // >)
    {
        return std::move(*this).cooked();
    }

    /*
     * Implicitly to get cooked.
     */
    template <typename ResultT>
    [[nodiscard]]
    constexpr operator ResultT() &&
        requires std::invocable<OriginalFn, CurriedArgs...> &&
        std::is_invocable_r_v<ResultT, Callable>
    {
        return std::move(*this).cooked();
    }

private:
    Callable func;
    bool     has_func = false;

    // non-constexpr function to cause compile error
    static void raise_compile_error(const char *) { }

    // constexpr auto arity = ?

}; // namespace usagi::functional

template <typename Fn>
[[nodiscard]]
constexpr auto cook(Fn &&func)
{
    return CurryCooker(std::forward<Fn>(func));
}

namespace static_tests::curry_cooker
{
struct s
{
    static constexpr void nullary_void() { }

    static constexpr auto add_many(auto &&...args) { return (args + ... + 0); }
};

constexpr auto test_curry_cooker()
{
    /*
     * things to test:
     * [free function, lambda]
     * [fixed args, with default args, variadic args]
     * [trivially copyable, copyable, moveable, dangling references]
     * CurryCooker lifetime.
     */

    /*
     * Nullary Functions Returning Void
     */

    constexpr auto l0 = [] { };
    {
        // cannot be constexpr here cuz the dtor will see the func never
        // called. constexpr auto c0 = CurryCooker(l0);
        auto c0 = CurryCooker(l0);
        static_assert(std::is_invocable_r_v<void, decltype(std::move(c0))>);
        // invoke the function to clean has_func flag. though it returns
        // void. if this returns a value, you will get a warning for
        // ignoring nodiscard.
        std::move(c0)();
        // now c0 is dead dead. calling it again is UB.
    }
    // same as above
    static_assert(NullaryInvocableRet<decltype(CurryCooker(l0)), void>);
    // free FUNCTION type
    static_assert(
        NullaryInvocableRet<decltype(CurryCooker(s::nullary_void)), void>
    );
    // function POINTER
    static_assert(
        NullaryInvocableRet<decltype(CurryCooker(&s::nullary_void)), void>
    );

    /*
     * Polyadic/Variadic Functions
     */

    // cuz can't just take a template function
    constexpr auto l1 = [](auto &&...args) {
        return s::add_many(std::forward<decltype(args)>(args)...);
    };
    // explicit cast, function evaluated
    static_assert((int)cook(l1) == 0);
    // implicit cast, evaluated, with closure containing ref.
    static_assert([&] {
        constexpr int x = cook([&](int b) { return l1(b); })(123);
        return x;
    }() == 123);
    // static_assert(CurryCooker(l1) == 0); -> ambiguous casting

    // todo: i want something like: CurryCooker(l1)(1) >> ^^int
    //   and this needs constexpr params.

    // eagerly evaluate by explicit cast
    static_assert((int)cook(l1)(1) == 1);
    static_assert((int)cook(l1)(1)(2) == 1 + 2);
    static_assert((int)cook(l1)(1, 2) == 1 + 2);
    static_assert((int)cook(l1)(1, 2)(3) == 1 + 2 + 3);

    // eagerly evaluate by cooking
    static_assert(cook(l1)(1).cooked() == 1);
    static_assert(cook(l1)(1)(2).cooked() == 1 + 2);
    static_assert(cook(l1)(1, 2).cooked() == 1 + 2);
    static_assert(cook(l1)(1, 2)(3).cooked() == 1 + 2 + 3);

    // eagerly evaluate by invoking
    static_assert(cook(l1)(1)() == 1);
    static_assert(cook(l1)(1)(2)() == 1 + 2);
    static_assert(cook(l1)(1, 2)() == 1 + 2);
    static_assert(cook(l1)(1, 2)(3)() == 1 + 2 + 3);

    // some random
    constexpr auto l2 = [](std::string_view s, std::size_t c, bool b) {
        return s.size() == c && b;
    };

    static_assert(cook(l2)("test", 4)(true)() == true);
    static_assert(cook(l2)("test", 4, false)() == false);
    static_assert(cook(l2)("")(4)(false)() == false);

    /*
     * Lifetime and Contracts
     */

    // a function used for calling CurryCookers
    constexpr auto l3 = [](auto &&...ops) {
        // (void) to avoid nodiscard warnings
        ((void)std::move(ops)(), ...);
    };

    // nested Cookers and support for move constructor
    static_assert(cook(cook(l1))(1)(2)() == 1 + 2);
    cook(l3)(cook(l1))(cook(l1))();
    // todo: this chain is not evaluated but not causing compile error
    auto x = cook(l3)(cook(l1))(cook(l1));

    /*
     * Lazy Evaluation
     */

    static_assert([&] {
        // c0 -> string_view -> size_t -> bool -> bool
        auto c0 = cook(l2);
        static_assert(!decltype(c0)::Invocable);
        static_assert(!std::is_invocable_r_v<bool, decltype(c0)>);
        static_assert(
            !std::is_invocable_r_v<bool, decltype(c0), std::string_view>
        );
        static_assert(!std::is_invocable_r_v<
            bool, decltype(c0), std::string_view, std::size_t
        >);
        static_assert(std::is_invocable_r_v<
            bool, decltype(c0), std::string_view, std::size_t, bool
        >);
        // c1 -> size_t -> bool -> bool
        auto c1 = std::move(c0)("test");
        static_assert(!decltype(c1)::Invocable);
        static_assert(!std::is_invocable_r_v<bool, decltype(c1)>);
        static_assert(!std::is_invocable_r_v<bool, decltype(c1), std::size_t>);
        static_assert(
            std::is_invocable_r_v<bool, decltype(c1), std::size_t, bool>
        );
        // currying wrong type is a fucking disaster.
        // need a way to carry the original type and appended types.
        // c2 -> bool -> bool
        // auto c2 = std::move(c1)("4");
        auto c2 = std::move(c1)(4);
        static_assert(!decltype(c2)::Invocable);
        static_assert(!std::is_invocable_r_v<bool, decltype(c2)>);
        static_assert(std::is_invocable_r_v<bool, decltype(c2), bool>);
        // c3 -> bool
        auto c3 = std::move(c2)(true);
        static_assert(decltype(c3)::Invocable);
        static_assert(std::is_invocable_r_v<bool, decltype(c3)>);
        auto frozen = std::move(c3).frozen();
        return frozen();
    }() == true);
}
} // namespace static_tests::curry_cooker
} // namespace usagi::functional

/*
#include <iostream>

int main()
{
    using namespace usagi::functional;
    // 1. You start with a raw function
    auto apply_for_jobs = [](int resumes, int ghostings) {
        std::cout << "Sent " << resumes << " resumes, got " << ghostings
                  << " ghosted.\n";
    };
    // 2. "Currying in more args" / Layering the ingredients
    auto current_state = CurryCooker(apply_for_jobs)(500); // 500 resumes sent
    // 3. Choice A: Get the final result immediately
    // Output: Sent 500, got 499 ghosted.
    std::move(current_state)(499).cooked();
    // 4. Choice B: Delay the despair for later (Create a Thunk)
    auto mid_crisis = std::move(current_state)(499).frozen();
    // ... weeks pass ...
    mid_crisis(); // Triggers the calculation / execution!
}
*/

/*
int main()
{
    auto print_job_status = [](int apps, int rejections) {
        std::cout << "Applied to: " << apps << " | Rejected by: " << rejections
                  << "\n";
    };
    // Start cooking the function
    auto active_chain = cook(print_job_status)(100);
    // Choice A: Serve it hot immediately
    std::move(active_chain)(99).cooked();
    // Choice B: Toss it in the freezer for later
    auto freeze_frame = cook(print_job_status)(100)(99).frozen();
}
*/

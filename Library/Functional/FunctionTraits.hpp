#pragma once

namespace usagi::functional
{
/**
 * Shio:
 * A type trait utility to extract the function signature (ReturnType(Args...))
 * from a given callable type (e.g., lambdas, functors, member function
 * pointers).
 */
template <typename T>
struct function_traits;

// Specialization for member function pointers (including lambda operator())
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...) const>
{
    using signature = ReturnType(Args...);
};

// Specialization for mutable member function pointers (mutable lambdas)
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...)>
{
    using signature = ReturnType(Args...);
};

// Specialization for standard function pointers
template <typename ReturnType, typename... Args>
struct function_traits<ReturnType (*)(Args...)>
{
    using signature = ReturnType(Args...);
};

// Specialization for references to functions
template <typename ReturnType, typename... Args>
struct function_traits<ReturnType (&)(Args...)>
{
    using signature = ReturnType(Args...);
};

// Convenience alias
template <typename T>
using function_signature_t = typename function_traits<T>::signature;

template <auto F0, auto F1>
concept SameFunctionSignature = std::is_same_v<
    function_signature_t<decltype(F0)>, function_signature_t<decltype(F1)>
>;
} // namespace usagi::functional

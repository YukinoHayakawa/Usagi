#pragma once

#include <cstddef>

namespace yuki
{

/**
 * \brief Apply first N arguments in a non-type parameter pack to a template. This is the primary template.
 * \tparam N Number of arguments in Args... should be applied to Target.
 * \tparam T The type of values being applied.
 * \tparam Target To which template the values will be applied.
 * Template not directly fitting the signature may be aliased using using directive. e.g.
 * template <typename T, T...> struct foo { };
 * template <int...Ch> using bar = foo<int, Ch...>;
 * Then use bar as Target.
 * \tparam Args A non-type template parameter pack of which the first N values will be applied to Target
 */
template <size_t N, typename T, template <T...> typename Target, T... Args>
struct apply_first_n_value;

/**
 * \brief Specialized for applying no arguments to Target.
 * \tparam T 
 * \tparam Target 
 */
template <typename T, template <T...> typename Target>
struct apply_first_n_value<0, T, Target>
{
    typedef Target<> type;
};

/**
 * \brief Specialized for terminating the applying action.
 * \tparam T 
 * \tparam Target This is necessary to make this specialization more special than the next one.
 * \tparam First 
 * \tparam Args 
 */
template <typename T, template <T...> typename Target, T First, T... Args>
struct apply_first_n_value<0, T, Target, First, Args...>
{
    typedef Target<> type;
};

/**
 * \brief Specialized for expanding the parameter pack.
 * \tparam N 
 * \tparam T 
 * \tparam Target 
 * \tparam First 
 * \tparam Others 
 */
template <size_t N, typename T, template <T...> typename Target, T First, T... Others>
struct apply_first_n_value<N, T, Target, First, Others...>
{
    static_assert(N <= 1 + sizeof...(Others), "paramater pack index out of bound");

private:
    template <T...Others2>
    using apply_next_t = Target<First, Others2...>;
    
public:
    typedef typename apply_first_n_value<N - 1, T, apply_next_t, Others...>::type type;
};

}

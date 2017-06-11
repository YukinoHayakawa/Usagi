#pragma once

#include <cstddef>

namespace yuki
{

/**
 * \brief Apply first N arguments in a non-type parameter pack to a template.
 * This is the primary template.
 * \tparam N Number of arguments in Args... applied to Target. If N > sizeof...(Args), all arguments
 * will be used.
 * \tparam T The type of arguments.
 * \tparam Target A template accepting N parameters of T.
 * Template not directly fitting the signature may be aliased using using directive. e.g.
 * template <typename T, T...> struct foo { };
 * template <int...Ch> using bar = foo<int, Ch...>;
 * \tparam Args A non-type template parameter pack whose first N values will be applied to Target
 */
template <std::size_t N, typename T, template <T...> typename Target, T... Args>
struct apply_first_n_value;

/**
* \brief Specialization for the applying no arguments to the template.
* \tparam N
* \tparam T
* \tparam Target
*/
template <typename T, template <T...> typename Target>
struct apply_first_n_value<0, T, Target>
{
    typedef Target<> type;
};

/**
 * \brief Specialization for applying no arguments to the template from a parameter pack having
 * one or more arguments.
 * \tparam T 
 * \tparam Target 
 */
template <typename T, template <T...> typename Target, T First, T... Args>
struct apply_first_n_value<0, T, Target, First, Args...>
{
    typedef Target<> type;
};

/**
 * \brief Specialization for insufficient arguments.
 * \tparam N 
 * \tparam T 
 * \tparam Target 
 */
template <std::size_t N, typename T, template <T...> typename Target>
struct apply_first_n_value<N, T, Target>
{
    typedef Target<> type;
};

/**
 * \brief Speacialization as recursion start point.
 * \tparam N 
 * \tparam T 
 * \tparam Target 
 * \tparam First 
 * \tparam Others 
 */
template <std::size_t N, typename T, template <T...> typename Target, T First, T... Others>
struct apply_first_n_value<N, T, Target, First, Others...>
{
private:
    template <T...NextOthers>
    using apply_next_t = Target<First, NextOthers...>;
    
public:
    typedef typename apply_first_n_value<N - 1, T, apply_next_t, Others...>::type type;
};

}

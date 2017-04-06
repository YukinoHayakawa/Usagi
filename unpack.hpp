#pragma once

#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/logical/not.hpp>
#include <boost/vmd/is_empty.hpp>

/**
 * \brief Pass on a set of macro parameter.
 * To pass more than one tokens as a macro argument, they must be enclosed with a pair os
 * parentheses, which are, however, not always wanted. To remove the parentheses, directly
 * use the name of this macro before the parameter.
 * 
 * e.g.
 * #define MERGE(set1, set2) YUKI_UNPACK(x, y, z), YUKI_UNPACK(a, b, c)
 * and MERGE((x, y, z), (a, b, c)) will be expanded to x, y, z, a, b, c
 * 
 * Ref: http://stackoverflow.com/a/35999754
 */
#define YUKI_UNPACK(...) __VA_ARGS__

/**
 * \brief Similar to YUKI_UNPACK, but automatically prepend a comma only if the input is
 * not empty.
 */
#define YUKI_TAIL_UNPACK(...) BOOST_PP_COMMA_IF(BOOST_PP_NOT(BOOST_VMD_IS_EMPTY(__VA_ARGS__))) __VA_ARGS__

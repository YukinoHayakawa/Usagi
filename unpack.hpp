#pragma once

/**
 * \brief Pass on a set of macro parameter.
 * To pass more than one tokens as a macro argument, they must be enclosed with a pair os
 * parentheses, which are, however, not always wanted. To remove the parentheses, directly
 * use the name of this macro before the parameter.
 * 
 * e.g.
 * #define MERGE(set1, set2) YUKI_UNPACK(x, y, z), YUKI_UNPACK(a, b, c)
 * and MERGE((x, y, z), (a, b, c)) will be expanded to x, y, z, a, b, c
 */
#define YUKI_UNPACK(...) __VA_ARGS__

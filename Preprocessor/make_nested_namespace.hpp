#pragma once

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>

#include "tuple.hpp"

#define YUKI_MAKE_NESTED_NAMESPACE_PRE(next, count, ns) \
    namespace ns { \
/**/
#define YUKI_MAKE_NESTED_NAMESPACE_POST(_next, _this, _data) \
    } \
/**/

// handle non-empty tuple
#define YUKI_MAKE_NESTED_NAMESPACE_NONEMPTY(_ns_tuple, _ns_size, _code) \
    BOOST_PP_SEQ_FOR_EACH(YUKI_MAKE_NESTED_NAMESPACE_PRE, _ns_size, BOOST_PP_TUPLE_TO_SEQ(_ns_tuple)) \
    YUKI_UNPACK _code \
    BOOST_PP_REPEAT(_ns_size, YUKI_MAKE_NESTED_NAMESPACE_POST, BOOST_PP_EMPTY()) \
/**/

// handle empty tuple
#define YUKI_MAKE_NESTED_NAMESPACE_EMPTY(_ns_tuple, _ns_size, _code) \
    YUKI_UNPACK _code \
/**/

#define YUKI_MAKE_NESTED_NAMESPACE_CHECK(_ns_tuple, _ns_size, _code) \
    BOOST_PP_IF(_ns_size, YUKI_MAKE_NESTED_NAMESPACE_NONEMPTY, YUKI_MAKE_NESTED_NAMESPACE_EMPTY) \
        (_ns_tuple, _ns_size, _code) \
/**/

/**
 * \brief Surround code with nested namespaces.
 * Directly expanding _ns with YUKI_UNPACK in the BOOST_PP_OVERLOAD macro generates error
 * so an intermediate macro is added.
 * todo: (C++17 N4230) use nested namespace definition
 * \param _ns A list of namespaced enclosed by a pair of parentheses and seperated by comma.
 * \param _code The code to wrap in the inner most namespace enclosed by a pair of parentheses.
 */
#define YUKI_MAKE_NESTED_NAMESPACE(_ns_tuple, _code) \
    YUKI_MAKE_NESTED_NAMESPACE_CHECK(_ns_tuple, YUKI_TUPLE_SIZE(_ns_tuple), _code) \
/**/

#pragma once

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>

#include "tuple.hpp"

#define USAGI_MAKE_NESTED_NAMESPACE_PRE(next, count, ns) \
    namespace ns { \
/**/
#define USAGI_MAKE_NESTED_NAMESPACE_POST(_next, _this, _data) \
    } \
/**/

// handle non-empty tuple
#define USAGI_MAKE_NESTED_NAMESPACE_NONEMPTY(_ns_tuple, _ns_size, _code) \
    BOOST_PP_SEQ_FOR_EACH(USAGI_MAKE_NESTED_NAMESPACE_PRE, _ns_size, BOOST_PP_TUPLE_TO_SEQ(_ns_tuple)) \
    USAGI_UNPACK _code \
    BOOST_PP_REPEAT(_ns_size, USAGI_MAKE_NESTED_NAMESPACE_POST, BOOST_PP_EMPTY()) \
/**/

// handle empty tuple
#define USAGI_MAKE_NESTED_NAMESPACE_EMPTY(_ns_tuple, _ns_size, _code) \
    USAGI_UNPACK _code \
/**/

#define USAGI_MAKE_NESTED_NAMESPACE_CHECK(_ns_tuple, _ns_size, _code) \
    BOOST_PP_IF(_ns_size, USAGI_MAKE_NESTED_NAMESPACE_NONEMPTY, USAGI_MAKE_NESTED_NAMESPACE_EMPTY) \
        (_ns_tuple, _ns_size, _code) \
/**/

/**
 * \brief Surround code with nested namespaces.
 * Directly expanding _ns with USAGI_UNPACK in the BOOST_PP_OVERLOAD macro generates error
 * so an intermediate macro is added.
 * todo: (C++17 N4230) use nested namespace definition
 * \param _ns A list of namespaced enclosed by a pair of parentheses and seperated by comma.
 * \param _code The code to wrap in the inner most namespace enclosed by a pair of parentheses.
 */
#define USAGI_MAKE_NESTED_NAMESPACE(_ns_tuple, _code) \
    USAGI_MAKE_NESTED_NAMESPACE_CHECK(_ns_tuple, USAGI_TUPLE_SIZE(_ns_tuple), _code) \
/**/

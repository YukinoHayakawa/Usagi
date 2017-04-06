#pragma once

#include <boost/preprocessor/facilities/overload.hpp>
#include <boost/preprocessor/facilities/empty.hpp>

#include "unpack.hpp"

#define YUKI_MAKE_NESTED_NAMESPACE_1(_code) YUKI_UNPACK _code
#define YUKI_MAKE_NESTED_NAMESPACE_2(_code, _ns1) namespace _ns1 { YUKI_UNPACK _code }
#define YUKI_MAKE_NESTED_NAMESPACE_3(_code, _ns1, _ns2) namespace _ns1 { namespace _ns2 { YUKI_UNPACK _code }}
#define YUKI_MAKE_NESTED_NAMESPACE_4(_code, _ns1, _ns2, _ns3) namespace _ns1 { namespace _ns2 { namespace _ns3 { YUKI_UNPACK _code }}}
#define YUKI_MAKE_NESTED_NAMESPACE_5(_code, _ns1, _ns2, _ns3, _ns4) namespace _ns1 { namespace _ns2 { namespace _ns3 { namespace _ns4 { YUKI_UNPACK _code }}}}
#define YUKI_MAKE_NESTED_NAMESPACE_6(_code, _ns1, _ns2, _ns3, _ns4, _ns5) namespace _ns1 { namespace _ns2 { namespace _ns3 { namespace _ns4 { namespace _ns5 { YUKI_UNPACK _code }}}}}
#define YUKI_MAKE_NESTED_NAMESPACE_7(_code, _ns1, _ns2, _ns3, _ns4, _ns5, _ns6) namespace _ns1 { namespace _ns2 { namespace _ns3 { namespace _ns4 { namespace _ns5 { namespace _ns6 { YUKI_UNPACK _code }}}}}}
#define YUKI_MAKE_NESTED_NAMESPACE_8(_code, _ns1, _ns2, _ns3, _ns4, _ns5, _ns6, _ns7) namespace _ns1 { namespace _ns2 { namespace _ns3 { namespace _ns4 { namespace _ns5 { namespace _ns6 { namespace _ns7 { YUKI_UNPACK _code }}}}}}}
#define YUKI_MAKE_NESTED_NAMESPACE_9(_code, _ns1, _ns2, _ns3, _ns4, _ns5, _ns6, _ns7, _ns8) namespace _ns1 { namespace _ns2 { namespace _ns3 { namespace _ns4 { namespace _ns5 { namespace _ns6 { namespace _ns7 { namespace _ns8 { YUKI_UNPACK _code }}}}}}}}

#if !BOOST_PP_VARIADICS_MSVC
#   define YUKI_MAKE_NESTED_NAMESPACE_VA(...) \
        BOOST_PP_OVERLOAD(YUKI_MAKE_NESTED_NAMESPACE_, __VA_ARGS__)(__VA_ARGS__)
#else
#   define YUKI_MAKE_NESTED_NAMESPACE_VA(...) \
        BOOST_PP_CAT(BOOST_PP_OVERLOAD(YUKI_MAKE_NESTED_NAMESPACE_, __VA_ARGS__)(__VA_ARGS__), BOOST_PP_EMPTY())
#endif

/**
 * \brief Surround code with nested namespaces.
 * Directly expanding _ns with YUKI_UNPACK in the BOOST_PP_OVERLOAD macro generates error
 * so an intermediate macro is added.
 * todo: replaced by N4230
 * \param _ns A list of namespaced enclosed by a pair of parentheses and seperated by comma.
 * \param _code The code to wrap in the inner most namespace enclosed by a pair of parentheses.
 */
#define YUKI_MAKE_NESTED_NAMESPACE(_ns, _code) YUKI_MAKE_NESTED_NAMESPACE_VA(_code YUKI_TAIL_UNPACK _ns)

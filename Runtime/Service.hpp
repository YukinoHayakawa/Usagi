#pragma once

#include <type_traits>

// Cast the Runtime to the specified Service Implementation.
// The client this macro should however only rely assume an interface provided
// by the according Service Interface.
// #define ServiceInterface___ ServiceImpl
#define USAGI_SERVICE(rt, svc) \
    static_cast<svc##___&>(rt).get_service() \
/**/

// Option actions that only performed when the specified Service is present.
#define USAGI_OPT_SERVICE(rt, svc_type, svc_var, task) \
    do { \
        if constexpr(std::is_base_of_v< \
            svc_type, std::remove_cvref_t<decltype(rt)>>) \
        { \
            auto &svc_var = USAGI_SERVICE(rt, svc_type); \
            task \
        } \
    } while(false) \
/**/

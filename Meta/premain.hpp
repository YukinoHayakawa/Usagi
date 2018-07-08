#pragma once

#include <Usagi/Preprocessor/export.hpp>

namespace usagi
{

/**
 * \brief Use static member variable to execute codes before entering main.
 * \tparam Action A class, whose constructor will be called before main().
 */
template <typename Action>
struct usagi_EXPORT_DECL premain_action
{
    struct usagi_EXPORT_DECL action : Action
    {
    };

    static action premain_action_invoker;
};

template <typename Action>
typename premain_action<Action>::action premain_action<Action>::premain_action_invoker;

}

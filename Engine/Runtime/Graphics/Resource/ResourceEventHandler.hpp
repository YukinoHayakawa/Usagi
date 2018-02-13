#pragma once

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki::graphics
{
class ResourceEventHandler : Noncopyable
{
public:
    typedef std::uint64_t user_param_t;

    virtual ~ResourceEventHandler() = default;

    /**
     * \brief A previous operation invloving the resource is finished. If no
     * other operation refers to this resource, this resource is now idle and
     * can be safely modified or destroyed.
     */
    virtual void onResourceReleased(user_param_t user_param) { }

    /**
     * \brief The copying from host to device memory is done and the resource
     * is ready to use.
     */
    virtual void onResourceStreamed(user_param_t batch_index) { }
};
}

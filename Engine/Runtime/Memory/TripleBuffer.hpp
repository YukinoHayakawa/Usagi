#pragma once

#include <memory>

namespace yuki::memory
{
/**
 * \brief Buffer for asynchronize read and write at different speed.
 * Can be customized for both CPU and GPU usages.
 * 
 * Requirements:
 * No assumption on the operations supported by Buffer.
 * Support resource staging.
 * 
 * Invariants:
 * read() != write()
 * 
 * Considerations:
 * Updating status before & after each frame is too costly.
 * Synchronize implicitly?
 * 
 * todo Triple buffer should be enough for games, but consider extending to multiple buffers
 * 
 * References:
 * https://users.rust-lang.org/t/spmc-buffer-triple-buffering-for-multiple-consumers/10118/2
 * https://docs.unrealengine.com/latest/INT/API/Runtime/Core/Containers/TTripleBuffer/
 * 
 * Memory Management Startegies:
 * https://msdn.microsoft.com/en-us/library/windows/desktop/mt613239(v=vs.85).aspx
 * elements like gui must be copied before rendering
 * object streaming can be done at the background
 * https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples
 * 
 * \tparam Buffer 
 * \tparam FallbackPolicy 
 */
template <
    typename Buffer,
    typename WriteProxy,
    typename FallbackPolicy
>
class TripleBuffer
{
    std::shared_ptr<Buffer> mBuffers[3];
    uint8_t mBack = 0, mStaging = 1, mFront = 2;

public:
    std::shared_ptr<Buffer> & read()
    {
        return mBuffers[mFront];
    }

    Buffer & write()
    {
        if(mBuffers[mBack].count() )
        return *mBuffers[mBack].get();
    }

};
}

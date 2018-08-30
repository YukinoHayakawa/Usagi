#include "Expression.hpp"

namespace usagi::moeloop
{
std::string Expression::name()
{
    return {};
}

std::shared_ptr<GpuImage> Expression::texture() const
{
    return { };
}

AlignedBox2f Expression::textureUvRect() const
{
    return { };
}
}

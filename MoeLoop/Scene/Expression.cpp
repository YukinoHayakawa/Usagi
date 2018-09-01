#include "Expression.hpp"

namespace usagi::moeloop
{
Expression::Expression(Element *parent, std::string name)
    : Element(parent, std::move(name))
{
}

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

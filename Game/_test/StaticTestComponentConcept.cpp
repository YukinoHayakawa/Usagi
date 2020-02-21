#include <string>
#include <memory>
#include <vector>

#include <Usagi/Game/Entity/Component.hpp>

namespace usagi
{
namespace
{
// ============================================================================

struct Primitive
{
    int a;
    float b;
    char c[16] = "test";
};
static_assert(Component<Primitive> == true);

// ============================================================================

// struct Math
// {
//     Vector3f a { 0, 0, 0};
//     Matrix4f b;
// };
// static_assert(Component<Math> == true);

// ============================================================================

struct UniqueResource
{
    std::unique_ptr<int> a;
};
static_assert(Component<UniqueResource> == false);

// ============================================================================

struct SharedResource
{
    std::shared_ptr<int> a;
};
static_assert(Component<SharedResource> == false);

// ============================================================================

struct DynamicString
{
    std::string a;
};
static_assert(Component<DynamicString> == false);

// ============================================================================

struct DynamicContainer
{
    std::vector<int> a;
};
static_assert(Component<DynamicContainer> == false);

// ============================================================================
}
}

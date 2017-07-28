#include <Usagi/Preprocessor/identifier.hpp>

// empty namespace
YUKI_USE_NAMESPACE(())

namespace A
{
namespace B
{

constexpr int i = 5;

}
}

namespace
{

void foo()
{
    YUKI_USE_NAMESPACE(())
}

// use nested namespace in function
void bar()
{
    YUKI_USE_NAMESPACE((A, B))
    static_assert(i == 5, "");
}

}

// use nested namespace in another namespace
namespace use_ns_test
{

YUKI_USE_NAMESPACE((A, B))
static_assert(i == 5, "");

}

#include <Usagi/Runtime/Runtime.hpp>

#include "MoeLoop.hpp"

using namespace usagi;
using namespace moeloop;

int main()
{
    auto runtime = Runtime::create();
    MoeLoop app(runtime.get());
    app.mainLoop();

    return 0;
}

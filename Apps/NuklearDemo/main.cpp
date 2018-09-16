#include <Usagi/Runtime/Runtime.hpp>

#include "NuklearDemo.hpp"

int main(int argc, char *argv[])
{
    auto runtime = usagi::Runtime::create();
    runtime->enableCrashHandler("NuklearDemoErrorDump");
    try
    {
        usagi::NuklearDemo demo(runtime);
        demo.mainLoop();
    }
    catch(const std::exception &e)
    {
        runtime->displayErrorDialog(e.what());
        throw;
    }
}

#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Graphics/Game/GraphicalGame.hpp>
#include <Usagi/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Runtime/Runtime.hpp>

#include "ImGuiDemo.hpp"

using namespace usagi;

int main(int argc, char *argv[])
{
    auto runtime = Runtime::create();
    runtime->enableCrashHandler("ImGuiDemoErrorDump");
    try
    {
        ImGuiDemo demo(runtime);
        demo.mainLoop();
    }
    catch(const std::exception &e)
    {
        runtime->displayErrorDialog(e.what());
        throw;
    }
}

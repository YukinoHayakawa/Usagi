#include <Usagi/Engine/Extension/Win32/Win32Window.hpp>

using namespace yuki;

int main(int argc, char *argv[])
{
    Win32Window window { "Spiral Galaxy Density Wave Simulation", 1280, 1280 };
    while(window.isWindowOpen())
    {
		window.processEvents();
    }
}

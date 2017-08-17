#include <Usagi/Engine/Time/Clock.hpp>
#include <Usagi/Engine/Runtime/Subsystem/DebugDrawManager.hpp>
#include <Usagi/Engine/Extension/Win32/WGLContext.hpp>
#include <Usagi/Engine/Extension/OpenGL/OpenGLGraphicsDevice.hpp>
#include <Usagi/Engine/Entity/Controller/NoClipEntityController.hpp>
#include <Usagi/Engine/Camera/PerspectiveCamera.hpp>
// dirty one last
#include <Usagi/Engine/Extension/Win32/Win32Window.hpp>
#include <Usagi/Engine/Debugging/DebugKeyMouseEventListener.hpp>

static const int windowWidth = 1280;
static const int windowHeight = 720;

int main(int argc, char *argv[])
{
    using namespace yuki;

    try
    {
        auto window = std::make_shared<Win32Window>("UsagiEditor", 1280, 720);
        auto device = std::make_shared<OpenGLGraphicsDevice>(std::make_shared<WGLContext>(window->getDeviceContext()));

        // setup camera
        auto camera = std::make_shared<PerspectiveCamera>();
        auto fovY = Degrees(60.f).toRadians();
        // todo derive from window size listen to resize events
        const float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
        camera->setPerspective(fovY, aspect, 0.1f, 1000.0f);
        camera->lookAt({ 10, 10, 10 }, { }, Eigen::Vector3f::UnitY());
        auto camman = std::make_shared<NoClipEntityController>(window, window);
        camman->attachTo(camera);
        window->addMouseEventListener(camman);
        window->addKeyEventListener(camman);

        auto inputdebug = std::make_shared<DebugKeyMouseEventListener>();
        window->addKeyEventListener(inputdebug);
        //window->addMouseEventListener(inputdebug);


        DebugDrawManager debug_draw_manager(device, camera);

        window->showWindow(true);
        device->setContextCurrent();

        Clock master_clock;
        while(true)
        {
            master_clock.tick();

            camman->tickUpdate(master_clock);

            window->processEvents();

            device->clearCurrentFrameBuffer();
            debug_draw_manager.render(master_clock);

            device->swapFrameBuffers();
        }
    }
    catch(std::exception &)
    {
        throw;
    }
}

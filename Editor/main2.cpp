#include <Usagi/Engine/Time/Clock.hpp>
#include <Usagi/Engine/Runtime/Subsystem/DebugDrawManager.hpp>
#include <Usagi/Engine/Extension/Win32/WGLContext.hpp>
#include <Usagi/Engine/Extension/OpenGL/OpenGLGraphicsDevice.hpp>
#include <Usagi/Engine/Component/DynamicComponent.hpp>
#include <Usagi/Engine/Component/TimeVariantComponent.hpp>
#include <Usagi/Engine/Entity/Controller/NoClipEntityController.hpp>
#include <Usagi/Engine/Debugging/DebugKeyMouseEventListener.hpp>
// dirty one last
#include <Usagi/Engine/Extension/Win32/Win32Window.hpp>

static inline float degToRad(const float ang)
{
    return ang * 3.1415926535897931f / 180.0f;
}

static const int windowWidth = 1280;
static const int windowHeight = 720;

Eigen::Affine3f gDdVpMatrix;

class SimpleCamera : public yuki::DynamicComponent, public yuki::TimeVariantComponent
{
public:
    Eigen::Projective3f mLocalToNdcProjectionMatrix;

    void lookAt(const Eigen::Vector3f& position, const Eigen::Vector3f& target, const Eigen::Vector3f& up)
    {
        Eigen::Matrix3f localToWorld;
        localToWorld.col(2) = (position - target).normalized(); // z
        localToWorld.col(0) = up.cross(localToWorld.col(2)).normalized(); // x
        localToWorld.col(1) = localToWorld.col(2).cross(localToWorld.col(0)); // y
        setOrientation(localToWorld);
        setPosition(position);
    }

    void setPerspective(float fovY, float aspect, float near, float far)
    {
        float theta = fovY*0.5;
        float range = far - near;
        float invtan = 1. / tan(theta);

        mLocalToNdcProjectionMatrix(0, 0) = invtan / aspect;
        mLocalToNdcProjectionMatrix(1, 1) = invtan;
        mLocalToNdcProjectionMatrix(2, 2) = -(near + far) / range;
        mLocalToNdcProjectionMatrix(3, 2) = -1;
        mLocalToNdcProjectionMatrix(2, 3) = -2 * near * far / range;
        mLocalToNdcProjectionMatrix(3, 3) = 0;
    }

    void tickUpdate(const yuki::Clock &clock) override
    {
        gDdVpMatrix = mLocalToNdcProjectionMatrix * getLocalToWorldTransform().matrix().inverse();
    }
};

int main(int argc, char *argv[])
{
    using namespace yuki;

    try
    {
        auto window = std::make_shared<Win32Window>("UsagiEditor", 1280, 720);
        auto device = std::make_shared<OpenGLGraphicsDevice>(std::make_shared<WGLContext>(window->getDeviceContext()));
        DebugDrawManager debug_draw_manager(*device.get());
        Clock master_clock;


        // todo remove
        auto input_handler = std::make_shared<DebugKeyMouseEventListener>();
        auto cam = std::make_shared<SimpleCamera>();
        const float fovY = degToRad(60.0f);
        const float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
        cam->setPerspective(fovY, aspect, 0.1f, 1000.0f);
        cam->lookAt({ 10, 10, 10 }, { }, Eigen::Vector3f::UnitY());

        auto camman = std::make_shared<NoClipEntityController>(window, window);
        camman->attachTo(cam);

        window->addMouseEventListener(camman);
        window->addKeyEventListener(camman);

        window->addMouseEventListener(input_handler);
        window->addKeyEventListener(input_handler);
        window->show();
        device->setContextCurrent();

        while(true)
        {
            master_clock.tick();

            camman->tickUpdate(master_clock);
            cam->tickUpdate(master_clock);

            window->processEvents();

            device->clearCurrentFrameBuffer();
            debug_draw_manager.render(*device.get(), master_clock);

            device->swapFrameBuffers();
        }
    }
    catch(std::exception &)
    {
        throw;
    }
}

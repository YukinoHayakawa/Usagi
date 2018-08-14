#pragma once

#include <memory>
#include <set>

#include <Usagi/Engine/Runtime/Graphics/RenderableSubsystem.hpp>
#include <Usagi/Engine/Runtime/Input/Keyboard/KeyEventListener.hpp>
#include <Usagi/Engine/Runtime/Input/Mouse/MouseEventListener.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuImage.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuSampler.hpp>

struct ImGuiContext;

namespace usagi
{
struct RenderPassCreateInfo;
class RenderPass;
class Window;
class Game;
class GpuCommandPool;
class GpuBuffer;
class GraphicsPipeline;
class Mouse;
struct ImGuiComponent;

class ImGuiSubsystem
    : public RenderableSubsystem
    , public KeyEventListener
    , public MouseEventListener
{
    Game *mGame = nullptr;
    Window *mWindow = nullptr;
    Mouse *mMouse = nullptr;

    ImGuiContext *mContext = nullptr;
    bool mMouseJustPressed[5] = { };

    std::set<Element*> mRegistry;

    static void setupInput();

    void newFrame(float dt, Framebuffer *framebuffer);
    void processElements(const TimeDuration &dt);
    void render(
        const std::shared_ptr<Framebuffer> &framebuffer,
        const std::function<void(std::shared_ptr<GraphicsCommandList>)> &cmd_out
    ) const;

    void updateMouse();

	std::shared_ptr<GraphicsPipeline> mPipeline;
	std::shared_ptr<GpuCommandPool> mCommandPool;
	std::shared_ptr<GpuBuffer> mVertexBuffer;
	std::shared_ptr<GpuBuffer> mIndexBuffer;
    std::shared_ptr<GpuImage> mFontTexture;
    std::shared_ptr<GpuSampler> mSampler;

public:
    ImGuiSubsystem(Game *game, Window *window, Mouse *mouse);
    ~ImGuiSubsystem() override;

    /**
     * \brief Public to allow pipeline recreation.
     * \param render_pass_info An array of attachment usages. The usages must
     * include all the attachments required by the subsystem. The subsystem
     * sets the layout and load/store operations and use them to create the
     * render pass.
     */
    void createPipeline(RenderPassCreateInfo render_pass_info);

    void onKeyStateChange(const KeyEvent &e) override;
    void onMouseButtonStateChange(const MouseButtonEvent &e) override;
    void onMouseWheelScroll(const MouseWheelEvent &e) override;

    void update(
        const TimeDuration &dt,
        std::shared_ptr<Framebuffer> framebuffer,
        const std::function<void(std::shared_ptr<GraphicsCommandList>)> &cmd_out
    ) override;

    bool processable(Element *element) override;
    void updateRegistry(Element *element) override;
};
}

#pragma once

#include <memory>
#include <set>

#include <Usagi/Graphics/RenderableSubsystem.hpp>
#include <Usagi/Runtime/Input/Keyboard/KeyEventListener.hpp>
#include <Usagi/Runtime/Input/Mouse/MouseEventListener.hpp>
#include <Usagi/Runtime/Graphics/GpuImage.hpp>
#include <Usagi/Runtime/Graphics/GpuSampler.hpp>
#include <Usagi/Runtime/Window/WindowEventListener.hpp>

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
    , public WindowEventListener
{
    Game *mGame = nullptr;
    std::shared_ptr<Window> mWindow;
    std::shared_ptr<Keyboard> mKeyboard;
    std::shared_ptr<Mouse> mMouse;

    ImGuiContext *mContext = nullptr;
    bool mMouseJustPressed[5] = { };
    mutable Vector2f mLastFrameBufferSize;

    std::set<Element*> mRegistry;

    void setupInput();

    void newFrame(float dt);
    void processElements(const TimeDuration &dt);
    void render(
        const std::shared_ptr<Framebuffer> &framebuffer,
        const CommandListSink &cmd_out
    ) const;

    void updateMouse();

	std::shared_ptr<GraphicsPipeline> mPipeline;
	std::shared_ptr<GpuCommandPool> mCommandPool;
    std::shared_ptr<RenderPass> mRenderPass;
    std::shared_ptr<GpuBuffer> mVertexBuffer;
	std::shared_ptr<GpuBuffer> mIndexBuffer;
    std::shared_ptr<GpuImage> mFontTexture;
    std::shared_ptr<GpuImageView> mFontTextureView;
    std::shared_ptr<GpuSampler> mSampler;

    void setup();

public:
    ImGuiSubsystem(
        Game *game,
        std::shared_ptr<Window> window,
        std::shared_ptr<Keyboard> keyboard,
        std::shared_ptr<Mouse> mouse);
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
    void onWindowCharInput(const WindowCharEvent &e) override;

    void update(const TimeDuration &dt) override;
    void render(
        const TimeDuration &dt,
        std::shared_ptr<Framebuffer> framebuffer,
        const CommandListSink &cmd_out) const override;

    bool processable(Element *element) override;
    void updateRegistry(Element *element) override;
};
}

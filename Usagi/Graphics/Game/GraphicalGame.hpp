﻿#pragma once

#include <Usagi/Game/Game.hpp>
#include <Usagi/Graphics/RenderTarget/RenderTargetProvider.hpp>
#include <Usagi/Graphics/RenderWindow.hpp>
#include <Usagi/Runtime/Window/WindowEventListener.hpp>
#include <Usagi/Runtime/Graphics/Enum/GpuBufferFormat.hpp>

#include "ImageTransitionSystem.hpp"

namespace usagi
{
/**
 * \brief Provides the following functionalities:
 *
 * Initialize the window, input, GPU systems of runtime.
 * A single window with a swapchain.
 * Auto-resize swapchain upon window resize.
 */
class GraphicalGame
    : public Game
    , public RenderTargetProvider
    , public WindowEventListener
{
protected:
    RenderWindow mMainWindow;
    std::unique_ptr<ImageTransitionSystem> mPreRender;
    std::unique_ptr<ImageTransitionSystem> mPostRender;
    std::vector<std::shared_ptr<GraphicsCommandList>> mPendingJobs;
    bool mPaused = false;

    void createMainWindow(
        const std::string &window_title,
        const Vector2u32 &window_size,
        GpuBufferFormat swapchain_format = GpuBufferFormat::R8G8B8A8_UNORM);
    void setupRenderTargets(bool depth);

    bool continueGame() const override;
    void render();
    void frame() override;

public:
    explicit GraphicalGame(std::shared_ptr<Runtime> runtime);

    /**
     * \brief The content of the vector will be removed.
     * \param jobs
     */
    void submitGraphicsJobs(
        std::vector<std::shared_ptr<GraphicsCommandList>> &jobs);
    GpuDevice * gpu() const override;
    void onWindowResizeEnd(const WindowSizeEvent &e) override;
    void onWindowMinimized(const WindowSizeEvent &e) override;
    void onWindowRestored(const WindowSizeEvent &e) override;

    RenderWindow * mainWindow() { return &mMainWindow; }
};
}

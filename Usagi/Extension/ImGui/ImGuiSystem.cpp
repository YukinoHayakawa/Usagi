#include "ImGuiSystem.hpp"

#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Converter/SpirvAssetConverter.hpp>
#include <Usagi/Core/Clock.hpp>
#include <Usagi/Core/Logging.hpp>
#include <Usagi/Game/Game.hpp>
#include <Usagi/Graphics/RenderTarget/RenderTarget.hpp>
#include <Usagi/Graphics/RenderTarget/RenderTargetDescriptor.hpp>
#include <Usagi/Runtime/Graphics/Enum/GraphicsIndexType.hpp>
#include <Usagi/Runtime/Graphics/GpuBuffer.hpp>
#include <Usagi/Runtime/Graphics/GpuCommandPool.hpp>
#include <Usagi/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Runtime/Graphics/GpuImage.hpp>
#include <Usagi/Runtime/Graphics/GpuImageCreateInfo.hpp>
#include <Usagi/Runtime/Graphics/GpuImageView.hpp>
#include <Usagi/Runtime/Graphics/GpuImageViewCreateInfo.hpp>
#include <Usagi/Runtime/Graphics/GpuSampler.hpp>
#include <Usagi/Runtime/Graphics/GpuSamplerCreateInfo.hpp>
#include <Usagi/Runtime/Graphics/GraphicsCommandList.hpp>
#include <Usagi/Runtime/Graphics/GraphicsPipelineCompiler.hpp>
#include <Usagi/Runtime/Input/Keyboard/Keyboard.hpp>
#include <Usagi/Runtime/Input/Mouse/Mouse.hpp>
#include <Usagi/Runtime/Runtime.hpp>
#include <Usagi/Runtime/Window/Window.hpp>

#include "ImGui.hpp"

void usagi::ImGuiSystem::setup()
{
    mContext = ImGui::CreateContext();
    auto &io = ImGui::GetIO();

    // input
    setupInput();

    // style
    const auto scale = mWindow->dpiScale().x();
    ImGui::StyleColorsLight();
    auto &style = ImGui::GetStyle();
    style.FrameBorderSize = 1.f;
    style.WindowRounding = 0;
    style.ChildRounding = 0;
    style.PopupRounding = 0;
    style.FrameRounding = 0;
    style.ScrollbarRounding = 0;
    style.GrabRounding = 0;
    auto *colors = style.Colors;
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.78f, 0.78f, 0.78f, 0.56f);
    style.ScaleAllSizes(scale);

    // resources
    auto gpu = mGame->runtime()->gpu();
    mVertexBuffer = gpu->createBuffer(GpuBufferUsage::VERTEX);
    mIndexBuffer = gpu->createBuffer(GpuBufferUsage::INDEX);

    mCommandPool = gpu->createCommandPool();

    // fonts
    {
        LOG(info, "ImGui: Start building font atlas");

        const auto size = 12 * scale;
        // todo fetch from asset system
        const auto file = "Data/fonts/Microsoft-Yahei-Mono.ttf";

        // add fonts
        // http://jrgraphix.net/research/unicode_blocks.php
        static const ImWchar RANGES[] =
        {
            0x0020, 0x007F, // Basic Latin
            0x00A0, 0x00FF, // Latin-1 Supplement
            0x2000, 0x206F, // General Punctuation
            0x3000, 0x303F, // CJK Symbols and Punctuation
            0x3040, 0x309F, // Hiragana
            0x30A0, 0x30FF, // Katakana
            0x31F0, 0x31FF, // Katakana Phonetic Extensions
            0x4E00, 0x9FFF, // CJK Unified Ideographs
            0xFF00, 0xFFEF, // Halfwidth and Fullwidth Forms
            0,
        };
        io.Fonts->AddFontFromFileTTF(file, size, nullptr, RANGES)
            ->DisplayOffset.y = scale;

        // upload font texture
        unsigned char* pixels;
        int width, height, bytes_per_pixel;
        io.Fonts->GetTexDataAsAlpha8(
            &pixels, &width, &height, &bytes_per_pixel);
        const auto upload_size =
            width * height * bytes_per_pixel * sizeof(char);
        GpuImageCreateInfo info;
        info.format = GpuBufferFormat::R8_UNORM;
        info.size = { width, height };
        info.usage = GpuImageUsage::SAMPLED;
        mFontTexture = gpu->createImage(info);
        mFontTexture->upload(pixels, upload_size);

        LOG(info, "ImGui: Finish building font atlas");
    }
    {
        GpuImageViewCreateInfo info;
        info.components.r = GpuImageComponentSwizzle::ONE;
        info.components.g = GpuImageComponentSwizzle::ONE;
        info.components.b = GpuImageComponentSwizzle::ONE;
        info.components.a = GpuImageComponentSwizzle::R;
        mFontTextureView = mFontTexture->createView(info);
        io.Fonts->TexID = mFontTextureView.get();
    }
    {
        GpuSamplerCreateInfo info;
        info.min_filter = GpuFilter::LINEAR;
        info.mag_filter = GpuFilter::NEAREST;
        info.addressing_mode_u = GpuSamplerAddressMode::REPEAT;
        info.addressing_mode_v = GpuSamplerAddressMode::REPEAT;
        mSampler = gpu->createSampler(info);
    }
}

usagi::ImGuiSystem::ImGuiSystem(
    Game *game,
    std::shared_ptr<Window> window,
    std::shared_ptr<Keyboard> keyboard,
    std::shared_ptr<Mouse> mouse)
    : mGame(game)
    , mWindow(std::move(window))
    , mKeyboard(std::move(keyboard))
    , mMouse(std::move(mouse))
{
    setup();

    mWindow->addEventListener(this);
    mKeyboard->addEventListener(this);
    mMouse->addEventListener(this);
}

usagi::ImGuiSystem::~ImGuiSystem()
{
    ImGui::SetCurrentContext(mContext);
    ImGui::DestroyContext();
}

void usagi::ImGuiSystem::createRenderTarget(
    RenderTargetDescriptor &descriptor)
{
    descriptor.sharedColorTarget("imgui");
    mRenderTarget = descriptor.finish();
}

void usagi::ImGuiSystem::createPipelines()
{
    auto gpu = mGame->runtime()->gpu();
    auto assets = mGame->assets();
    auto compiler = gpu->createPipelineCompiler();

    // Shaders
    {
        compiler->setShader(ShaderStage::VERTEX,
            assets->res<SpirvAssetConverter>(
                "imgui:shaders/glsl_shader.vert", ShaderStage::VERTEX)
        );
        compiler->setShader(ShaderStage::FRAGMENT,
            assets->res<SpirvAssetConverter>(
                "imgui:shaders/glsl_shader.frag", ShaderStage::FRAGMENT)
        );
    }
    // Vertex Inputs
    {
        compiler->setVertexBufferBinding(0, sizeof(ImDrawVert));

        compiler->setVertexAttribute(
            "aPos", 0,
            offsetof(ImDrawVert, pos), GpuBufferFormat::R32G32_SFLOAT
        );
        compiler->setVertexAttribute(
            "aUV", 0,
            offsetof(ImDrawVert, uv), GpuBufferFormat::R32G32_SFLOAT
        );
        compiler->setVertexAttribute(
            "aColor", 0,
            offsetof(ImDrawVert, col), GpuBufferFormat::R8G8B8A8_UNORM
        );
    }
    // Rasterization
    {
        RasterizationState state;
        state.face_culling_mode = FaceCullingMode::NONE;
        compiler->setRasterizationState(state);
    }
    // Blending
    {
        ColorBlendState state;
        state.enable = true;
        state.setBlendingFactor(
            BlendingFactor::SOURCE_ALPHA,
            BlendingFactor::ONE_MINUS_SOURCE_ALPHA);
        state.setBlendingOperation(BlendingOperation::ADD);
        compiler->setColorBlendState(state);
    }
    compiler->setRenderPass(mRenderTarget->renderPass());

    mPipeline = compiler->compile();
}

void usagi::ImGuiSystem::setupInput()
{
    // Setup back-end capabilities flags
    auto &io = ImGui::GetIO();

    // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
    io.KeyMap[ImGuiKey_Tab] = static_cast<int>(KeyCode::TAB);
    io.KeyMap[ImGuiKey_LeftArrow] = static_cast<int>(KeyCode::LEFT);
    io.KeyMap[ImGuiKey_RightArrow] = static_cast<int>(KeyCode::RIGHT);
    io.KeyMap[ImGuiKey_UpArrow] = static_cast<int>(KeyCode::UP);
    io.KeyMap[ImGuiKey_DownArrow] = static_cast<int>(KeyCode::DOWN);
    io.KeyMap[ImGuiKey_PageUp] = static_cast<int>(KeyCode::PAGE_UP);
    io.KeyMap[ImGuiKey_PageDown] = static_cast<int>(KeyCode::PAGE_DOWN);
    io.KeyMap[ImGuiKey_Home] = static_cast<int>(KeyCode::HOME);
    io.KeyMap[ImGuiKey_End] = static_cast<int>(KeyCode::END);
    io.KeyMap[ImGuiKey_Insert] = static_cast<int>(KeyCode::INSERT);
    io.KeyMap[ImGuiKey_Delete] = static_cast<int>(KeyCode::DELETE);
    io.KeyMap[ImGuiKey_Backspace] = static_cast<int>(KeyCode::BACKSPACE);
    io.KeyMap[ImGuiKey_Space] = static_cast<int>(KeyCode::SPACE);
    io.KeyMap[ImGuiKey_Enter] = static_cast<int>(KeyCode::ENTER);
    io.KeyMap[ImGuiKey_Escape] = static_cast<int>(KeyCode::ESCAPE);
    io.KeyMap[ImGuiKey_A] = static_cast<int>(KeyCode::A);
    io.KeyMap[ImGuiKey_C] = static_cast<int>(KeyCode::C);
    io.KeyMap[ImGuiKey_V] = static_cast<int>(KeyCode::V);
    io.KeyMap[ImGuiKey_X] = static_cast<int>(KeyCode::X);
    io.KeyMap[ImGuiKey_Y] = static_cast<int>(KeyCode::Y);
    io.KeyMap[ImGuiKey_Z] = static_cast<int>(KeyCode::Z);

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    io.GetClipboardTextFn = [](void *window) {
        static std::string text;
        text = static_cast<Window*>(window)->getClipboardText();
        return text.c_str();
    };
    io.SetClipboardTextFn = [](void *window, const char* text) {
        return static_cast<Window*>(window)->setClipboardText(text);
    };
    io.ClipboardUserData = mWindow.get();
}

void usagi::ImGuiSystem::update(const Clock &clock)
{
    ImGui::SetCurrentContext(mContext);

    newFrame(static_cast<float>(clock.elapsed()));
    processElements(clock);
}

void usagi::ImGuiSystem::newFrame(const float dt)
{
    auto &io = ImGui::GetIO();

    // Font atlas needs to be built, call renderer _NewFrame() function
    // e.g. ImGui_ImplOpenGL3_NewFrame()
    IM_ASSERT(io.Fonts->IsBuilt());

    // Setup display size
    const Vector2f ws = mWindowSizeFunc();
    const Vector2f fs = mFrameBufferSizeFunc();
    // FrameBufferSize ./ WindowSize
    const Vector2f scale = mFrameBufferSizeFunc().cwiseQuotient(ws);

    io.DisplaySize = { fs.x(), fs.y() };
    io.DisplayFramebufferScale = { scale.x(), scale.y() };

    // Setup time step
    io.DeltaTime = dt;

    updateMouse();

    // todo: update Gamepad

    ImGui::NewFrame();
}

void usagi::ImGuiSystem::updateMouse()
{
    auto &io = ImGui::GetIO();

    // Update buttons
    for(auto i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
    {
        // If a mouse press event came, always pass it as
        // "mouse held this frame", so we don't miss click-release events
        // that are shorter than 1 frame.
        io.MouseDown[i] = mMouseJustPressed[i] ||
            mMouse->isButtonPressed(static_cast<MouseButtonCode>(i));
        mMouseJustPressed[i] = false;
    }

    // Update mouse position
    if(mWindow->focused())
    {
        if(io.WantSetMousePos)
        {
            // todo: set pos
        }
        else
        {
            const auto p = mMouse->cursorPositionInActiveWindow();
            io.MousePos = ImVec2(p.x(), p.y());
        }
    }

    // todo: update cursor
}

void usagi::ImGuiSystem::processElements(const Clock &clock)
{
    for(auto &e : mRegistry)
        std::get<ImGuiComponent*>(e.second)->draw(clock);
}

std::shared_ptr<usagi::GraphicsCommandList> usagi::ImGuiSystem::render(
    const Clock &clock)
{
    ImGui::Render();

    const auto draw_data = ImGui::GetDrawData();
    if(draw_data->TotalVtxCount == 0)
        return { };

    // Upload Vertex and index Data
    {
        const auto vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
        const auto index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);
        mVertexBuffer->allocate(vertex_size);
        mIndexBuffer->allocate(index_size);
        auto vtx_dst = mVertexBuffer->mappedMemory<ImDrawVert>();
        auto idx_dst = mIndexBuffer->mappedMemory<ImDrawIdx>();
        for(auto n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList *im_draw_list = draw_data->CmdLists[n];
            memcpy(vtx_dst, im_draw_list->VtxBuffer.Data,
                im_draw_list->VtxBuffer.Size * sizeof(ImDrawVert));
            memcpy(idx_dst, im_draw_list->IdxBuffer.Data,
                im_draw_list->IdxBuffer.Size * sizeof(ImDrawIdx));
            vtx_dst += im_draw_list->VtxBuffer.Size;
            idx_dst += im_draw_list->IdxBuffer.Size;
        }
        mVertexBuffer->flush();
        mIndexBuffer->flush();
    }

    // Render Command List
    auto cmd_list = mCommandPool->allocateGraphicsCommandList();
    cmd_list->beginRecording();
    cmd_list->beginRendering(
        mRenderTarget->renderPass(),
        mRenderTarget->createFramebuffer()
    );
    cmd_list->bindPipeline(mPipeline);
    cmd_list->bindResourceSet(0, { mSampler });

    // Bind Vertex And Index Buffer
    {
        cmd_list->bindVertexBuffer(0, mVertexBuffer, 0);
        cmd_list->bindIndexBuffer(mIndexBuffer, 0,
            GraphicsIndexType::UINT16
        );
        mVertexBuffer->release();
        mIndexBuffer->release();
    }

    // Setup viewport
    {
        cmd_list->setViewport(
            0,
            { 0, 0 },
            { draw_data->DisplaySize.x, draw_data->DisplaySize.y }
        );
    }

    // Setup scale and translation
    {
        float scale[2];
        scale[0] = 2.0f / draw_data->DisplaySize.x;
        scale[1] = 2.0f / draw_data->DisplaySize.y;
        float translate[2];
        translate[0] = -1.0f - draw_data->DisplayPos.x * scale[0];
        translate[1] = -1.0f - draw_data->DisplayPos.y * scale[1];
        cmd_list->setConstant(ShaderStage::VERTEX,
            "uScale", scale, sizeof(scale));
        cmd_list->setConstant(ShaderStage::VERTEX,
            "uTranslate", translate, sizeof(translate));
    }

    // Render the command lists
    auto vtx_offset = 0;
    auto idx_offset = 0;
    const auto display_pos = draw_data->DisplayPos;
    for(auto n = 0; n < draw_data->CmdListsCount; n++)
    {
        auto im_cmd_list = draw_data->CmdLists[n];
        for(auto cmd_i = 0; cmd_i < im_cmd_list->CmdBuffer.Size; ++cmd_i)
        {
            const auto pcmd = &im_cmd_list->CmdBuffer[cmd_i];
            if(pcmd->UserCallback)
            {
                pcmd->UserCallback(im_cmd_list, pcmd);
            }
            else
            {
                Vector2i32 origin = {
                    pcmd->ClipRect.x - display_pos.x,
                    pcmd->ClipRect.y - display_pos.y
                };
                origin = origin.cwiseMax(0);
                const Vector2u32 size = {
                    pcmd->ClipRect.z - pcmd->ClipRect.x,
                    pcmd->ClipRect.w - pcmd->ClipRect.y
                };
                cmd_list->setScissor(0, origin, size);

                cmd_list->bindResourceSet(1, {
                    pcmd->TextureId
                        ? pcmd->TextureId->shared_from_this()
                        : mFontTextureView
                });

                cmd_list->drawIndexedInstanced(
                    pcmd->ElemCount, // index count
                    1,               // instance count
                    idx_offset,      // first index
                    vtx_offset,      // vertex offset
                    0                // first instance
                );
            }
            idx_offset += pcmd->ElemCount;
        }
        vtx_offset += im_cmd_list->VtxBuffer.Size;
    }

    cmd_list->endRendering();
    cmd_list->endRecording();

    return std::move(cmd_list);
}

bool usagi::ImGuiSystem::onKeyStateChange(const KeyEvent &e)
{
    auto &io = ImGui::GetIO();

    io.KeysDown[static_cast<std::size_t>(e.key_code)] = e.pressed;

    io.KeyCtrl = io.KeysDown[static_cast<std::size_t>(KeyCode::LEFT_CONTROL)]
        || io.KeysDown[static_cast<std::size_t>(KeyCode::RIGHT_CONTROL)];
    io.KeyShift = io.KeysDown[static_cast<std::size_t>(KeyCode::LEFT_SHIFT)]
        || io.KeysDown[static_cast<std::size_t>(KeyCode::RIGHT_SHIFT)];
    io.KeyAlt = io.KeysDown[static_cast<std::size_t>(KeyCode::LEFT_ALT)]
        || io.KeysDown[static_cast<std::size_t>(KeyCode::RIGHT_ALT)];
    io.KeySuper = io.KeysDown[static_cast<std::size_t>(KeyCode::LEFT_OS)]
        || io.KeysDown[static_cast<std::size_t>(KeyCode::RIGHT_OS)];

    return true;
}

bool usagi::ImGuiSystem::onMouseButtonStateChange(
    const MouseButtonEvent &e)
{
    if(e.pressed)
        mMouseJustPressed[static_cast<std::size_t>(e.button)] = true;

    return true;
}

bool usagi::ImGuiSystem::onMouseWheelScroll(const MouseWheelEvent &e)
{
    auto &io = ImGui::GetIO();

    io.MouseWheelH += e.distance.x();
    io.MouseWheel += e.distance.y();

    return true;
}

void usagi::ImGuiSystem::onWindowCharInput(const WindowCharEvent &e)
{
    auto &io = ImGui::GetIO();

    io.AddInputCharacter(e.utf16);
}

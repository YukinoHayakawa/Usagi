#include "ImGuiSubsystem.hpp"

#include <Usagi/Engine/Asset/AssetRoot.hpp>
#include <Usagi/Engine/Game/Game.hpp>
#include <Usagi/Engine/Runtime/Graphics/Enum/GraphicsIndexType.hpp>
#include <Usagi/Engine/Runtime/Graphics/GpuDevice.hpp>
#include <Usagi/Engine/Runtime/Graphics/GraphicsPipelineCompiler.hpp>
#include <Usagi/Engine/Runtime/Graphics/RenderPassCreateInfo.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/Framebuffer.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuBuffer.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuCommandPool.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuImageCreateInfo.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuImageView.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuImageViewCreateInfo.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GpuSamplerCreateInfo.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/GraphicsCommandList.hpp>
#include <Usagi/Engine/Runtime/Input/Mouse/Mouse.hpp>
#include <Usagi/Engine/Runtime/Runtime.hpp>
#include <Usagi/Engine/Runtime/Window/Window.hpp>

#include "ImGui.hpp"
#include "ImGuiComponent.hpp"

usagi::ImGuiSubsystem::ImGuiSubsystem(
    Game *game,
    Window *window,
    Mouse *mouse)
    : mGame(game)
    , mWindow(window)
    , mMouse(mouse)
{
    mContext = ImGui::CreateContext();
    auto &io = ImGui::GetIO();

    ImGui::StyleColorsDark();
    // todo hidpi
    //ImGui::GetStyle().ScaleAllSizes(mWindow->dpiScale().x());

    setupInput();

    auto gpu = mGame->runtime()->gpu();
    mVertexBuffer = gpu->createBuffer(GpuBufferUsage::VERTEX);
    mIndexBuffer = gpu->createBuffer(GpuBufferUsage::INDEX);
    mCommandPool = gpu->createCommandPool();
    {
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

usagi::ImGuiSubsystem::~ImGuiSubsystem()
{
    ImGui::SetCurrentContext(mContext);
    ImGui::DestroyContext();
}

void usagi::ImGuiSubsystem::createPipeline(
    RenderPassCreateInfo render_pass_info)
{
    auto gpu = mGame->runtime()->gpu();
    auto assets = mGame->assets();
    auto compiler = gpu->createPipelineCompiler();

    // Shaders
    {
        compiler->setShader(ShaderStage::VERTEX,
            assets->find<SpirvBinary>("imgui:shaders/glsl_shader.vert")
        );
        compiler->setShader(ShaderStage::FRAGMENT,
            assets->find<SpirvBinary>("imgui:shaders/glsl_shader.frag")
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
        state.src_color_factor = BlendingFactor::SOURCE_ALPHA;
        state.dst_color_factor = BlendingFactor::ONE_MINUS_SOURCE_ALPHA;
        state.color_blend_op = BlendingOperation::ADD;
        state.src_alpha_factor = BlendingFactor::SOURCE_ALPHA;
        state.dst_alpha_factor = BlendingFactor::ONE_MINUS_SOURCE_ALPHA;
        state.alpha_blend_op = BlendingOperation::ADD;
        compiler->setColorBlendState(state);
    }
    // Render Pass
    {
        render_pass_info.attachment_usages[0].layout = GpuImageLayout::COLOR;
        compiler->setRenderPass(gpu->createRenderPass(render_pass_info));
    }

    mPipeline = compiler->compile();
}

void usagi::ImGuiSubsystem::setupInput()
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
}

void usagi::ImGuiSubsystem::update(
    const TimeDuration &dt,
    std::shared_ptr<Framebuffer> framebuffer,
    const std::function<void(std::shared_ptr<GraphicsCommandList>)> &cmd_out)
{
    ImGui::SetCurrentContext(mContext);

    // Input system must be updated previously.

    newFrame(static_cast<float>(dt.count()), framebuffer.get());
    processElements(dt);
    render(framebuffer, cmd_out);
}

void usagi::ImGuiSubsystem::newFrame(const float dt, Framebuffer *framebuffer)
{
    auto &io = ImGui::GetIO();

    // Font atlas needs to be built, call renderer _NewFrame() function
    // e.g. ImGui_ImplOpenGL3_NewFrame()
    IM_ASSERT(io.Fonts->IsBuilt());

    // Setup display size
    const auto d = mWindow->size().cast<float>();
    const auto f = framebuffer->size().cast<float>();
    const auto s = f.cwiseQuotient(d); // f / d

    io.DisplaySize = { d.x(), d.y() };
    io.DisplayFramebufferScale = { s.x(), s.y() };

    // Setup time step
    io.DeltaTime = dt;

    updateMouse();

    // todo: Gamepad

    ImGui::NewFrame();
}

void usagi::ImGuiSubsystem::updateMouse()
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

void usagi::ImGuiSubsystem::processElements(const TimeDuration &dt)
{
    for(auto &e : mRegistry)
        e->getComponent<ImGuiComponent>()->draw(dt);
}

void usagi::ImGuiSubsystem::render(
    const std::shared_ptr<Framebuffer> &framebuffer,
    const std::function<void(std::shared_ptr<GraphicsCommandList>)> &cmd_out
) const
{
    ImGui::Render();

    const auto draw_data = ImGui::GetDrawData();
    if(draw_data->TotalVtxCount == 0)
        return;

    // Create the Vertex and Index buffers
    {
        const auto vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
        const auto index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);
        // todo:
        // buffers have usage attributes (vertex, uniform, index, image, etc.)
        // reallocation are handled by memory allocator like the orphaning
        // technique used in opengl
        // buffer usage are tracked by the renderer. when a buffer is no longer
        // used, it is discarded. (possible impl: buffer holds refs to real buffer,
        // which has a lastUsedFrameId counter, and is put into gc list.
        // (possibly sorted by frameId)
        // when its frame ended, the resources are freed.
        // or, find the frame and put it into its gc list. if the frame is already
        // ended, free the buffer immediately. otherwise wait until the frame
        // ends, which is signaled by the device using fence. warn: this may
        // requires locking.
        mVertexBuffer->allocate(vertex_size); // immediately gets a new buffer
        mIndexBuffer->allocate(index_size);
    }

    // Upload Vertex and index Data
    {
        // persistently mapped
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
    cmd_list->beginRendering(mPipeline, framebuffer);
    cmd_list->bindResourceSet(0, { mSampler });

    // Bind Vertex And Index Buffer
    {
        cmd_list->bindVertexBuffer(0, mVertexBuffer.get(), 0);
        cmd_list->bindIndexBuffer(mIndexBuffer.get(), 0,
            GraphicsIndexType::UINT16
        );
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
    cmd_out(std::move(cmd_list));
}

bool usagi::ImGuiSubsystem::processable(Element *element)
{
    return false;
}

void usagi::ImGuiSubsystem::updateRegistry(Element *element)
{
    // todo better approach...?
    if(element->hasComponent<ImGuiComponent>())
        mRegistry.insert(element);
    else
        mRegistry.erase(element);
}

void usagi::ImGuiSubsystem::onKeyStateChange(const KeyEvent &e)
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
}

void usagi::ImGuiSubsystem::onMouseButtonStateChange(
    const MouseButtonEvent &e)
{
    if(e.pressed)
        mMouseJustPressed[static_cast<std::size_t>(e.button)] = true;
}

void usagi::ImGuiSubsystem::onMouseWheelScroll(const MouseWheelEvent &e)
{
    auto &io = ImGui::GetIO();

    io.MouseWheelH += e.distance.x();
    io.MouseWheel += e.distance.y();
}

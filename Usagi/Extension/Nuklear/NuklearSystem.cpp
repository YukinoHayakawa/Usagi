#include "NuklearSystem.hpp"

#include <Usagi/Asset/AssetRoot.hpp>
#include <Usagi/Asset/Converter/SpirvAssetConverter.hpp>
#include <Usagi/Core/Clock.hpp>
#include <Usagi/Core/Logging.hpp>
#include <Usagi/Game/Game.hpp>
#include <Usagi/Graphics/RenderTarget/RenderTarget.hpp>
#include <Usagi/Graphics/RenderTarget/RenderTargetDescriptor.hpp>
#include <Usagi/Runtime/Graphics/Enum/GraphicsIndexType.hpp>
#include <Usagi/Runtime/Graphics/Framebuffer.hpp>
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

#include "Nuklear.hpp"

// bug: mouse scrolls window even when not focusing it
namespace
{
struct nk_vertex
{
    float position[2];
    float uv[2];
    nk_byte col[4];
};
}

void usagi::NuklearSystem::setup()
{
    nk_init_default(&mContext, nullptr);

    mContext.clip.copy = &NuklearSystem::clipboardCopy;
    mContext.clip.paste = &NuklearSystem::clipboardPaste;
    mContext.clip.userdata = nk_handle_ptr(mWindow.get());

    nk_buffer_init_default(&mCommandList);

    // resources
    auto gpu = mGame->runtime()->gpu();
    mVertexBuffer = gpu->createBuffer(GpuBufferUsage::VERTEX);
    mIndexBuffer = gpu->createBuffer(GpuBufferUsage::INDEX);

    mCommandPool = gpu->createCommandPool();

    // fonts
    {
        LOG(info, "Nuklear: Start building font atlas");

        nk_font_atlas_init_default(&mAtlas);
        nk_font_atlas_begin(&mAtlas);
        // todo font file from asset system
        // todo unified font cache with imgui
        auto config = nk_font_config(0);
        static const nk_rune RANGES[] =
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
        config.range = RANGES;
        auto sans = nk_font_atlas_add_from_file(
            &mAtlas,
            "Data/fonts/SourceHanSansCN-Normal.ttf", 32, &config);
        int w, h;
        const auto image = nk_font_atlas_bake(
            &mAtlas, &w, &h, NK_FONT_ATLAS_ALPHA8);
        const auto upload_size = w * h * sizeof(std::uint8_t);
        LOG(info, "Nuklear: Finish building font atlas");

        GpuImageCreateInfo info;
        info.format = GpuBufferFormat::R8_UNORM;
        info.size = { w, h };
        info.usage = GpuImageUsage::SAMPLED;
        mFontTexture = gpu->createImage(info);
        mFontTexture->upload(image, upload_size);
        nk_style_set_font(&mContext, &sans->handle);
    }
    {
        GpuImageViewCreateInfo info;
        info.components.r = GpuImageComponentSwizzle::ONE;
        info.components.g = GpuImageComponentSwizzle::ONE;
        info.components.b = GpuImageComponentSwizzle::ONE;
        info.components.a = GpuImageComponentSwizzle::R;
        mFontTextureView = mFontTexture->createView(info);
        nk_font_atlas_end(
            &mAtlas,
            nk_handle_ptr(mFontTextureView.get()),
            &mNullTexture);
    }
    {
        GpuSamplerCreateInfo info;
        info.min_filter = GpuFilter::LINEAR;
        info.mag_filter = GpuFilter::LINEAR;
        info.addressing_mode_u = GpuSamplerAddressMode::REPEAT;
        info.addressing_mode_v = GpuSamplerAddressMode::REPEAT;
        mSampler = gpu->createSampler(info);
    }
}

usagi::NuklearSystem::NuklearSystem(
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

    nk_input_begin(&mContext);
}

usagi::NuklearSystem::~NuklearSystem()
{
    nk_font_atlas_clear(&mAtlas);
    nk_buffer_free(&mCommandList);
    nk_free(&mContext);
}

void usagi::NuklearSystem::createPipelines()
{
    auto gpu = mGame->runtime()->gpu();
    auto assets = mGame->assets();
    auto compiler = gpu->createPipelineCompiler();

    // Shaders
    {
        compiler->setShader(ShaderStage::VERTEX,
            assets->res<SpirvAssetConverter>(
                "nuklear:shaders/shader.vert", ShaderStage::VERTEX)
        );
        compiler->setShader(ShaderStage::FRAGMENT,
            assets->res<SpirvAssetConverter>(
                "nuklear:shaders/shader.frag", ShaderStage::FRAGMENT)
        );
    }
    // Input Assembly
    {
        compiler->iaSetPrimitiveTopology(PrimitiveTopology::TRIANGLE_LIST);
    }
    // Vertex Inputs
    {
        compiler->setVertexBufferBinding(0, sizeof(nk_vertex));

        compiler->setVertexAttribute(
            "Position", 0,
            offsetof(nk_vertex, position), GpuBufferFormat::R32G32_SFLOAT
        );
        compiler->setVertexAttribute(
            "TexCoord", 0,
            offsetof(nk_vertex, uv), GpuBufferFormat::R32G32_SFLOAT
        );
        compiler->setVertexAttribute(
            "Color", 0,
            offsetof(nk_vertex, col), GpuBufferFormat::R8G8B8A8_UNORM
        );
    }
    // Rasterization
    {
        RasterizationState state;
        state.face_culling_mode = FaceCullingMode::NONE;
        state.front_face = FrontFace::CLOCKWISE;
        state.polygon_mode = PolygonMode::FILL;
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

void usagi::NuklearSystem::clipboardPaste(
    nk_handle usr, nk_text_edit *edit)
{
    const auto window = static_cast<Window*>(usr.ptr);
    const auto str = window->getClipboardText();
    nk_textedit_paste(edit, str.c_str(), static_cast<int>(str.size()));
}

void usagi::NuklearSystem::clipboardCopy(
    nk_handle usr,
    const char *text,
    int len)
{
    assert(len >= 0);
    const auto window = static_cast<Window*>(usr.ptr);
    window->setClipboardText({ text, static_cast<std::size_t>(len) });
}

void usagi::NuklearSystem::update(const Clock &clock)
{
    nk_input_end(&mContext);
    processElements(clock);
}

void usagi::NuklearSystem::createRenderTarget(
    RenderTargetDescriptor &descriptor)
{
    descriptor.sharedColorTarget("nuklear");
    mRenderTarget = descriptor.finish();
}

void usagi::NuklearSystem::processElements(const Clock &clock)
{
    for(auto &e : mRegistry)
        std::get<NuklearComponent*>(e.second)->draw(clock, &mContext);
}

std::shared_ptr<usagi::GraphicsCommandList> usagi::NuklearSystem::render(
    const Clock &clock)
{
    /* fill converting configuration */
    {
        static constexpr std::size_t MAX_VERTEX_BUFFER = 512 * 1024;
        static constexpr std::size_t MAX_INDEX_BUFFER = 128 * 1024;
        mVertexBuffer->allocate(MAX_VERTEX_BUFFER);
        mIndexBuffer->allocate(MAX_INDEX_BUFFER);
        auto vtx_dst = mVertexBuffer->mappedMemory();
        auto idx_dst = mIndexBuffer->mappedMemory();

        static const nk_draw_vertex_layout_element vertex_layout[] = {
            { NK_VERTEX_POSITION, NK_FORMAT_FLOAT,
                offsetof(nk_vertex, position) },
            { NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT,
                offsetof(nk_vertex, uv) },
            { NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8,
                offsetof(nk_vertex, col) },
            { NK_VERTEX_LAYOUT_END }
        };
        nk_convert_config config { };
        config.vertex_layout = vertex_layout;
        config.vertex_size = sizeof(nk_vertex);
        config.vertex_alignment = NK_ALIGNOF(nk_vertex);
        config.global_alpha = 1.0f;
        config.shape_AA = NK_ANTI_ALIASING_ON;
        config.line_AA = NK_ANTI_ALIASING_ON;
        config.circle_segment_count = 22;
        config.curve_segment_count = 22;
        config.arc_segment_count = 22;
        config.null = mNullTexture;

        /* setup buffers to load vertices and elements */
        {
            nk_buffer vbuf, ibuf;
            nk_buffer_init_fixed(&vbuf, vtx_dst, MAX_VERTEX_BUFFER);
            nk_buffer_init_fixed(&ibuf, idx_dst, MAX_INDEX_BUFFER);
            nk_convert(&mContext, &mCommandList, &vbuf, &ibuf, &config);
        }
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

    const auto fs = mFrameBufferSizeFunc();

    // Setup viewport
    {
        cmd_list->setViewport(0, { 0, 0 }, fs);
    }

    // Setup scale and translation
    {
        Vector2f scale { 2, 2 };
        scale = scale.cwiseQuotient(fs);
        Vector2f translate = { -1, -1 };
        cmd_list->setConstant(ShaderStage::VERTEX,
            "scale", scale.data(), sizeof(scale));
        cmd_list->setConstant(ShaderStage::VERTEX,
            "translate", translate.data(), sizeof(translate));
    }

    /* iterate over and execute each draw command */
    const nk_draw_command *cmd;
    std::uint32_t offset = 0;
    nk_draw_foreach(cmd, &mContext, &mCommandList)
    {
        if(!cmd->elem_count) continue;
        auto texture_view = static_cast<GpuImageView*>(cmd->texture.ptr);

        cmd_list->setScissor(0,
            // deal with stupid nk_null_rect
            {
                std::clamp(cmd->clip_rect.x, 0.f, fs.x()),
                std::clamp(cmd->clip_rect.y, 0.f, fs.y())
            },
            {
                std::clamp(cmd->clip_rect.w, 0.f, fs.x()),
                std::clamp(cmd->clip_rect.h, 0.f, fs.y())
            }
        );

        cmd_list->bindResourceSet(1, {
            texture_view->shared_from_this()
        });

        cmd_list->drawIndexedInstanced(
            cmd->elem_count, // index count
            1,               // instance count
            offset,          // first index
            0,               // vertex offset
            0                // first instance
        );
        offset += cmd->elem_count;
    }
    nk_clear(&mContext);

    cmd_list->endRendering();
    cmd_list->endRecording();

    nk_input_begin(&mContext);

    return std::move(cmd_list);
}

bool usagi::NuklearSystem::onKeyStateChange(const KeyEvent &e)
{
    switch(e.key_code)
    {
        case KeyCode::LEFT_SHIFT:
        case KeyCode::RIGHT_SHIFT:
            nk_input_key(&mContext, NK_KEY_SHIFT, e.pressed);
            return true;

        case KeyCode::DELETE:
            nk_input_key(&mContext, NK_KEY_DEL, e.pressed);
            return true;

        case KeyCode::ENTER:
        case KeyCode::NUM_ENTER:
            nk_input_key(&mContext, NK_KEY_ENTER, e.pressed);
            return true;

        case KeyCode::TAB:
            nk_input_key(&mContext, NK_KEY_TAB, e.pressed);
            return true;

        case KeyCode::LEFT:
            if(e.keyboard->isCtrlPressed())
                nk_input_key(&mContext, NK_KEY_TEXT_WORD_LEFT, e.pressed);
            else
                nk_input_key(&mContext, NK_KEY_LEFT, e.pressed);
            return true;
        case KeyCode::RIGHT:
            if(e.keyboard->isCtrlPressed())
                nk_input_key(&mContext, NK_KEY_TEXT_WORD_RIGHT, e.pressed);
            else
                nk_input_key(&mContext, NK_KEY_RIGHT, e.pressed);
            return true;

        case KeyCode::BACKSPACE:
            nk_input_key(&mContext, NK_KEY_BACKSPACE, e.pressed);
            return true;

        case KeyCode::HOME:
            nk_input_key(&mContext, NK_KEY_TEXT_START, e.pressed);
            nk_input_key(&mContext, NK_KEY_SCROLL_START, e.pressed);
            return true;
        case KeyCode::END:
            nk_input_key(&mContext, NK_KEY_TEXT_END, e.pressed);
            nk_input_key(&mContext, NK_KEY_SCROLL_END, e.pressed);
            return true;
        case KeyCode::PAGE_UP:
            nk_input_key(&mContext, NK_KEY_SCROLL_UP, e.pressed);
            return true;
        case KeyCode::PAGE_DOWN:
            nk_input_key(&mContext, NK_KEY_SCROLL_DOWN, e.pressed);
            return true;

        case KeyCode::A:
            if(e.keyboard->isCtrlPressed())
                nk_input_key(&mContext, NK_KEY_TEXT_SELECT_ALL, e.pressed);
            return true;
        case KeyCode::C:
            if(e.keyboard->isCtrlPressed())
                nk_input_key(&mContext, NK_KEY_COPY, e.pressed);
            return true;
        case KeyCode::V:
            if(e.keyboard->isCtrlPressed())
                nk_input_key(&mContext, NK_KEY_PASTE, e.pressed);
            return true;
        case KeyCode::X:
            if(e.keyboard->isCtrlPressed())
                nk_input_key(&mContext, NK_KEY_CUT, e.pressed);
            return true;
        case KeyCode::Z:
            if(e.keyboard->isCtrlPressed())
                nk_input_key(&mContext, NK_KEY_TEXT_UNDO, e.pressed);
            return true;
        case KeyCode::Y:
            if(e.keyboard->isCtrlPressed())
                nk_input_key(&mContext, NK_KEY_TEXT_REDO, e.pressed);
            return true;

        default: return false;
    }
}

bool usagi::NuklearSystem::onMouseMove(const MousePositionEvent &e)
{
    nk_input_motion(
        &mContext,
        static_cast<int>(e.position.x()),
        static_cast<int>(e.position.y())
    );
    return true;
}

bool usagi::NuklearSystem::onMouseButtonStateChange(
    const MouseButtonEvent &e)
{
    const auto pos = e.position.cast<int>();
    switch(e.button)
    {
        case MouseButtonCode::LEFT:
            nk_input_button(
                &mContext, NK_BUTTON_LEFT, pos.x(), pos.y(), e.pressed);
            return true;
        case MouseButtonCode::RIGHT:
            nk_input_button(
                &mContext, NK_BUTTON_RIGHT, pos.x(), pos.y(), e.pressed);
            return true;
        case MouseButtonCode::MIDDLE:
            nk_input_button(
                &mContext, NK_BUTTON_MIDDLE, pos.x(), pos.y(), e.pressed);
            return true;
        default: return false;
    }
}

bool usagi::NuklearSystem::onMouseWheelScroll(const MouseWheelEvent &e)
{
    nk_input_scroll(&mContext, { e.distance.x(), e.distance.y() });
    return true;
}

void usagi::NuklearSystem::onWindowCharInput(const WindowCharEvent &e)
{
    if(e.utf32 >= 32)
        nk_input_unicode(&mContext, e.utf32);
}

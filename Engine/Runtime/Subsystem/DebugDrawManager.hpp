#pragma once

#include <debug-draw/debug_draw.hpp>
#include <vector>

#include <Usagi/Engine/Runtime/Renderable.hpp>

namespace yuki
{

class VertexBuffer;

class DebugDrawManager : public Renderable, public dd::RenderInterface
{
    GraphicsDevice *mGraphicsDevice;
    std::vector<std::shared_ptr<GDTexture>> mTextures;
    std::shared_ptr<class GDPipeline> mLinePointPipeline, mTextPipeline;
    std::shared_ptr<class VertexBuffer> mLinePointVertexBuffer, mTextVertexBuffer;
    std::shared_ptr<class ConstantBuffer> mLinePointConstantBuffer, mTextConstantBuffer;
    std::shared_ptr<class GDSampler> mDefaultSampler;

public:
    DebugDrawManager(GraphicsDevice &gd);
    ~DebugDrawManager() override;

    void render(GraphicsDevice &gd, const Clock &render_clock) override;

    dd::GlyphTextureHandle createGlyphTexture(int width, int height, const void *pixels) override;
    void destroyGlyphTexture(dd::GlyphTextureHandle glyphTex) override;

    void drawPointList(const dd::DrawVertex *points, int count, bool depthEnabled) override;
    void drawLineList(const dd::DrawVertex *lines, int count, bool depthEnabled) override;
    void drawGlyphList(const dd::DrawVertex *glyphs, int count, dd::GlyphTextureHandle glyphTex) override;
};

}

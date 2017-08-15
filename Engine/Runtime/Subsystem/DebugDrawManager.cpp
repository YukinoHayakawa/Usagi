#include <cassert>
#include <vectormath.h>

#include <Usagi/Engine/Time/Clock.hpp>
#include <Usagi/Engine/Runtime/GraphicsDevice/GraphicsDevice.hpp>
#include <Usagi/Engine/Runtime/GraphicsDevice/GDTexture.hpp>
#include <Usagi/Engine/Runtime/GraphicsDevice/VertexBuffer.hpp>
#include <Usagi/Engine/Runtime/GraphicsDevice/GDPipeline.hpp>
#include <Usagi/Engine/Runtime/GraphicsDevice/Shader.hpp>
#include <Usagi/Engine/Runtime/GraphicsDevice/ConstantBuffer.hpp>
#include <Usagi/Engine/Runtime/GraphicsDevice/GDSampler.hpp>
#include <Usagi/Engine/Geometry/Angle.hpp>
#include <Usagi/Engine/Camera/Camera.hpp>

#include "DebugDrawManager.hpp"

namespace
{
// todo: platform independent shader
const char *lpvs = R"(
#version 450

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec4 in_ColorPointSize;

out vec4 v_Color;

layout(std140, binding = 0) uniform View
{
    mat4 u_MvpMatrix;
};

void main()
{
    gl_Position  = u_MvpMatrix * vec4(in_Position, 1.0);
    gl_PointSize = in_ColorPointSize.w;
    v_Color      = vec4(in_ColorPointSize.xyz, 1.0);
}
)";

const char *lpfs = R"(
#version 450

in vec4 v_Color;
out vec4 out_FragColor;

void main()
{
    out_FragColor = v_Color;
}
)";

const char *tvs = R"(
#version 450

layout(location = 0) in vec2 in_Position;
layout(location = 1) in vec2 in_TexCoords;
layout(location = 2) in vec3 in_Color;

layout(std140, binding = 0) uniform Screen
{
    vec2 u_screenDimensions;
};

out vec2 v_TexCoords;
out vec4 v_Color;

void main()
{
    // Map to normalized clip coordinates:
    float x = ((2.0 * (in_Position.x - 0.5)) / u_screenDimensions.x) - 1.0;
    float y = 1.0 - ((2.0 * (in_Position.y - 0.5)) / u_screenDimensions.y);

    gl_Position = vec4(x, y, 0.0, 1.0);
    v_TexCoords = in_TexCoords;
    v_Color     = vec4(in_Color, 1.0);
}
)";

const char *tfs = R"(
#version 450

in vec2 v_TexCoords;
in vec4 v_Color;

layout(binding = 16) uniform sampler2D u_glyphTexture;

out vec4 out_FragColor;

void main()
{
    out_FragColor = v_Color;
    out_FragColor.a = texture(u_glyphTexture, v_TexCoords).r;
}
)";

// todo get from window
static const int windowWidth = 1280;
static const int windowHeight = 720;

static void drawGrid()
{
    const ddVec3 green = { 0.0f, 0.6f, 0.0f };
    dd::xzSquareGrid(-50.0f, 50.0f, -1.0f, 1.7f, green); // Grid from -50 to +50 in both X & Z
}

static void drawLabel(ddVec3Param pos, const char * name)
{
    // NOTE: Labels that are not in the view volume should not be added.
    // In this demo, we skip checking for simplicity, so projected labels
    // that go out of view might still show up in the corners. A frustum
    // check before adding them would fix the issue.
    const ddVec3 textColor = { 0.8f, 0.8f, 1.0f };
//     dd::projectedText(name, pos, textColor, toFloatPtr(camera.vpMatrix),
//         0, 0, 1280, 720, 0.5f);
}

static void drawMiscObjects()
{
    const ddVec3 red = { 1.0f, 0.0f, 0.0f };
    const ddVec3 blue = { 0.0f, 0.0f, 1.0f };
    const ddVec3 cyan = { 0.0f, 1.0f, 1.0f };
    const ddVec3 magenta = { 1.0f, 0.2f, 0.8f };
    const ddVec3 yellow = { 1.0f, 1.0f, 0.0f };
    const ddVec3 orange = { 1.0f, 0.5f, 0.0f };
    const ddVec3 white = { 1.0f, 1.0f, 1.0f };

    // Start a row of objects at this position:
    ddVec3 origin = { -15.0f, 0.0f, 0.0f };

    // Box with a point at it's center:
    drawLabel(origin, "box");
    dd::box(origin, blue, 1.5f, 1.5f, 1.5f);
    dd::point(origin, white, 15.0f);
    origin[0] += 3.0f;

    // Sphere with a point at its center
    drawLabel(origin, "sphere");
    dd::sphere(origin, red, 1.0f);
    dd::point(origin, white, 15.0f);
    origin[0] += 4.0f;

    // Two cones, one open and one closed:
    const ddVec3 condeDir = { 0.0f, 2.5f, 0.0f };
    origin[1] -= 1.0f;

    drawLabel(origin, "cone (open)");
    dd::cone(origin, condeDir, yellow, 1.0f, 2.0f);
    dd::point(origin, white, 15.0f);
    origin[0] += 4.0f;

    drawLabel(origin, "cone (closed)");
    dd::cone(origin, condeDir, cyan, 0.0f, 1.0f);
    dd::point(origin, white, 15.0f);
    origin[0] += 4.0f;

    // Axis-aligned bounding box:
    const ddVec3 bbMins = { -1.0f, -0.9f, -1.0f };
    const ddVec3 bbMaxs = { 1.0f,  2.2f,  1.0f };
    const ddVec3 bbCenter = {
        (bbMins[0] + bbMaxs[0]) * 0.5f,
        (bbMins[1] + bbMaxs[1]) * 0.5f,
        (bbMins[2] + bbMaxs[2]) * 0.5f
    };
    drawLabel(origin, "AABB");
    dd::aabb(bbMins, bbMaxs, orange);
    dd::point(bbCenter, white, 15.0f);

    // Move along the Z for another row:
    origin[0] = -15.0f;
    origin[2] += 5.0f;

    // A big arrow pointing up:
    const ddVec3 arrowFrom = { origin[0], origin[1], origin[2] };
    const ddVec3 arrowTo = { origin[0], origin[1] + 5.0f, origin[2] };
    drawLabel(arrowFrom, "arrow");
    dd::arrow(arrowFrom, arrowTo, magenta, 1.0f);
    dd::point(arrowFrom, white, 15.0f);
    dd::point(arrowTo, white, 15.0f);
    origin[0] += 4.0f;

    // Plane with normal vector:
    const ddVec3 planeNormal = { 0.0f, 1.0f, 0.0f };
    drawLabel(origin, "plane");
    dd::plane(origin, planeNormal, yellow, blue, 1.5f, 1.0f);
    dd::point(origin, white, 15.0f);
    origin[0] += 4.0f;

    // Circle on the Y plane:
    drawLabel(origin, "circle");
    dd::circle(origin, planeNormal, orange, 1.5f, 15.0f);
    dd::point(origin, white, 15.0f);
    origin[0] += 3.2f;

    // Tangent basis vectors:
    const ddVec3 normal = { 0.0f, 1.0f, 0.0f };
    const ddVec3 tangent = { 1.0f, 0.0f, 0.0f };
    const ddVec3 bitangent = { 0.0f, 0.0f, 1.0f };
    origin[1] += 0.1f;
    drawLabel(origin, "tangent basis");
    dd::tangentBasis(origin, normal, tangent, bitangent, 2.5f);
    dd::point(origin, white, 15.0f);

    // And a set of intersecting axes:
    origin[0] += 4.0f;
    origin[1] += 1.0f;
    drawLabel(origin, "cross");
    dd::cross(origin, 2.0f);
    dd::point(origin, white, 15.0f);
}

static void drawFrustum()
{
    const ddVec3 color = { 0.8f, 0.3f, 1.0f };
    const ddVec3 origin = { -8.0f, 0.5f, 14.0f };
    drawLabel(origin, "frustum + axes");

    // The frustum will depict a fake camera:
    const Matrix4 proj = Matrix4::perspective(yuki::Degrees(45.0f).toRadians(), 800.0f / 600.0f, 0.5f, 4.0f);
    const Matrix4 view = Matrix4::lookAt(Point3(-8.0f, 0.5f, 14.0f), Point3(-8.0f, 0.5f, -14.0f), Vector3::yAxis());
    const Matrix4 clip = inverse(proj * view);
    dd::frustum(toFloatPtr(clip), color);

    // A white dot at the eye position:
    const ddVec3 white = { 1.0f, 1.0f, 1.0f };
    dd::point(origin, white, 15.0f);

    // A set of arrows at the camera's origin/eye:
    const Matrix4 transform = Matrix4::translation(Vector3(-8.0f, 0.5f, 14.0f)) * Matrix4::rotationZ(yuki::Degrees(60.f).toRadians());
    dd::axisTriad(toFloatPtr(transform), 0.3f, 2.0f);
}

static void drawText()
{
    // HUD text:
    const ddVec3 textColor = { 1.0f,  1.0f,  1.0f };
    const ddVec3 textPos2D = { 10.0f, 15.0f, 0.0f };
    dd::screenText("Welcome to the Core OpenGL Debug Draw demo.\n\n"
        "[SPACE]  to toggle labels on/off\n"
        "[RETURN] to toggle grid on/off",
        textPos2D, textColor, 0.55f);
}

}

yuki::DebugDrawManager::DebugDrawManager(std::shared_ptr<GraphicsDevice> graphics_device, std::shared_ptr<Camera> camera)
    : Renderable { std::move(graphics_device) }
    , mCamera { std::move(camera) }
{
    /*
     * Line and point rendering
     */

    mLinePointVertexBuffer = mGraphicsDevice->createVertexBuffer();
    mLinePointVertexBuffer->initStorage(DEBUG_DRAW_VERTEX_BUFFER_SIZE, sizeof(dd::DrawVertex));

    mLinePointPipeline = mGraphicsDevice->createPipeline();
    mLinePointPipeline->vpSetVertexInputFormat({
        { 0, 0, 0, NativeDataType::FLOAT, 3, false }, // in_Position
        { 1, 0, 3 * sizeof(float), NativeDataType::FLOAT, 4, false }, // in_ColorPointSize
    });
    mLinePointPipeline->vpBindVertexBuffer(0, mLinePointVertexBuffer);
    {
        auto vs = mGraphicsDevice->createVertexShader();
        vs->useSourceString(lpvs);
        vs->compile();
        mLinePointPipeline->vsUseVertexShader(std::move(vs));
    }
    mLinePointPipeline->rsSetFaceCulling(GDPipeline::FaceCullingType::FRONT);
    {
        auto fs = mGraphicsDevice->createFragmentShader();
        fs->useSourceString(lpfs);
        fs->compile();
        mLinePointPipeline->fsUseFragmentShader(std::move(fs));
    }

    mLinePointConstantBuffer = mGraphicsDevice->createConstantBuffer();
    mLinePointConstantBuffer->setAttributeFormat({
        { ShaderDataType::MATRIX4, 1 }, // u_MvpMatrix
    });
    mLinePointPipeline->vsBindConstantBuffer(0, mLinePointConstantBuffer);

    /*
     * Text rendering
     */

    mTextVertexBuffer = mGraphicsDevice->createVertexBuffer();
    mTextVertexBuffer->initStorage(DEBUG_DRAW_VERTEX_BUFFER_SIZE, sizeof(dd::DrawVertex));

    mTextPipeline = mGraphicsDevice->createPipeline();
    mTextPipeline->vpSetVertexInputFormat({
        { 0, 0, 0, NativeDataType::FLOAT, 2, false }, // in_Position
        { 1, 0, 2 * sizeof(float), NativeDataType::FLOAT, 2, false }, // in_TexCoords
        { 2, 0, 4 * sizeof(float), NativeDataType::FLOAT, 3, false }, // in_Color
    });
    mTextPipeline->vpBindVertexBuffer(0, mTextVertexBuffer);
    {
        auto vs = mGraphicsDevice->createVertexShader();
        vs->useSourceString(tvs);
        vs->compile();
        mTextPipeline->vsUseVertexShader(std::move(vs));
    }
    {
        auto fs = mGraphicsDevice->createFragmentShader();
        fs->useSourceString(tfs);
        fs->compile();
        mTextPipeline->fsUseFragmentShader(std::move(fs));
    }
    mTextPipeline->bldEnableBlend(true);
    mTextPipeline->bldSetOp(GDPipeline::BlendingOperation::ADD);
    mTextPipeline->bldSetSrcFactor(GDPipeline::BlendingFactor::SOURCE_ALPHA);
    mTextPipeline->bldSetDestFactor(GDPipeline::BlendingFactor::INV_SOURCE_ALPHA);

    mTextConstantBuffer = mGraphicsDevice->createConstantBuffer();
    mTextConstantBuffer->setAttributeFormat({
        { ShaderDataType::VECTOR2, 1 }, // u_screenDimensions
    });
    mTextPipeline->vsBindConstantBuffer(0, mTextConstantBuffer);

    mDefaultSampler = mGraphicsDevice->createSampler();
    mDefaultSampler->setFilter(GDSampler::FilterType::LINEAR, GDSampler::FilterType::NEAREST);
    mDefaultSampler->setTextureWrapping(GDSampler::TextureWrapping::CLAMP_TO_EDGE, GDSampler::TextureWrapping::CLAMP_TO_EDGE);
    mTextPipeline->fsBindSampler(0, mDefaultSampler);
    mTextPipeline->fsSamplerUsageHint({
        { 0, 0 },
    });

    dd::initialize(this);
}

yuki::DebugDrawManager::~DebugDrawManager()
{
    dd::shutdown();
}

void yuki::DebugDrawManager::render(const Clock &render_clock)
{
    // todo remove
    drawGrid();
    drawMiscObjects();
    drawFrustum();
    drawText();

    auto pvMat = mCamera->getProjectionMatrix() * mCamera->getLocalToWorldTransform().inverse().matrix();

    mLinePointConstantBuffer->setAttributeData(0, pvMat.data());
    // todo update from window upon resize events
    float scrdim[2] { windowWidth, windowHeight };
    mTextConstantBuffer->setAttributeData(0, scrdim);

    dd::flush(render_clock.getTime() * 1000);
}

dd::GlyphTextureHandle yuki::DebugDrawManager::createGlyphTexture(int width, int height, const void *pixels)
{
    assert(width > 0 && height > 0);
    assert(pixels != nullptr);

    auto tex = mGraphicsDevice->createTexture();

    tex->setFormat(width, height, GDTexture::ChannelEnum::R);
    tex->upload(NativeDataType::UNSIGNED_BYTE, GDTexture::ChannelEnum::R, 1, pixels);

    mTextures.push_back(tex);

    // avoid id 0 so DD acknowledges that we return a valid handle
    return reinterpret_cast<dd::GlyphTextureHandle>(mTextures.size());
}

void yuki::DebugDrawManager::destroyGlyphTexture(dd::GlyphTextureHandle glyphTex)
{
    // do nothing, let the std::vector handle that.
    // notice that for now the dd library only calls this method from shutdown(),
    // but it is not guaranteed that it will not dynamically allocate textures.
}

void yuki::DebugDrawManager::drawPointList(const dd::DrawVertex *points, int count, bool depthEnabled)
{
    assert(points != nullptr);
    assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

    mLinePointVertexBuffer->streamFromHostBuffer(points, count * sizeof(dd::DrawVertex));
    mLinePointPipeline->fdEnableDepthTest(depthEnabled);

    mLinePointPipeline->assemble();

    mGraphicsDevice->drawPoints(0, count);
}

void yuki::DebugDrawManager::drawLineList(const dd::DrawVertex *lines, int count, bool depthEnabled)
{
    assert(lines != nullptr);
    assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

    mLinePointVertexBuffer->streamFromHostBuffer(lines, count * sizeof(dd::DrawVertex));
    mLinePointPipeline->fdEnableDepthTest(depthEnabled);

    mLinePointPipeline->assemble();

    mGraphicsDevice->drawLines(0, count);
}

void yuki::DebugDrawManager::drawGlyphList(const dd::DrawVertex *glyphs, int count, dd::GlyphTextureHandle glyphTex)
{
    assert(glyphs != nullptr);
    assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

    mTextVertexBuffer->streamFromHostBuffer(glyphs, count * sizeof(dd::DrawVertex));
    mTextPipeline->fsBindTexture(0, mTextures[reinterpret_cast<size_t>(glyphTex) - 1]);

    mTextPipeline->assemble();

    mGraphicsDevice->drawTriangles(0, count);
}

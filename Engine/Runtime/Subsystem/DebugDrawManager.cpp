#include <cassert>
#include <vectormath.h>

#include <Usagi/Engine/Runtime/Clock.hpp>
#include <Usagi/Engine/Runtime/GraphicsDevice/GraphicsDevice.hpp>
#include <Usagi/Engine/Runtime/GraphicsDevice/BufferElementType.hpp>
#include <Usagi/Engine/Runtime/GraphicsDevice/GDTexture.hpp>
#include <Usagi/Engine/Runtime/GraphicsDevice/VertexBuffer.hpp>
#include <Usagi/Engine/Runtime/GraphicsDevice/Shader.hpp>

#include "DebugDrawManager.hpp"
#include <GL/glew.h>

namespace
{

const char *lpvs = R"(
#version 450

in vec3 in_Position;
in vec4 in_ColorPointSize;

out vec4 v_Color;
uniform mat4 u_MvpMatrix;

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

in vec2 in_Position;
in vec2 in_TexCoords;
in vec3 in_Color;

uniform vec2 u_screenDimensions;

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

uniform sampler2D u_glyphTexture;
out vec4 out_FragColor;

void main()
{
    out_FragColor = v_Color;
    out_FragColor.a = texture(u_glyphTexture, v_TexCoords).r;
}
)";

// Angle in degrees to angle in radians for sin/cos/etc.
static inline float degToRad(const float ang)
{
    return ang * 3.1415926535897931f / 180.0f;
}

struct Keys
{
    // For the first-person camera controls.
    bool wDown;
    bool sDown;
    bool aDown;
    bool dDown;
    // Flags:
    bool showLabels; // True if object labels are drawn. Toggle with the space bar.
    bool showGrid;   // True if the ground grid is drawn. Toggle with the return key.
} keys;

struct Mouse
{
    enum { MaxDelta = 100 };
    int  deltaX;
    int  deltaY;
    int  lastPosX;
    int  lastPosY;
    bool leftButtonDown;
    bool rightButtonDown;
} mouse;

struct Time
{
    float seconds;
    long long milliseconds;
} deltaTime;

static const int windowWidth = 1280;
static const int windowHeight = 720;

struct Camera
{
    //
    // Camera Axes:
    //
    //    (up)
    //    +Y   +Z (forward)
    //    |   /
    //    |  /
    //    | /
    //    + ------ +X (right)
    //  (eye)
    //
    Vector3 right;
    Vector3 up;
    Vector3 forward;
    Vector3 eye;
    Matrix4 viewMatrix;
    Matrix4 projMatrix;
    Matrix4 vpMatrix;

    enum MoveDir
    {
        Forward, // Move forward relative to the camera's space
        Back,    // Move backward relative to the camera's space
        Left,    // Move left relative to the camera's space
        Right    // Move right relative to the camera's space
    };

    Camera()
    {
        right = Vector3(1.0f, 0.0f, 0.0f);
        up = Vector3(0.0f, 1.0f, 0.0f);
        forward = Vector3(0.0f, 0.0f, 1.0f);
        eye = Vector3(0.0f, 0.0f, 0.0f);
        viewMatrix = Matrix4::identity();
        vpMatrix = Matrix4::identity();

        const float fovY = degToRad(60.0f);
        const float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
        projMatrix = Matrix4::perspective(fovY, aspect, 0.1f, 1000.0f);
    }

    void pitch(const float angle)
    {
        // Pitches camera by 'angle' radians.
        forward = rotateAroundAxis(forward, right, angle); // Calculate new forward.
        up = cross(forward, right);                   // Calculate new camera up vector.
    }

    void rotate(const float angle)
    {
        // Rotates around world Y-axis by the given angle (in radians).
        const float sinAng = std::sin(angle);
        const float cosAng = std::cos(angle);
        float xxx, zzz;

        // Rotate forward vector:
        xxx = forward[0];
        zzz = forward[2];
        forward[0] = xxx *  cosAng + zzz * sinAng;
        forward[2] = xxx * -sinAng + zzz * cosAng;

        // Rotate up vector:
        xxx = up[0];
        zzz = up[2];
        up[0] = xxx *  cosAng + zzz * sinAng;
        up[2] = xxx * -sinAng + zzz * cosAng;

        // Rotate right vector:
        xxx = right[0];
        zzz = right[2];
        right[0] = xxx *  cosAng + zzz * sinAng;
        right[2] = xxx * -sinAng + zzz * cosAng;
    }

    void move(const MoveDir dir, const float amount)
    {
        switch(dir)
        {
            case Camera::Forward: eye += forward * amount; break;
            case Camera::Back: eye -= forward * amount; break;
            case Camera::Left: eye += right   * amount; break;
            case Camera::Right: eye -= right   * amount; break;
        }
    }

    void checkKeyboardMovement()
    {
        const float moveSpeed = 3.0f * deltaTime.seconds;
        if(keys.aDown) { move(Camera::Left, moveSpeed); }
        if(keys.dDown) { move(Camera::Right, moveSpeed); }
        if(keys.wDown) { move(Camera::Forward, moveSpeed); }
        if(keys.sDown) { move(Camera::Back, moveSpeed); }
    }

    void checkMouseRotation()
    {
        static const float maxAngle = 89.5f; // Max degrees of rotation
        static float pitchAmt;

        if(!mouse.leftButtonDown)
        {
            return;
        }

        const float rotateSpeed = 6.0f * deltaTime.seconds;

        // Rotate left/right:
        float amt = static_cast<float>(mouse.deltaX) * rotateSpeed;
        rotate(degToRad(-amt));

        // Calculate amount to rotate up/down:
        amt = static_cast<float>(mouse.deltaY) * rotateSpeed;

        // Clamp pitch amount:
        if((pitchAmt + amt) <= -maxAngle)
        {
            amt = -maxAngle - pitchAmt;
            pitchAmt = -maxAngle;
        }
        else if((pitchAmt + amt) >= maxAngle)
        {
            amt = maxAngle - pitchAmt;
            pitchAmt = maxAngle;
        }
        else
        {
            pitchAmt += amt;
        }

        pitch(degToRad(-amt));
    }

    void updateMatrices()
    {
        viewMatrix = Matrix4::lookAt(Point3(eye), getTarget(), up);
        vpMatrix = projMatrix * viewMatrix; // Vectormath lib uses column-major OGL style, so multiply P*V*M
    }

    Point3 getTarget() const
    {
        return Point3(eye[0] + forward[0], eye[1] + forward[1], eye[2] + forward[2]);
    }

    static Vector3 rotateAroundAxis(const Vector3 & vec, const Vector3 & axis, const float angle)
    {
        const float sinAng = std::sin(angle);
        const float cosAng = std::cos(angle);
        const float oneMinusCosAng = (1.0f - cosAng);

        const float aX = axis[0];
        const float aY = axis[1];
        const float aZ = axis[2];

        float x = (aX * aX * oneMinusCosAng + cosAng)      * vec[0] +
            (aX * aY * oneMinusCosAng + aZ * sinAng) * vec[1] +
            (aX * aZ * oneMinusCosAng - aY * sinAng) * vec[2];

        float y = (aX * aY * oneMinusCosAng - aZ * sinAng) * vec[0] +
            (aY * aY * oneMinusCosAng + cosAng)      * vec[1] +
            (aY * aZ * oneMinusCosAng + aX * sinAng) * vec[2];

        float z = (aX * aZ * oneMinusCosAng + aY * sinAng) * vec[0] +
            (aY * aZ * oneMinusCosAng - aX * sinAng) * vec[1] +
            (aZ * aZ * oneMinusCosAng + cosAng)      * vec[2];

        return Vector3(x, y, z);
    }
} camera;

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
    dd::projectedText(name, pos, textColor, toFloatPtr(camera.vpMatrix),
        0, 0, 1280, 720, 0.5f);
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
    const Matrix4 proj = Matrix4::perspective(degToRad(45.0f), 800.0f / 600.0f, 0.5f, 4.0f);
    const Matrix4 view = Matrix4::lookAt(Point3(-8.0f, 0.5f, 14.0f), Point3(-8.0f, 0.5f, -14.0f), Vector3::yAxis());
    const Matrix4 clip = inverse(proj * view);
    dd::frustum(toFloatPtr(clip), color);

    // A white dot at the eye position:
    const ddVec3 white = { 1.0f, 1.0f, 1.0f };
    dd::point(origin, white, 15.0f);

    // A set of arrows at the camera's origin/eye:
    const Matrix4 transform = Matrix4::translation(Vector3(-8.0f, 0.5f, 14.0f)) * Matrix4::rotationZ(degToRad(60.0f));
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

yuki::DebugDrawManager::DebugDrawManager(GraphicsDevice &gd)
    : mGraphicsDevice { &gd }
{
    (*(mBlendState = gd.createBlendState()).get())
        .enableBlend(false)
        ;

    (*(mRasterizerState = gd.createRasterizerState()).get())
        .enableDepthTest(true)
        .setFaceCulling(RasterizerState::FaceCullingType::FRONT)
        ;

    (*(mLinePointVBO = gd.createVertexBuffer()).get())
        .setLayout(DEBUG_DRAW_VERTEX_BUFFER_SIZE * sizeof(dd::DrawVertex), sizeof(dd::DrawVertex), {
            { "in_Position", BufferElementType::FLOAT, 3, false, 0 },
            { "in_ColorPointSize", BufferElementType::FLOAT, 4, false, 3 * sizeof(float) },
        })
        ;
    (*(mLinePointShader = gd.createShader()).get())
        .setVertexShaderSource(lpvs)
        .setFragmentShaderSource(lpfs)
        .compile()
        ;

    (*(mTextVBO = gd.createVertexBuffer()).get())
        .setLayout(DEBUG_DRAW_VERTEX_BUFFER_SIZE * sizeof(dd::DrawVertex), sizeof(dd::DrawVertex), {
            { "in_Position", BufferElementType::FLOAT, 2, false, 0 },
            { "in_TexCoords", BufferElementType::FLOAT, 2, false, 2 * sizeof(float) },
            { "in_Color", BufferElementType::FLOAT, 4, false, 4 * sizeof(float) },
        })
        ;
    (*(mTextShader = gd.createShader()).get())
        .setVertexShaderSource(tvs)
        .setFragmentShaderSource(tfs)
        .compile()
        ;
    
    dd::initialize(this);
}

yuki::DebugDrawManager::~DebugDrawManager()
{     
    dd::shutdown();
}

void yuki::DebugDrawManager::render(GraphicsDevice &gd, const Clock &render_clock)
{
    assert(&gd == mGraphicsDevice);

    // todo remove
    drawGrid();
    drawMiscObjects();
    drawFrustum();
    drawText();

    dd::flush(render_clock.getTime() * 1000);
}

dd::GlyphTextureHandle yuki::DebugDrawManager::createGlyphTexture(int width, int height, const void *pixels)
{
    auto tex = mGraphicsDevice->createTexture();

    tex->setFormat(width, height, 1, BufferElementType::BYTE);
    tex->upload(pixels);

    mTextures.push_back(tex);

    return reinterpret_cast<dd::GlyphTextureHandle>(tex.get());
}

void yuki::DebugDrawManager::destroyGlyphTexture(dd::GlyphTextureHandle glyphTex)
{
    // do nothing, let the std::vector handle that.
    // notice that for now the dd library only calls this method from shutdown(),
    // but it is not guaranteed that it will not dynamically allocate textures.
}

void yuki::DebugDrawManager::drawPointList(const dd::DrawVertex *points, int count, bool depthEnabled)
{
    // update data
    mLinePointVBO->streamUpdate(points, count * sizeof(dd::DrawVertex));
    mRasterizerState->enableDepthTest(depthEnabled);

    // setup pipeline
    mLinePointShader->linkInputs(*mLinePointVBO.get()); // setup input layout
    mRasterizerState->use();
    mBlendState->use();

    auto ss = mLinePointShader->bind();
    auto vs = mLinePointVBO->bind();


    //todo remove
    GLuint p;
    glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&p));
    auto linePointProgram_MvpMatrixLocation = glGetUniformLocation(p, "u_MvpMatrix");

    glUniformMatrix4fv(linePointProgram_MvpMatrixLocation,
        1, GL_FALSE, toFloatPtr(camera.vpMatrix));



    mGraphicsDevice->drawPoints(0, count);

    // automatically unbind shader, VAO, VBO here
}

void yuki::DebugDrawManager::drawLineList(const dd::DrawVertex *lines, int count, bool depthEnabled)
{
    mLinePointVBO->streamUpdate(lines, count * sizeof(dd::DrawVertex));
    mRasterizerState->enableDepthTest(depthEnabled);

    mLinePointShader->linkInputs(*mLinePointVBO.get());
    mRasterizerState->use();
    mBlendState->use();

    auto ss = mLinePointShader->bind();
    auto vs = mLinePointVBO->bind();


    //todo remove
    GLuint p;
    glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&p));
    auto linePointProgram_MvpMatrixLocation = glGetUniformLocation(p, "u_MvpMatrix");

    glUniformMatrix4fv(linePointProgram_MvpMatrixLocation,
        1, GL_FALSE, toFloatPtr(camera.vpMatrix));




    mGraphicsDevice->drawLines(0, count);
}

void yuki::DebugDrawManager::drawGlyphList(const dd::DrawVertex *glyphs, int count, dd::GlyphTextureHandle glyphTex)
{
    mTextVBO->streamUpdate(glyphs, count * sizeof(dd::DrawVertex));
    mRasterizerState->enableDepthTest(true);

    mTextShader->linkInputs(*mTextVBO.get());
    mRasterizerState->use();
    mBlendState->use();

    auto ss = mTextShader->bind();
    auto vs = mTextVBO->bind();

    mGraphicsDevice->drawTriangles(0, count);
}

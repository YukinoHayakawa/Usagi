#include "DebugDrawDemoComponent.hpp"

#include <Usagi/Engine/Core/Math.hpp>

void usagi::DebugDrawDemoComponent::draw(dd::ContextHandle ctx)
{
    const Vector3f origin = Vector3f::Zero();
    dd::sphere(ctx, origin.data(), dd::colors::Azure, 5.0f);
    dd::point(ctx, origin.data(), dd::colors::BlueViolet, 20.f);

    drawGrid(ctx);
    drawMiscObjects(ctx);
    drawFrustum(ctx);
    drawText(ctx);
}

void usagi::DebugDrawDemoComponent::drawGrid(dd::ContextHandle ctx)
{
    if(!show_grid) { return; }

    // Grid from -50 to +50 in both X & Z
    dd::xySquareGrid(ctx, -50.0f, 50.0f, -1.0f, 2.f, dd::colors::Green);
}

void usagi::DebugDrawDemoComponent::drawLabel(
    dd::ContextHandle ctx,
    ddVec3_In pos,
    const char *name)
{
    if(!show_labels) { return; }

    /*// Only draw labels inside the camera frustum.
    if(camera.isPointInsideFrustum(pos[0], pos[1], pos[2]))
    {
        const ddVec3 textColor = { 0.8f, 0.8f, 1.0f };
        dd::projectedText(
            name, pos, textColor, toFloatPtr(ctx, camera.vpMatrix),
            0, 0, WindowWidth, WindowHeight, 0.5f);
    }*/
}

void usagi::DebugDrawDemoComponent::drawMiscObjects(dd::ContextHandle ctx)
{
    // Start a row of objects at this position:
    ddVec3 origin = { -15.0f, 0.0f, 0.0f };

    // Box with a point at it's center:
    drawLabel(ctx, origin, "box");
    dd::box(ctx, origin, dd::colors::Blue, 1.5f, 1.5f, 1.5f);
    dd::point(ctx, origin, dd::colors::White, 15.0f);
    origin[0] += 3.0f;

    // Sphere with a point at its center
    drawLabel(ctx, origin, "sphere");
    dd::sphere(ctx, origin, dd::colors::Red, 1.0f);
    dd::point(ctx, origin, dd::colors::White, 15.0f);
    origin[0] += 4.0f;

    // Two cones, one open and one closed:
    const ddVec3 condeDir = { 0.0f, 0.0f, 2.5f };
    origin[2] -= 1.0f;

    drawLabel(ctx, origin, "cone (open)");
    dd::cone(ctx, origin, condeDir, dd::colors::Yellow, 1.0f, 2.0f);
    dd::point(ctx, origin, dd::colors::White, 15.0f);
    origin[0] += 4.0f;

    drawLabel(ctx, origin, "cone (closed)");
    dd::cone(ctx, origin, condeDir, dd::colors::Cyan, 0.0f, 1.0f);
    dd::point(ctx, origin, dd::colors::White, 15.0f);
    origin[0] += 4.0f;

    // Axis-aligned bounding box:
    const ddVec3 bbMins = { -1.0f, -0.9f, -1.0f };
    const ddVec3 bbMaxs = { 1.0f, 2.2f, 1.0f };
    const ddVec3 bbCenter = {
        (bbMins[0] + bbMaxs[0]) * 0.5f,
        (bbMins[1] + bbMaxs[1]) * 0.5f,
        (bbMins[2] + bbMaxs[2]) * 0.5f
    };
    drawLabel(ctx, origin, "AABB");
    dd::aabb(ctx, bbMins, bbMaxs, dd::colors::Orange);
    dd::point(ctx, bbCenter, dd::colors::White, 15.0f);

    // Move along the Y for another row:
    origin[0] = -15.0f;
    origin[1] += 5.0f;

    // A big arrow pointing up:
    const ddVec3 arrowFrom = { origin[0], origin[1], origin[2] };
    const ddVec3 arrowTo = { origin[0], origin[1], origin[2] + 5.0f };
    drawLabel(ctx, arrowFrom, "arrow");
    dd::arrow(ctx, arrowFrom, arrowTo, dd::colors::Magenta, 1.0f);
    dd::point(ctx, arrowFrom, dd::colors::White, 15.0f);
    dd::point(ctx, arrowTo, dd::colors::White, 15.0f);
    origin[0] += 4.0f;

    // Plane with normal vector:
    const ddVec3 planeNormal = { 0.0f, 0.0f, 1.0f };
    drawLabel(ctx, origin, "plane");
    dd::plane(ctx, origin, planeNormal, dd::colors::Yellow, dd::colors::Blue,
        1.5f, 1.0f);
    dd::point(ctx, origin, dd::colors::White, 15.0f);
    origin[0] += 4.0f;

    // Circle on the Y plane:
    drawLabel(ctx, origin, "circle");
    dd::circle(ctx, origin, planeNormal, dd::colors::Orange, 1.5f, 15.0f);
    dd::point(ctx, origin, dd::colors::White, 15.0f);
    origin[0] += 3.2f;

    // Tangent basis vectors:
    const ddVec3 normal = { 0.0f, 1.0f, 0.0f };
    const ddVec3 tangent = { 1.0f, 0.0f, 0.0f };
    const ddVec3 bitangent = { 0.0f, 0.0f, 1.0f };
    origin[1] += 0.1f;
    drawLabel(ctx, origin, "tangent basis");
    dd::tangentBasis(ctx, origin, normal, tangent, bitangent, 2.5f);
    dd::point(ctx, origin, dd::colors::White, 15.0f);

    // And a set of intersecting axes:
    origin[0] += 4.0f;
    origin[1] += 1.0f;
    drawLabel(ctx, origin, "cross");
    dd::cross(ctx, origin, 2.0f);
    dd::point(ctx, origin, dd::colors::White, 15.0f);
}

void usagi::DebugDrawDemoComponent::drawFrustum(dd::ContextHandle ctx)
{
    const ddVec3 color = { 0.8f, 0.3f, 1.0f };
    const ddVec3 origin = { -8.0f, 0.5f, 14.0f };
    drawLabel(ctx, origin, "frustum + axes");

    /*// The frustum will depict a fake camera:
    const Matrix4f proj = Matrix4::perspective(degToRad(45.0f), 800.0f / 600.0f, 0.5f, 4.0f);
    const Matrix4f view = Matrix4::lookAt(Point3(-8.0f, 0.5f, 14.0f), Point3(-8.0f, 0.5f, -14.0f), Vector3::yAxis());
    const Matrix4f clip = inverse(proj * view);
    dd::frustum(toFloatPtr(ctx, clip), color);

    // A white dot at the eye position:
    dd::point(ctx, origin, dd::colors::White, 15.0f);

    // A set of arrows at the camera's origin/eye:
    const Matrix4 transform = Matrix4::translation(Vector3(-8.0f, 0.5f, 14.0f)) * Matrix4::rotationZ(degToRad(60.0f));
    dd::axisTriad(toFloatPtr(ctx, transform), 0.3f, 2.0f);*/
}

void usagi::DebugDrawDemoComponent::drawText(dd::ContextHandle ctx)
{
    // HUD text:
    const ddVec3 textColor = { 1.0f, 1.0f, 1.0f };
    const ddVec3 textPos2D = { 10.0f, 15.0f, 0.0f };
    dd::screenText(ctx, "Welcome to the Core OpenGL Debug Draw demo.\n\n"
        "[SPACE]  to toggle labels on/off\n"
        "[RETURN] to toggle grid on/off",
        textPos2D, textColor, 0.55f);
}

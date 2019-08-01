#include "PerspectiveCamera.hpp"

#include "CameraSample.hpp"

void usagi::PerspectiveCamera::setPerspective(
    const Vector2f &screen_size,
    const float fov_y_radians,
    const float aspect,
    const float near,
    const float far)
{
    assert(aspect > 0.f);
    assert(far > near);
    // Deriving the projection matrix:
    // http://ogldev.atspace.co.uk/www/tutorial12/tutorial12.html
    // Note that the division by depth is performed by vertex post-processing
    // of the graphics hardware by dividing the vector by w component.
    // http://vulkan-spec-chunked.ahcox.com/ch23s04.html

    const auto theta = fov_y_radians * 0.5f;
    const auto range = far - near;
    const auto invtan = 1.f / std::tan(theta);

    mLocalToNdc = Matrix4f::Zero();
    mLocalToNdc(0, 0) = invtan / aspect;
    mLocalToNdc(1, 2) = -invtan;
    mLocalToNdc(2, 1) = far / range;
    mLocalToNdc(2, 3) = -near * far / range;
    mLocalToNdc(3, 1) = 1;

    // The transform includes two steps:
    // First, swap & invert the camera axises so they match with those of
    // NDC.
    // Second, transform the view frustum to the NDC cube.
    //
    // invtan/aspect,  0,           0,          0,
    // 0,              0,           -invtan,    0,
    // 0,              far / range, 0,          -near * far / range,
    // 0,              1,           0,          0;

    Affine3f screen_to_ndc = Affine3f::Identity();
    screen_to_ndc.scale(Vector3f(
        2.f / screen_size.x(),
        2.f / screen_size.y(),
        1
    ));
    screen_to_ndc.pretranslate(Vector3f(-1, -1, 0));

    mScreenToLocal = mLocalToNdc.inverse() * screen_to_ndc;
}

usagi::Ray usagi::PerspectiveCamera::generateRay(
    const CameraSample &sample) const
{
    Vector3f screen_pos = resize<3>(sample.screen);

    // todo thin lens instead of pinhole
    const Vector3f local_start = Vector3f::Zero();
    const Vector3f local_end = mScreenToLocal * AsPoint(screen_pos);

    Ray r;
    r.direction = (local_end - local_start).normalized();
    r.origin = local_start;
    return r;
}

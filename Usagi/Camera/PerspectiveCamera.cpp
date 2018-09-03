#include "PerspectiveCamera.hpp"

usagi::PerspectiveCamera::PerspectiveCamera(
    const float fov_y_radians,
    const float aspect,
    const float near,
    const float far)
{
    setPerspective(fov_y_radians, aspect, near, far);
}

void usagi::PerspectiveCamera::setPerspective(
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
}

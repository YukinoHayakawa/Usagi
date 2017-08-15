#include "PerspectiveCamera.hpp"

void yuki::PerspectiveCamera::setPerspective(Radians fovY, float aspect, float near, float far)
{
    float theta = fovY * 0.5f;
    float range = far - near;
    float invtan = 1.f / tan(theta);

    mLocalToNdc(0, 0) = invtan / aspect;
    mLocalToNdc(1, 1) = invtan;
    mLocalToNdc(2, 2) = -(near + far) / range;
    mLocalToNdc(3, 2) = -1;
    mLocalToNdc(2, 3) = -2 * near * far / range;
    mLocalToNdc(3, 3) = 0;
}

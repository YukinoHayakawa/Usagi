#include <Usagi/Math/Matrix.hpp>

#include "details/EntityPage.hpp"

using namespace usagi;

struct alignas(64) SphereComponent
{
    // Vector3f position;
    float size = 0;
    // std::string a = "xxxxxx";
    // std::string a = "xxxxxx";
    Vector2d x;
};

int main(int argc, char *argv[])
{
    ecs::EntityPage<16, SphereComponent> page;
    auto standard = std::is_trivially_destructible_v<SphereComponent>;
    auto standarxd = std::is_standard_layout_v<SphereComponent>;
}

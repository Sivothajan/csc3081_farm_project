#include "Fence.h"
#include "../utils/Helpers.h"
void Fence::segment(Vec3 a, Vec3 b) const {
    const int count = std::max(1, int(std::ceil(length(b - a) / 2.5f)));
    const Vec3 wood{.64f, .48f, .29f};
    for (int i = 0; i <= count; ++i) {
        Vec3 p = a + (b - a) * (float(i) / count);
        Draw::box(p + Vec3{0, .75f, 0}, {.18f, 1.5f, .18f}, wood);
        Draw::ellipsoid(p + Vec3{0, 1.51f, 0}, {.14f, .10f, .14f}, {.76f, .61f, .39f}, 6, 4);
    }
    for (float y : {.52f, 1.12f})
        Draw::beam(a + Vec3{0, y, 0}, b + Vec3{0, y, 0}, .075f, wood, 6);
}
void Fence::render() const {
    segment({-24, 0, -24}, {24, 0, -24});
    segment({-24, 0, -24}, {-24, 0, 24});
    segment({24, 0, -24}, {24, 0, 24});
    segment({-24, 0, 24}, {-2.2f, 0, 24});
    segment({2.2f, 0, 24}, {24, 0, 24});
    segment({3, 0, 3}, {22, 0, 3});
    segment({3, 0, 3}, {3, 0, 13});
    segment({3, 0, 17}, {3, 0, 22});
}

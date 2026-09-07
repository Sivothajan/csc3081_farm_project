#include "Grass.h"
#include "../utils/Helpers.h"
#include <random>
#include <algorithm>
void Grass::initialize() {
    field.clear();
    std::mt19937 rng(3081);
    std::uniform_real_distribution<float> random(0, 1);
    for (int i = 0; i < 2800; ++i) {
        const bool meadow = i < 2300;
        float x = meadow ? -21.5f + 18 * random(rng) : 5 + 15 * random(rng);
        float z = meadow ? 3.3f + 17 * random(rng) : 6 + 11 * random(rng);
        float h = (meadow ? .55f : .28f) + random(rng) * .60f;
        field.push_back({{x, .045f, z},
                         h,
                         h,
                         .10f + random(rng) * .065f,
                         random(rng) * 6.28318f,
                         random(rng) * 6.28318f,
                         .8f + random(rng) * .4f,
                         false,
                         meadow});
    }
}
void Grass::render(const WindSystem& wind) const {
    // One triangle batch for all blades keeps immediate-mode overhead manageable.
    glBegin(GL_TRIANGLES);
    for (const auto& blade : field) {
        Vec3 right{std::cos(blade.orientation), 0, std::sin(blade.orientation)};
        for (int i = 0; i < Constants::GRASS_SEGMENTS; ++i) {
            float u = float(i) / Constants::GRASS_SEGMENTS,
                  v = float(i + 1) / Constants::GRASS_SEGMENTS;
            Vec3 a =
                wind.bend(blade.base, blade.currentHeight, u, blade.phase, blade.response, 1.3f);
            Vec3 b =
                wind.bend(blade.base, blade.currentHeight, v, blade.phase, blade.response, 1.3f);
            Vec3 wa = right * (blade.width * (1 - u) * .5f),
                 wb = right * (blade.width * (1 - v) * .5f);
            float tone = .04f * std::sin(blade.phase);
            glColor3f(.32f + u * .20f + tone, .49f + u * .23f + tone, .16f + u * .12f);
            Draw::triangle(a - wa, a + wa, b + wb);
            Draw::triangle(a - wa, b + wb, b - wb);
        }
    }
    glEnd();
}
int Grass::cutNear(Vec3 p, float radius) {
    int count = 0;
    for (auto& blade : field)
        if (blade.cuttable && !blade.cut && distanceSquared2D(p, blade.base) < radius * radius) {
            blade.currentHeight = blade.originalHeight * .22f;
            blade.cut = true;
            ++count;
        }
    return count;
}
void Grass::reset() {
    for (auto& b : field) {
        b.cut = false;
        b.currentHeight = b.originalHeight;
    }
}
int Grass::cutCount() const {
    return int(
        std::count_if(field.begin(), field.end(), [](const GrassBlade& b) { return b.cut; }));
}

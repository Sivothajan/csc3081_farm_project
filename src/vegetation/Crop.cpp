#include "Crop.h"
#include "../utils/Helpers.h"
#include <random>
void Crop::initialize() {
    stalks.clear();
    std::mt19937 rng(571);
    std::uniform_real_distribution<float> random(0, 1);
    for (float x : rowCenters) {
        for (float z = -20.2f; z < -4.4f; z += .42f)
            stalks.push_back({{x + (random(rng) - .5f) * .22f, .05f, z},
                              1.3f + random(rng) * .5f,
                              random(rng) * 6.28f});
    }
}
void Crop::render(const WindSystem& wind) const {
    glBegin(GL_TRIANGLES);
    for (const auto& stalk : stalks) {
        auto point = [&](float u) {
            return wind.bend(stalk.base, stalk.height, u, stalk.phase, .65f, .72f);
        };
        for (int axis = 0; axis < 2; ++axis) {
            Vec3 side = axis == 0 ? Vec3{1, 0, 0} : Vec3{0, 0, 1};
            glColor3f(.58f, .57f, .20f);
            for (int i = 0; i < 6; ++i) {
                Vec3 a = point(float(i) / 6), b = point(float(i + 1) / 6), w = side * .023f;
                Draw::triangle(a - w, a + w, b + w);
                Draw::triangle(a - w, b + w, b - w);
            }
            // Leaves and seed heads follow the same deformed centerline.
            for (float u : {.35f, .55f}) {
                Vec3 a = point(u), b = point(u + .10f) + side * .30f,
                     c = point(u + .23f) + side * .52f;
                glColor3f(.49f, .55f, .18f);
                Draw::triangle(a, b + Vec3{0, .07f, 0}, c);
                Draw::triangle(a, c, b - Vec3{0, .07f, 0});
            }
            for (int k = 0; k < 5; ++k) {
                float u = .77f + float(k) * .045f;
                Vec3 center = point(u);
                glColor3f(.88f, .70f, .28f);
                for (float sign : {-1.0f, 1.0f}) {
                    Vec3 tip =
                        center + side * (sign * (.13f - float(k) * .014f)) + Vec3{0, .13f, 0};
                    Draw::triangle(center, tip, point(u + .065f));
                }
            }
        }
    }
    glEnd();
}

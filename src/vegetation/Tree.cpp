#include "Tree.h"
#include "../utils/Helpers.h"
void Tree::one(Vec3 p, float scale, float phase, const WindSystem& wind) const {
    glPushMatrix();
    glTranslatef(p.x, 0, p.z);
    glScalef(scale, scale, scale);
    Draw::beam({0, 0, 0}, {0, 4.1f, 0}, .28f, {.39f, .27f, .16f});
    for (int branch = 0; branch < 5; ++branch) {
        float angle = float(branch) * 1.2566f + phase;
        Vec3 root{0, 2.7f + float(branch % 2) * .5f, 0};
        Vec3 rest{std::cos(angle) * 2.0f, 1.3f, std::sin(angle) * 2.0f};
        Vec3 previous = root, tip = root;
        for (int segment = 1; segment <= 5; ++segment) {
            float u = float(segment) / 5;
            float offset = wind.getDisplacement(p.x, p.z, phase + float(branch), u, .35f) * .15f;
            tip = root + rest * u + wind.direction() * offset;
            Draw::beam(previous, tip, .115f * (1 - u) + .035f, {.43f, .31f, .17f});
            previous = tip;
        }
        Draw::ellipsoid(tip + Vec3{0, .35f, 0}, {1.35f, 1.5f, 1.28f},
                        {.29f + float(branch % 2) * .07f, .48f, .20f}, 10, 7);
        // Individual hanging leaves use two deformed segments, attached to the branch.
        glBegin(GL_TRIANGLES);
        for (int leaf = 0; leaf < 9; ++leaf) {
            float a = float(leaf) * .698f;
            Vec3 base = tip + Vec3{std::cos(a), -.2f, std::sin(a)};
            Vec3 middle =
                base + Vec3{.14f, -.35f, 0} +
                wind.direction() * (wind.getDisplacement(p.x, p.z, a + phase, .5f, 2.1f) * .10f);
            Vec3 end =
                base + Vec3{.1f, -.70f, 0} +
                wind.direction() * (wind.getDisplacement(p.x, p.z, a + phase, 1, 2.1f) * .10f);
            glColor3f(.43f, .60f, .24f);
            Draw::triangle(base, middle + Vec3{.13f, 0, 0}, end);
            Draw::triangle(base, end, middle - Vec3{.13f, 0, 0});
        }
        glEnd();
    }
    Draw::ellipsoid({0, 5, 0}, {1.5f, 1.6f, 1.5f}, {.39f, .55f, .23f}, 10, 7);
    glPopMatrix();
}
void Tree::render(const WindSystem& wind) const {
    one({-19, 0, -18}, 1.1f, .2f, wind);
    one({-12, 0, -18}, .9f, 1.7f, wind);
    one({-21, 0, -12}, .85f, 2.6f, wind);
    one({-12, 0, -11}, .73f, .9f, wind);
    one({11, 0, -21}, .85f, 2.0f, wind);
    one({20, 0, -21}, 1.05f, 3.0f, wind);
}

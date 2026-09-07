#include "Windmill.h"
#include "../utils/Helpers.h"
void Windmill::update(float dt, float strength) {
    angle = std::fmod(angle + strength * 65 * dt, 360.0f);
}
void Windmill::render() const {
    glPushMatrix();
    glTranslatef(17, 0, -13);
    const Vec3 timber{.42f, .32f, .22f};
    for (float x : {-1.6f, 1.6f})
        for (float z : {-1.6f, 1.6f})
            Draw::beam({x, 0, z}, {x * .25f, 8, z * .25f}, .16f, timber);
    for (float y : {1.2f, 3.6f, 6.0f}) {
        float r = 1.6f - y * .15f;
        Draw::beam({-r, y, r}, {r, y, r}, .09f, timber);
        Draw::beam({-r, y, -r}, {r, y, -r}, .09f, timber);
        Draw::beam({r, y, -r}, {r, y, r}, .09f, timber);
        Draw::beam({-r, y, -r}, {-r, y, r}, .09f, timber);
    }
    Draw::beam({-1.4f, 1, 1.4f}, {.6f, 6.5f, .6f}, .07f, timber);
    Draw::beam({1.4f, 1, 1.4f}, {-.6f, 6.5f, .6f}, .07f, timber);
    Draw::box({0, 8, 0}, {1.2f, 1.2f, 1.3f}, {.73f, .35f, .23f});
    glPushMatrix();
    glTranslatef(0, 8, .9f);
    glRotatef(angle, 0, 0, 1);
    for (int i = 0; i < 8; ++i) {
        glPushMatrix();
        glRotatef(float(i) * 45, 0, 0, 1);
        Draw::box({0, 1.8f, 0}, {.07f, 3.6f, .10f}, {.35f, .38f, .37f});
        Draw::box({.23f, 2.65f, .03f}, {.55f, 1.55f, .09f}, {.89f, .87f, .71f});
        for (float y = 2.0f; y < 3.4f; y += .3f)
            Draw::box({.23f, y, .10f}, {.55f, .035f, .035f}, {.58f, .57f, .46f});
        glPopMatrix();
    }
    Draw::ellipsoid({0, 0, .12f}, {.34f, .34f, .23f}, {.78f, .32f, .20f});
    glPopMatrix();
    glPopMatrix();
}

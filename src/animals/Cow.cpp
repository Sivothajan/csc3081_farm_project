#include "Cow.h"
#include "../utils/Helpers.h"
#include <algorithm>
void Cow::update(float value) {
    time = value;
    float t = time + offset, phase = std::fmod(t, 24.0f);
    // Ten seconds walking, eleven grazing, then three standing. Analytical travel
    // keeps the circular route independent of how many frames were rendered.
    float walkingTime = std::floor(t / 24) * 10 + std::min(phase, 10.0f);
    float angle = walkingTime * .14f + offset;
    location = center + Vec3{std::cos(angle) * 2.1f, 0, std::sin(angle) * 2.1f};
    heading = -(angle + Constants::PI * .5f) * 180 / Constants::PI;
    legSwing = phase < 10 ? std::sin(time * 4.5f + offset) * 18 : 0;
    float grazing = std::clamp(std::min(phase - 10, 21 - phase), 0.0f, 1.0f);
    headDip = grazing * (55 + 3 * std::sin(time * 2));
}
void Cow::render() const {
    const Vec3 white{.92f, .89f, .77f}, black{.16f, .19f, .17f}, pink{.75f, .49f, .40f};
    glPushMatrix();
    glTranslatef(location.x, 0, location.z);
    glRotatef(heading, 0, 1, 0);
    Draw::ellipsoid({0, 1.25f, 0}, {1.15f, .63f, .58f}, white, 16, 10);
    for (float side : {-1.0f, 1.0f}) {
        Draw::ellipsoid({-.35f, 1.38f, side * .52f}, {.44f, .36f, .085f}, black);
        Draw::ellipsoid({.47f, 1.18f, side * .49f}, {.25f, .27f, .12f}, black);
        for (float x : {-.73f, .72f}) {
            glPushMatrix();
            glTranslatef(x, 1.03f, side * .36f);
            glRotatef(legSwing * (x * side > 0 ? 1.0f : -1.0f), 0, 0, 1);
            Draw::box({0, -.43f, 0}, {.18f, .86f, .18f}, white);
            Draw::box({.04f, -.91f, 0}, {.27f, .18f, .24f}, black);
            glPopMatrix();
        }
    }
    // Neck, head, ears, eyes and snout share the grazing pivot.
    glPushMatrix();
    glTranslatef(.86f, 1.50f, 0);
    glRotatef(-headDip, 0, 0, 1);
    Draw::ellipsoid({.38f, 0, 0}, {.48f, .43f, .38f}, white);
    Draw::ellipsoid({.77f, -.14f, 0}, {.26f, .23f, .36f}, pink);
    for (float side : {-1.0f, 1.0f}) {
        Draw::ellipsoid({.32f, .28f, side * .44f}, {.20f, .10f, .25f}, black);
        Draw::ellipsoid({.60f, .12f, side * .31f}, {.065f, .075f, .055f}, black);
        Draw::ellipsoid({.98f, -.1f, side * .16f}, {.025f, .045f, .055f}, black);
        Draw::beam({.23f, .32f, side * .24f}, {.16f, .61f, side * .30f}, .05f, {.82f, .74f, .53f},
                   6);
    }
    glPopMatrix();
    Vec3 tailEnd{-1.38f, .46f, std::sin(time * 2 + offset) * .20f};
    Draw::beam({-1.0f, 1.4f, 0}, tailEnd, .045f, white);
    Draw::ellipsoid(tailEnd, {.10f, .20f, .10f}, black);
    Draw::ellipsoid({-.3f, .73f, 0}, {.32f, .20f, .30f}, pink);
    glPopMatrix();
}

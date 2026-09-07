#include "Cow.h"
#include "../core/Text.h"
#include "../utils/Helpers.h"
#include <algorithm>
void Cow::update(float value) {
    sleepAmount = 0;
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
bool Cow::walkTo(Vec3 target, float dt) {
    Vec3 delta = target - location;
    float distance = length(delta), step = std::min(distance, 2.4f * dt);
    if (distance > .0001f)
        follow(location + delta * (step / distance), delta, dt);
    else
        rest(false, dt);
    return distance <= step + .0001f;
}
void Cow::follow(Vec3 next, Vec3 direction, float dt) {
    time += dt;
    sleepAmount = std::max(0.0f, sleepAmount - dt);
    legSwing = length(next - location) > .00001f ? std::sin(time * 7 + offset) * 23 : 0;
    location = next;
    headDip = 0;
    if (length(direction) > .0001f) {
        float target = -std::atan2(direction.z, direction.x) * 180 / Constants::PI;
        heading += std::clamp(std::remainder(target - heading, 360.0f), -240 * dt, 240 * dt);
    }
}
bool Cow::face(Vec3 direction, float dt) {
    follow(location, direction, dt);
    float target = -std::atan2(direction.z, direction.x) * 180 / Constants::PI;
    return std::abs(std::remainder(target - heading, 360.0f)) < 1;
}
void Cow::rest(bool asleep, float dt) {
    time += dt;
    sleepAmount = std::clamp(sleepAmount + (asleep ? dt : -dt), 0.0f, 1.0f);
    legSwing = 0;
    headDip = 28 * sleepAmount;
}
void Cow::render() const {
    const Vec3 white{.92f, .89f, .77f}, black{.16f, .19f, .17f}, pink{.75f, .49f, .40f};
    glPushMatrix();
    glTranslatef(location.x, -.55f * sleepAmount, location.z);
    glRotatef(heading, 0, 1, 0);
    Draw::ellipsoid({0, 1.25f, 0},
                    {1.15f, .63f + .015f * sleepAmount * std::sin(time * 1.7f), .58f}, white, 16,
                    10);
    for (float side : {-1.0f, 1.0f}) {
        Draw::ellipsoid({-.35f, 1.38f, side * .52f}, {.44f, .36f, .085f}, black);
        Draw::ellipsoid({.47f, 1.18f, side * .49f}, {.25f, .27f, .12f}, black);
        for (float x : {-.73f, .72f}) {
            glPushMatrix();
            glTranslatef(x, 1.03f, side * .36f);
            float fold = (x > 0 ? -78.0f : 78.0f) * sleepAmount;
            glRotatef(fold + legSwing * (x * side > 0 ? 1.0f : -1.0f), 0, 0, 1);
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
        Draw::ellipsoid({.60f, .12f, side * .31f}, {.065f, .075f - .06f * sleepAmount, .055f},
                        black);
        Draw::ellipsoid({.98f, -.1f, side * .16f}, {.025f, .045f, .055f}, black);
        Draw::beam({.23f, .32f, side * .24f}, {.16f, .61f, side * .30f}, .05f, {.82f, .74f, .53f},
                   6);
    }
    glPopMatrix();
    Vec3 tailEnd{-1.38f, .46f + .42f * sleepAmount,
                 std::sin(time * 2 + offset) * .20f * (1 - sleepAmount)};
    Draw::beam({-1.0f, 1.4f, 0}, tailEnd, .045f, white);
    Draw::ellipsoid(tailEnd, {.10f, .20f, .10f}, black);
    Draw::ellipsoid({-.3f, .73f + .1f * sleepAmount, 0}, {.32f, .20f, .30f}, pink);
    glPopMatrix();
    if (sleepAmount > .8f) {
        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
        glDisable(GL_LIGHTING);
        glColor3f(.98f, .88f, .57f);
        glRasterPos3f(location.x - .16f, 1.65f + .06f * std::sin(time), location.z);
        for (unsigned char c : Text::get("sleep.symbol"))
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        glPopAttrib();
    }
}

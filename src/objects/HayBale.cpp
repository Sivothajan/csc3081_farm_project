#include "HayBale.h"
#include "../utils/Helpers.h"
void HayBale::bale(Vec3 center, float scale) {
    glPushMatrix();
    glTranslatef(center.x, center.y, center.z);
    glScalef(scale, scale, scale);
    Draw::box({0, 0, 0}, {2.0f, 1.35f, 1.6f}, {.82f, .64f, .25f});
    for (float x : {-.65f, .65f})
        Draw::box({x, 0, 0}, {.09f, 1.39f, 1.64f}, {.40f, .34f, .18f});
    for (float y = -.5f; y < .65f; y += .14f)
        Draw::box({0, y, .805f}, {1.97f, .025f, .02f}, {.93f, .75f, .36f});
    glPopMatrix();
}
void HayBale::render() const {
    for (Vec3 p :
         {Vec3{19, .7f, 19}, Vec3{16.7f, .7f, 19.2f}, Vec3{18, 2.0f, 19.1f}, Vec3{-21, .7f, -2.8f}})
        bale(p);
    // Water trough in the pasture.
    Draw::box({18, .4f, 5}, {3, .8f, 1.3f}, {.47f, .50f, .45f});
    Draw::ground(16.65f, 4.48f, 19.35f, 5.52f, .81f, {.37f, .65f, .71f});
}

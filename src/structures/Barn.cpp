#include "Barn.h"
#include "../objects/HayBale.h"
#include "../utils/Helpers.h"
#include <algorithm>

void Barn::update(float dt, bool open) {
    opening = std::clamp(opening + (open ? dt : -dt) * .65f, 0.0f, 1.0f);
}
void Barn::light(float nightAmount, bool enabled) const {
    if (!enabled || nightAmount <= .001f) {
        glDisable(GL_LIGHT1);
        return;
    }
    glEnable(GL_LIGHT1);
    const GLfloat position[]{-17, 3.3f, -8.5f, 1};
    const GLfloat diffuse[]{nightAmount, nightAmount * .79f, nightAmount * .48f, 1};
    const GLfloat ambient[]{.10f * nightAmount, .075f * nightAmount, .04f * nightAmount, 1};
    glLightfv(GL_LIGHT1, GL_POSITION, position);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, .08f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, .018f);
}
void Barn::render(float nightAmount) const {
    const Vec3 red{.66f, .23f, .17f}, cream{.94f, .86f, .67f}, wood{.48f, .32f, .18f};
    glPushMatrix();
    glTranslatef(-17, 0, -8.3f);
    // Separate walls leave a real doorway and a clear aisle in front of the stalls.
    Draw::box({0, .01f, 0}, {7, .06f, 8.2f}, {.48f, .43f, .33f});
    Draw::box({0, 1.9f, -4.1f}, {7, 3.8f, .18f}, red);
    for (float side : {-1.0f, 1.0f}) {
        Draw::box({side * 3.5f, 1.9f, 0}, {.18f, 3.8f, 8.2f}, red);
        Draw::box({side * 2.5f, 1.9f, 4.1f}, {2, 3.8f, .18f}, red);
    }
    Draw::box({0, 3.45f, 4.1f}, {3, .7f, .18f}, red);
    for (float x = -3.3f; x < 3.5f; x += .43f) {
        if (std::abs(x) > 1.5f)
            Draw::box({x, 1.9f, 4.21f}, {.07f, 3.8f, .06f}, {.79f, .32f, .22f});
    }
    glColor3f(red.x, red.y, red.z);
    glBegin(GL_TRIANGLES);
    Draw::triangle({-3.5f, 3.8f, 4.1f}, {3.5f, 3.8f, 4.1f}, {0, 5.7f, 4.1f});
    Draw::triangle({3.5f, 3.8f, -4.1f}, {-3.5f, 3.8f, -4.1f}, {0, 5.7f, -4.1f});
    glEnd();
    for (float side : {-1.0f, 1.0f}) {
        glPushMatrix();
        glTranslatef(side * 1.85f, 4.72f, 0);
        glRotatef(-side * 28.5f, 0, 0, 1);
        Draw::box({0, 0, 0}, {4.3f, .2f, 8.8f}, {.23f, .30f, .29f});
        glPopMatrix();
        // The controller waits for these outward-opening leaves before releasing a cow.
        glPushMatrix();
        glTranslatef(side * 1.48f, 0, 4.23f);
        glRotatef(side * opening * 100, 0, 1, 0);
        Draw::box({-side * .73f, 1.5f, 0}, {1.46f, 3, .12f}, {.38f, .17f, .13f});
        Draw::beam({0, .1f, .08f}, {-side * 1.43f, 2.92f, .08f}, .05f, cream, 4);
        Draw::box({-side * 1.4f, 1.5f, .08f}, {.09f, 3, .09f}, cream);
        Draw::box({-side * 1.23f, 1.5f, .15f}, {.07f, .3f, .07f}, {.17f, .19f, .16f});
        glPopMatrix();
        Draw::box({side * 1.53f, 1.55f, 4.27f}, {.12f, 3.2f, .12f}, cream);
        Draw::box({side * 2.5f, 2.4f, 4.23f}, {.9f, 1, .1f}, cream);
        Draw::box({side * 2.5f, 2.4f, 4.30f}, {.7f, .8f, .06f}, {.20f, .36f, .38f});
    }
    Draw::box({0, 3.12f, 4.27f}, {3.2f, .14f, .12f}, cream);
    Draw::box({0, 4.35f, 4.16f}, {.7f, .7f, .1f}, cream);

    for (float x : {-2.25f, -.75f, .75f, 2.25f}) {
        for (float z : {-3.7f, .55f})
            Draw::box({x, .62f, z}, {.10f, 1.24f, .10f}, wood);
        for (float y : {.45f, 1.02f})
            Draw::beam({x, y, -3.7f}, {x, y, .55f}, .045f, wood, 6);
    }
    for (int i = 0; i < 3; ++i) {
        float x = float(i - 1) * 1.5f;
        Draw::ground(x - .68f, -3.5f, x + .68f, .5f, .055f, {.70f, .55f, .24f});
        for (int straw = 0; straw < 20; ++straw) {
            float z = -3.35f + float(straw) * .19f;
            Draw::beam({x - .55f, .068f, z}, {x + .5f, .068f, z + .10f}, .012f, {.88f, .72f, .36f},
                       4);
        }
        HayBale::bale({x, .33f, -3.55f}, .42f);
        Draw::beam({x, 3.8f, -3.25f}, {x, 3.04f, -3.25f}, .022f, wood, 6);
        Draw::plaque({x, 2.74f, -3.25f}, "Stall " + std::to_string(i + 1), 1.32f, .62f);
    }
    for (float x : {-2.85f, 2.85f}) {
        HayBale::bale({x, .34f, -1.4f}, .45f);
        HayBale::bale({x, .96f, -1.4f}, .45f);
        HayBale::bale({x, .34f, -.55f}, .45f);
    }
    Draw::beam({0, 4.25f, -.2f}, {0, 3.25f, -.2f}, .035f, wood, 6);
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_LIGHTING);
    Draw::ellipsoid({0, 3.2f, -.2f}, {.20f, .12f, .20f},
                    {.55f + nightAmount * .45f, .48f + nightAmount * .36f, .28f}, 10, 6);
    glPopAttrib();
    glPopMatrix();
}

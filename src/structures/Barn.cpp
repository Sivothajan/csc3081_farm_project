#include "Barn.h"
#include "../utils/Helpers.h"
void Barn::render() const {
    glPushMatrix();
    glTranslatef(-17, 0, -7);
    Draw::box({0, 1.9f, 0}, {7, 3.8f, 5.6f}, {.66f, .23f, .17f});
    for (float x = -3.3f; x < 3.5f; x += .43f)
        Draw::box({x, 1.9f, 2.83f}, {.07f, 3.8f, .06f}, {.79f, .32f, .22f});
    glColor3f(.67f, .25f, .18f);
    glBegin(GL_TRIANGLES);
    Draw::triangle({-3.5f, 3.8f, 2.8f}, {3.5f, 3.8f, 2.8f}, {0, 5.7f, 2.8f});
    Draw::triangle({3.5f, 3.8f, -2.8f}, {-3.5f, 3.8f, -2.8f}, {0, 5.7f, -2.8f});
    glEnd();
    for (float side : {-1.0f, 1.0f}) {
        glPushMatrix();
        glTranslatef(side * 1.85f, 4.72f, 0);
        glRotatef(-side * 28.5f, 0, 0, 1);
        Draw::box({0, 0, 0}, {4.3f, .2f, 6.2f}, {.23f, .30f, .29f});
        glPopMatrix();
    }
    const Vec3 cream{.94f, .86f, .67f};
    Draw::box({0, 1.45f, 2.88f}, {2.8f, 2.9f, .12f}, {.38f, .17f, .13f});
    for (float x : {-1.48f, 0.0f, 1.48f})
        Draw::box({x, 1.5f, 2.98f}, {.12f, 3.1f, .1f}, cream);
    Draw::box({0, 3.05f, 2.98f}, {3.1f, .14f, .1f}, cream);
    Draw::beam({-1.4f, .1f, 3.03f}, {1.4f, 2.92f, 3.03f}, .055f, cream, 4);
    Draw::beam({1.4f, .1f, 3.03f}, {-1.4f, 2.92f, 3.03f}, .055f, cream, 4);
    for (float x : {-2.5f, 2.5f}) {
        Draw::box({x, 2.4f, 2.88f}, {.9f, 1.0f, .1f}, cream);
        Draw::box({x, 2.4f, 2.96f}, {.7f, .8f, .06f}, {.20f, .36f, .38f});
    }
    Draw::box({0, 4.35f, 2.85f}, {.7f, .7f, .1f}, cream);
    glPopMatrix();
}

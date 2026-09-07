#include "Rock.h"
#include "../utils/Helpers.h"
void Rock::render() const {
    for (int i = 0; i < 18; ++i) {
        float t = float(i), x, z;
        if (i < 9) {
            x = -22.0f + t * 2.4f;
            z = -22.4f + std::sin(t) * .45f;
        } else {
            x = 21.8f + std::sin(t) * .65f;
            z = -18.0f + (t - 9) * 4.5f;
        }
        glPushMatrix();
        glTranslatef(x, .2f, z);
        glRotatef(t * 57, 0, 1, 0);
        glScalef(.45f + .2f * std::sin(t), .5f, .65f);
        glColor3f(.47f + .03f * std::sin(t), .48f, .42f);
        glutSolidDodecahedron();
        glPopMatrix();
    }
}

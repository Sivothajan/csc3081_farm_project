#include "Lighting.h"
#include <GL/freeglut.h>
void Lighting::apply(float night, bool enabled) const {
    if (!enabled) {
        glDisable(GL_LIGHTING);
        return;
    }
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    const GLfloat ambient[] = {.40f - night * .15f, .41f - night * .13f, .38f - night * .08f, 1};
    const GLfloat diffuse[] = {.85f - night * .48f, .81f - night * .42f, .67f - night * .19f, 1};
    const GLfloat direction[] = {-0.5f, 1.0f, 0.6f, 0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, direction);
}

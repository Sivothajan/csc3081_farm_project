#include "GrassCuttingSystem.h"
#include "../utils/Helpers.h"
void GrassCuttingSystem::update(Grass& grass, Vec3 camera) const {
    // Flying over the farm must not accidentally mow the whole field below.
    if (enabled && camera.y < 2.6f)
        grass.cutNear(camera, Constants::CUT_RADIUS);
}
void GrassCuttingSystem::render(Vec3 camera) const {
    if (!enabled || camera.y >= 2.6f)
        return;
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glColor3f(.98f, .86f, .35f);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 64; ++i) {
        float a = float(i) * 2 * Constants::PI / 64;
        glVertex3f(camera.x + std::cos(a) * Constants::CUT_RADIUS, .065f,
                   camera.z + std::sin(a) * Constants::CUT_RADIUS);
    }
    glEnd();
    glPopAttrib();
}

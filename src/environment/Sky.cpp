#include "Sky.h"
#include "../utils/Helpers.h"
#include <algorithm>
void Sky::update(float dt) {
    float change = dt * .4f;
    nightAmount = std::clamp(nightAmount + (night ? change : -change), 0.0f, 1.0f);
}
void Sky::background() const {
    glClearColor(.66f - nightAmount * .61f, .81f - nightAmount * .72f, .86f - nightAmount * .70f,
                 1);
}
void Sky::render(float time) const {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    Vec3 color{.96f - nightAmount * .60f, .96f - nightAmount * .56f, .89f - nightAmount * .42f};
    for (int i = 0; i < 5; ++i) {
        float x = std::fmod(float(i) * 19 + time * .25f, 100.0f) - 50;
        float z = -32 + float(i % 3) * 7;
        for (int j = 0; j < 4; ++j)
            Draw::ellipsoid({x + float(j) * 1.4f, 34 + std::sin(float(j) * 1.6f), z},
                            {2.3f, 1.2f, 1.7f}, color, 10, 7);
    }
    Draw::ellipsoid({-27, 25, -35}, {2.3f, 2.3f, 2.3f},
                    {1.0f - nightAmount * .13f, .91f, .63f + nightAmount * .25f}, 16, 12);
    glPopAttrib();
}

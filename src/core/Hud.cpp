#include "Hud.h"
#include "../utils/Helpers.h"
#include <iomanip>
#include <sstream>
namespace {
void panel(float x, float y, float w, float h) {
    glColor4f(.075f, .14f, .13f, .91f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}
} // namespace
void Hud::render(int width, int height, const WindSystem& wind, HudState s) const {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT |
                 GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_FOG);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    float top = float(height), right = float(width);
    panel(20, top - 115, std::min(370.0f, right - 300), 95);
    glColor3f(.95f, .88f, .65f);
    Draw::text(38, top - 50, "W I L L O W F I E L D", GLUT_BITMAP_HELVETICA_18);
    glColor3f(.76f, .83f, .73f);
    Draw::text(38, top - 73, "CSC3081  /  Interactive 3D Farm");
    Draw::text(38, top - 94,
               std::string(Constants::AUTHOR_NAME) + " / " + Constants::REGISTRATION_NUMBER);
    panel(right - 254, top - 130, 234, 110);
    glColor3f(.96f, .88f, .65f);
    std::ostringstream label;
    label << "WIND  " << std::fixed << std::setprecision(2) << wind.getStrength() << " / 3.00";
    Draw::text(right - 237, top - 46, label.str(), GLUT_BITMAP_HELVETICA_12);
    for (int i = 0; i < 12; ++i) {
        float x = right - 237 + float(i) * 16;
        bool active = float(i) < wind.getStrength() * 4;
        glColor3f(active ? .82f : .25f, active ? .73f : .34f, active ? .37f : .28f);
        glBegin(GL_QUADS);
        glVertex2f(x, top - 73);
        glVertex2f(x + 12, top - 73);
        glVertex2f(x + 12, top - 62);
        glVertex2f(x, top - 62);
        glEnd();
    }
    glColor3f(.78f, .84f, .75f);
    Draw::text(right - 237, top - 99, "+ / - strength    0 still    1 / 2 / 3");
    Draw::text(right - 237, top - 116,
               s.paused
                   ? "ANIMATION PAUSED / P resumes"
                   : (wind.getStrength() == 0 ? "STILL / WIND DISABLED" : "LIVE WIND > +X / +Z"));
    panel(20, 20, right - 40, 70);
    glColor3f(.96f, .90f, .73f);
    Draw::text(36, 69,
               width < 800
                   ? "WASD move | Arrows look | Q/E height | G meadow | V overview"
                   : "WASD move   Arrows / right-drag look   Q/E height   G meadow   V overview");
    Draw::text(36, 49,
               width < 800
                   ? "C cut | R regrow | P pause | N night | B study | L/T/F debug | Esc exit"
                   : "C cut   R regrow   P pause   N day/night   B bend study   L light   T "
                     "texture   F wire   Esc exit");
    std::ostringstream status;
    status << (s.cutting ? "CUTTING ON" : "CUTTING OFF") << "   |   " << s.cutCount
           << " / 2300 blades cut";
    if (s.cutting && s.height >= 2.6f)
        status << "   |   G: enter meadow";
    else if (s.cutting)
        status << "   |   Walk to mow";
    glColor3f(.79f, .83f, .60f);
    Draw::text(36, 30, status.str());
    if (s.diagram && width >= 800 && height >= 550) {
        float x = right - 254, y = 110;
        panel(x, y, 234, 230);
        glColor3f(.96f, .89f, .69f);
        Draw::text(x + 16, y + 207, "BEND STUDY  /  B to hide");
        glColor3f(.66f, .73f, .65f);
        Draw::text(x + 16, y + 189, "Same root, increasing tip offset");
        float bx = x + 64, by = y + 46, scale = 105;
        glLineWidth(1);
        glColor3f(.42f, .49f, .44f);
        glBegin(GL_LINES);
        glVertex2f(bx, by);
        glVertex2f(bx, by + scale);
        glEnd();
        glLineWidth(3);
        glColor3f(.88f, .77f, .36f);
        glBegin(GL_LINE_STRIP);
        const Vec3 root{-10, 0, 10};
        for (int i = 0; i <= 5; ++i) {
            float u = float(i) / 5;
            Vec3 offset = wind.bend(root, 1, u, .8f) - root;
            glVertex2f(bx + offset.x * scale, by + offset.y * scale);
        }
        glEnd();
        glColor3f(.88f, .90f, .74f);
        Draw::text(bx - 23, by - 18, "FIXED ROOT");
        Draw::text(x + 16, y + 18, "Offset = wind x (height / total)^2");
    }
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
}

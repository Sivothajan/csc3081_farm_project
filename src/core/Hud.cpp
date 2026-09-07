#include "Hud.h"
#include "../utils/Helpers.h"
#include "../utils/Font.h"
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
void fitted(float x, float y, const std::string& value, float size, float width) {
    Draw::text(x, y, value,
               std::min(size, size * width / std::max(Font::width(value, size), 1.0f)));
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
    bool compact = width < 900;
    float body = compact ? 20.0f : 24.0f, small = compact ? 18.0f : 20.0f;
    float leftWidth = std::min(420.0f, right - 296);
    if (s.focusedView) {
        panel(16, top - 84, right - 32, 68);
        glColor3f(.95f, .88f, .65f);
        Draw::text(32, top - 48, "Willowfield", 34);
        Draw::text(right - 228, top - 45,
                   std::string(s.night ? "Night" : "Day") + " / " + std::to_string(s.sleepingCows) +
                       " cows asleep",
                   small);
        glColor3f(.78f, .84f, .75f);
        Draw::text(32, top - 72, s.herdStatus + (s.paused ? " / Paused" : ""), small);
    } else {
        panel(16, top - 120, leftWidth, 104);
        glColor3f(.95f, .88f, .65f);
        Draw::text(32, top - 54, "Willowfield", compact ? 38.0f : 44.0f);
        glColor3f(.76f, .83f, .73f);
        Draw::text(32, top - 80, "CSC3081 / Interactive 3D farm", small);
        fitted(32, top - 105,
               std::string(Constants::AUTHOR_NAME) + " / " + Constants::REGISTRATION_NUMBER, small,
               leftWidth - 32);
        panel(16, top - 208, leftWidth, 80);
        glColor3f(.96f, .88f, .65f);
        Draw::text(32, top - 151, s.night ? "Night on the farm" : "Day on the farm", body);
        glColor3f(.78f, .84f, .75f);
        fitted(32, top - 176, s.herdStatus, small, leftWidth - 32);
        Draw::text(32, top - 198, std::to_string(s.sleepingCows) + " of 3 cows asleep / H to visit",
                   small);
        panel(right - 264, top - 136, 248, 120);
        glColor3f(.96f, .88f, .65f);
        std::ostringstream label;
        label << "Wind  " << std::fixed << std::setprecision(1) << wind.getStrength() << " / 3";
        Draw::text(right - 248, top - 46, label.str(), body);
        for (int i = 0; i < 12; ++i) {
            float x = right - 248 + float(i) * 18;
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
        fitted(right - 248, top - 100, "+ / - adjust   0 / 1 / 2 / 3 presets", 18, 216);
        fitted(right - 248, top - 124,
               s.paused
                   ? "Paused / P to resume"
                   : (wind.getStrength() == 0 ? "A still, quiet day" : "Breeze toward +X / +Z"),
               small, 216);
    }
    panel(16, 16, right - 32, s.focusedView ? 66.0f : 112.0f);
    glColor3f(.96f, .90f, .73f);
    if (s.focusedView) {
        fitted(32, 57, s.viewName + " / Tab next view / F1-F8 choose", body, right - 64);
        fitted(32, 31, "WASD move   Arrows look   N day/night   P pause   F10 hide help", small,
               right - 64);
    } else {
        fitted(32, 104, s.viewName + "   /   Tab next view   /   F1-F8 choose a view", body,
               right - 64);
        fitted(32, 79,
               compact ? "WASD move   Arrows look   Q/E height   G meadow   H barn"
                       : "WASD move   Arrows / right-drag look   Q/E height   G meadow   H barn   "
                         "V overview",
               small, right - 64);
        fitted(32, 55,
               "N day/night   P pause   C cut   R regrow   B study   L/T/F display   F10 help   "
               "Esc exit",
               small, right - 64);
        std::ostringstream status;
        status << (s.cutting ? "Cutting on" : "Cutting off") << "   /   " << s.cutCount
               << " / 2300 blades cut";
        if (s.cutting && s.height >= 2.6f)
            status << "   |   G: enter meadow";
        else if (s.cutting)
            status << "   |   Walk to mow";
        glColor3f(.79f, .83f, .60f);
        fitted(32, 30, status.str(), small, right - 64);
    }
    if (s.diagram && !s.focusedView && width >= 800 && height >= 550) {
        float x = right - 286, y = 144;
        panel(x, y, 270, 250);
        glColor3f(.96f, .89f, .69f);
        fitted(x + 16, y + 220, "Bend study / B to hide", 24, 238);
        glColor3f(.66f, .73f, .65f);
        fitted(x + 16, y + 196, "Same root, more movement at the tip", 18, 238);
        float bx = x + 72, by = y + 58, scale = 115;
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
        Draw::text(bx - 23, by - 20, "Fixed root", 18);
        fitted(x + 16, y + 14, "Offset = wind x (height / total)^2", 18, 238);
    }
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
}

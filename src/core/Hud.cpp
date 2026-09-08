#include "Hud.h"
#include "Text.h"
#include "../utils/Helpers.h"
#include "../utils/Font.h"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace {
void panel(float x, float y, float width, float height) {
    glColor4f(.075f, .14f, .13f, .82f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}
void fitted(float x, float y, const std::string& value, float size, float width) {
    Draw::text(x, y, value,
               std::min(size, size * width / std::max(Font::width(value, size), 1.0f)));
}
} // namespace
void Hud::render(int width, int windowHeight, const WindSystem& wind, HudState s) const {
    if (width <= 0 || windowHeight <= 0)
        return;
    // Anchor the panels to the window edges without changing the scene viewport.
    float scale = std::min({1.0f, float(width) / 960, float(windowHeight) / 640});
    float right = float(width) / scale, top = float(windowHeight) / scale - 12;
    bool wide = right >= 1000;
    float studyX = right - 280, leftWidth = std::min(360.0f, right - 304);
    float leftTextWidth = leftWidth - 28, studyTextX = studyX + 14;
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
    gluOrtho2D(0, right, 0, float(windowHeight) / scale);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    panel(12, top - 150, leftWidth, 150);
    glColor3f(.96f, .89f, .68f);
    fitted(26, top - 34, Text::get("farm.title"), 34, leftTextWidth);
    glColor3f(.76f, .83f, .73f);
    fitted(26, top - 57, Text::get("farm.author"), 18, leftTextWidth);
    glColor3f(.96f, .90f, .73f);
    fitted(26, top - 82,
           Text::format("view.status", {{"view", s.viewName},
                                        {"period", Text::get(s.night ? "night" : "day")},
                                        {"count", std::to_string(s.sleepingCows)}}),
           22, leftTextWidth);
    glColor3f(.78f, .84f, .75f);
    fitted(26, top - 107, s.paused ? Text::get("animation.paused") : s.herdStatus, 18,
           leftTextWidth);

    fitted(26, top - 132, s.farmerStatus, 18, leftTextWidth);

    float controlsWidth = wide ? (right - 76) / 2 : right - 52;
    float controlsRight = right / 2 + 12;
    panel(12, 12, right - 24, wide ? 68.0f : 96.0f);
    glColor3f(.96f, .90f, .73f);
    fitted(26, wide ? 56.0f : 85.0f, Text::get("help.move"), 20, controlsWidth);
    fitted(wide ? controlsRight : 26, wide ? 56.0f : 65.0f, Text::get("help.actions"), 20,
           controlsWidth);
    fitted(26, wide ? 30.0f : 45.0f, Text::get("help.views"), 20, controlsWidth);
    std::string status = Text::format(
        "cutting.status", {{"mode", Text::get(s.cutting ? "cutting.on" : "cutting.off")},
                           {"count", std::to_string(s.cutCount)}});
    if (s.cutting && s.height >= 2.6f)
        status = Text::get("cutting.ground");
    if (!s.textNotice.empty())
        status = s.textNotice;
    glColor3f(.79f, .83f, .60f);
    fitted(wide ? controlsRight : 26, wide ? 30.0f : 25.0f, status, 18, controlsWidth);

    std::ostringstream value, maximum;
    value << std::fixed << std::setprecision(1) << wind.getStrength();
    maximum << Constants::WIND_MAX;
    panel(studyX, top - 104, 268, 104);
    glColor3f(.96f, .89f, .68f);
    fitted(studyTextX, top - 31,
           Text::format("wind.title", {{"value", value.str()}, {"maximum", maximum.str()}}), 26,
           240);
    for (int i = 0; i < 12; ++i) {
        float x = studyTextX + float(i) * 20;
        bool active = float(i) < wind.getStrength() * 4;
        glColor3f(active ? .82f : .25f, active ? .73f : .34f, active ? .37f : .28f);
        glBegin(GL_QUADS);
        glVertex2f(x, top - 50);
        glVertex2f(x + 15, top - 50);
        glVertex2f(x + 15, top - 42);
        glVertex2f(x, top - 42);
        glEnd();
    }
    glColor3f(.78f, .84f, .75f);
    fitted(studyTextX, top - 71, Text::get("wind.controls"), 17, 240);
    fitted(studyTextX, top - 93,
           Text::get(wind.getStrength() == 0 ? "wind.still" : "wind.direction"), 17, 240);

    float studyBottom = wide ? 92.0f : 120.0f;
    float studyTop = studyBottom + (s.diagram ? 136.0f : 36.0f);
    panel(studyX, studyBottom, 268, studyTop - studyBottom);
    glColor3f(.96f, .89f, .68f);
    fitted(studyTextX, studyTop - 25, Text::get(s.diagram ? "study.title" : "study.show"), 20, 240);
    if (s.diagram) {
        float bx = studyTextX + 24, by = studyBottom + 30, graphScale = 64;
        glLineWidth(1);
        glColor3f(.42f, .49f, .44f);
        glBegin(GL_LINES);
        glVertex2f(bx, by);
        glVertex2f(bx, by + graphScale);
        glEnd();
        glLineWidth(3);
        glColor3f(.88f, .77f, .36f);
        glBegin(GL_LINE_STRIP);
        const Vec3 root{-10, 0, 10};
        for (int i = 0; i <= 10; ++i) {
            Vec3 offset = wind.bend(root, 1, float(i) / 10, .8f) - root;
            glVertex2f(bx + offset.x * graphScale, by + offset.y * graphScale);
        }
        glEnd();
        glColor3f(.78f, .84f, .75f);
        fitted(studyTextX + 102, studyBottom + 82, Text::get("study.root"), 18, 138);
        fitted(studyTextX + 102, studyBottom + 58, Text::get("study.tip"), 18, 138);
        fitted(studyTextX, studyBottom + 12, Text::get("study.formula"), 17, 240);
    }
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
}

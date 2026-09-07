#include "Font.h"
#include "FontMetrics.h"
#include "../core/Texture.h"
#include <algorithm>

namespace {
GLuint atlas = 0;
unsigned char printable(unsigned char c) {
    return c >= 32 && c <= 126 ? c : '?';
}
} // namespace
namespace Font {
void initialize(const std::filesystem::path& executable) {
    auto base = std::filesystem::absolute(executable).parent_path() / "assets";
    if (!std::filesystem::exists(base))
        base = "assets";
    atlas = TextureSet::loadBmp(base / "fonts/handwriting.bmp", true);
}
bool available() {
    return atlas != 0;
}
float width(const std::string& value, float size) {
    float result = 0;
    for (unsigned char raw : value) {
        unsigned char c = printable(raw);
        result += atlas ? float(FontMetrics::advances[c - 32]) / (64 * 48)
                        : float(glutStrokeWidth(GLUT_STROKE_ROMAN, c)) * .65f / 100;
    }
    return result * size;
}
void draw(float x, float y, const std::string& value, float size) {
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_TEXTURE_BIT |
                 GL_CURRENT_BIT | GL_LINE_BIT | GL_POLYGON_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDisable(GL_FOG);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (!atlas) {
        // Missing assets still leave every control and sign readable.
        glDisable(GL_TEXTURE_2D);
        glLineWidth(1.5f);
        glPushMatrix();
        glTranslatef(x, y, 0);
        glScalef(size * .65f / 100, size * .65f / 100, 1);
        for (unsigned char c : value)
            glutStrokeCharacter(GLUT_STROKE_ROMAN, printable(c));
        glPopMatrix();
    } else {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, atlas);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        float scale = size / 48;
        glBegin(GL_QUADS);
        for (unsigned char raw : value) {
            int index = printable(raw) - 32;
            float u = float(index % 16) / 16, v = 1 - float(index / 16) * 80 / 512;
            float left = x - 8 * scale, bottom = y - 20 * scale;
            glTexCoord2f(u, v - 80.0f / 512);
            glVertex2f(left, bottom);
            glTexCoord2f(u + 1.0f / 16, v - 80.0f / 512);
            glVertex2f(left + 64 * scale, bottom);
            glTexCoord2f(u + 1.0f / 16, v);
            glVertex2f(left + 64 * scale, y + 60 * scale);
            glTexCoord2f(u, v);
            glVertex2f(left, y + 60 * scale);
            x += float(FontMetrics::advances[index]) / 64 * scale;
        }
        glEnd();
    }
    glPopAttrib();
}
} // namespace Font

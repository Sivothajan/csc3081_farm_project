#include "Helpers.h"
#include "Font.h"
#include <algorithm>
namespace Draw {
void box(Vec3 p, Vec3 s, Vec3 c) {
    glColor3f(c.x, c.y, c.z);
    glPushMatrix();
    glTranslatef(p.x, p.y, p.z);
    glScalef(s.x, s.y, s.z);
    // Explicit faces carry normals and UVs, unlike GLUT cubes without texture coordinates.
    const Vec3 vertices[8] = {{-.5f, -.5f, -.5f}, {.5f, -.5f, -.5f}, {.5f, .5f, -.5f},
                              {-.5f, .5f, -.5f},  {-.5f, -.5f, .5f}, {.5f, -.5f, .5f},
                              {.5f, .5f, .5f},    {-.5f, .5f, .5f}};
    const int faces[6][4] = {{4, 5, 6, 7}, {1, 0, 3, 2}, {0, 4, 7, 3},
                             {5, 1, 2, 6}, {3, 7, 6, 2}, {0, 1, 5, 4}};
    const Vec3 normals[6] = {{0, 0, 1}, {0, 0, -1}, {-1, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, -1, 0}};
    const float uv[4][2] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    glBegin(GL_QUADS);
    for (int f = 0; f < 6; ++f) {
        glNormal3f(normals[f].x, normals[f].y, normals[f].z);
        for (int k = 0; k < 4; ++k) {
            glTexCoord2fv(uv[k]);
            const Vec3 v = vertices[faces[f][k]];
            glVertex3f(v.x, v.y, v.z);
        }
    }
    glEnd();
    glPopMatrix();
}
void ellipsoid(Vec3 p, Vec3 s, Vec3 c, int slices, int stacks) {
    glColor3f(c.x, c.y, c.z);
    glPushMatrix();
    glTranslatef(p.x, p.y, p.z);
    glScalef(s.x, s.y, s.z);
    glutSolidSphere(1, slices, stacks);
    glPopMatrix();
}
void beam(Vec3 a, Vec3 b, float radius, Vec3 color, int sides) {
    Vec3 d = b - a;
    float n = length(d);
    if (n < .00001f)
        return;
    glColor3f(color.x, color.y, color.z);
    glPushMatrix();
    glTranslatef(a.x, a.y, a.z);
    // GLUT cylinders point along +Z. Align their axis with the branch/rail direction.
    float angle = std::acos(std::clamp(d.z / n, -1.0f, 1.0f)) * 180 / Constants::PI;
    if (std::abs(d.x) + std::abs(d.y) > .00001f)
        glRotatef(angle, -d.y, d.x, 0);
    else if (d.z < 0)
        glRotatef(180, 0, 1, 0);
    glutSolidCylinder(radius, n, sides, 1);
    glPopMatrix();
}
void ground(float x0, float z0, float x1, float z1, float y, Vec3 c, float repeat) {
    glColor3f(c.x, c.y, c.z);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0);
    glVertex3f(x0, y, z0);
    glTexCoord2f(0, repeat);
    glVertex3f(x0, y, z1);
    glTexCoord2f(repeat, repeat);
    glVertex3f(x1, y, z1);
    glTexCoord2f(repeat, 0);
    glVertex3f(x1, y, z0);
    glEnd();
}
void triangle(Vec3 a, Vec3 b, Vec3 c) {
    Vec3 n = normalized(cross(b - a, c - a));
    glNormal3f(n.x, n.y, n.z);
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glVertex3f(c.x, c.y, c.z);
}
void text(float x, float y, const std::string& value, float size) {
    Font::draw(x, y, value, size);
}
void plaque(Vec3 center, const std::string& title, float width, float height,
            const std::string& subtitle) {
    GLfloat view[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, view);
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(center.x, center.y, center.z);
    // Keep the board upright but face its entire surface toward the camera.
    glRotatef(std::atan2(-view[8], view[0]) * 180 / Constants::PI, 0, 1, 0);
    box({0, 0, 0}, {width, height, .12f}, {.58f, .42f, .23f});
    box({0, 0, .067f}, {width - .10f, height - .10f, .035f}, {.16f, .27f, .22f});
    glTranslatef(0, 0, .09f);
    float size = std::min(height * (subtitle.empty() ? .66f : .46f),
                          (width - .28f) / std::max(Font::width(title, 1), .1f));
    glColor3f(.99f, .93f, .75f);
    text(-Font::width(title, size) / 2, subtitle.empty() ? -size * .33f : .015f, title, size);
    if (!subtitle.empty()) {
        float small = std::min(height * .27f, (width - .28f) / Font::width(subtitle, 1));
        glColor3f(.85f, .84f, .65f);
        text(-Font::width(subtitle, small) / 2, -height * .32f, subtitle, small);
    }
    glPopMatrix();
    glPopAttrib();
}
void sign(Vec3 p, const std::string& title, const std::string& subtitle) {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_TEXTURE_2D);
    box({p.x, p.y + 1, p.z}, {.14f, 2, .14f}, {.40f, .28f, .16f});
    plaque(p + Vec3{0, 1.85f, 0}, title, 3.2f, 1, subtitle);
    glPopAttrib();
}
} // namespace Draw

#include "Helpers.h"
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
void text(float x, float y, const std::string& value, void* font) {
    glRasterPos2f(x, y);
    for (unsigned char c : value)
        glutBitmapCharacter(font, c);
}
void sign(Vec3 p, const std::string& title) {
    box({p.x, p.y + .8f, p.z}, {.12f, 1.6f, .12f}, {.40f, .28f, .16f});
    box({p.x, p.y + 1.5f, p.z}, {2.2f, .65f, .12f}, {.19f, .29f, .24f});
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    float textWidth = float(
        glutStrokeLength(GLUT_STROKE_ROMAN, reinterpret_cast<const unsigned char*>(title.c_str())));
    float scale = std::min(.0023f, 1.94f / std::max(textWidth, 1.0f));
    glColor3f(.97f, .91f, .70f);
    glLineWidth(1.5f);
    glPushMatrix();
    glTranslatef(p.x - textWidth * scale * .5f, p.y + 1.41f, p.z + .075f);
    glScalef(scale, scale, scale);
    for (unsigned char c : title)
        glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
    glPopMatrix();
    glPopAttrib();
}
} // namespace Draw

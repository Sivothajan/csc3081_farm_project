#pragma once
#include <GL/freeglut.h>
#include "MathUtils.h"
#include <string>
namespace Draw {
void box(Vec3 center, Vec3 size, Vec3 color);
void ellipsoid(Vec3 center, Vec3 size, Vec3 color, int slices = 12, int stacks = 8);
void beam(Vec3 from, Vec3 to, float radius, Vec3 color, int sides = 8);
void ground(float x0, float z0, float x1, float z1, float y, Vec3 color, float repeat = 1);
void triangle(Vec3 a, Vec3 b, Vec3 c);
void text(float x, float y, const std::string& value, float size = 18);
void plaque(Vec3 center, const std::string& title, float width, float height,
            const std::string& subtitle = "");
void sign(Vec3 position, const std::string& title, const std::string& subtitle = "");
} // namespace Draw

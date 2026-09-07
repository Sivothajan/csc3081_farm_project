#pragma once
#include <cmath>
#include "Constants.h"
struct Vec3 {
    float x = 0, y = 0, z = 0;
    Vec3 operator+(Vec3 b) const { return {x + b.x, y + b.y, z + b.z}; }
    Vec3 operator-(Vec3 b) const { return {x - b.x, y - b.y, z - b.z}; }
    Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
};
inline float radians(float degrees) {
    return degrees * Constants::PI / 180.0f;
}
inline float length(Vec3 v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
inline Vec3 normalized(Vec3 v) {
    float n = length(v);
    return n > 0.00001f ? v * (1.0f / n) : Vec3{0, 1, 0};
}
inline Vec3 cross(Vec3 a, Vec3 b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}
inline float distanceSquared2D(Vec3 a, Vec3 b) {
    return (a.x - b.x) * (a.x - b.x) + (a.z - b.z) * (a.z - b.z);
}

#include "Camera.h"
#include <GL/freeglut.h>
#include <algorithm>
void Camera::update(float dt, const std::array<bool, 256>& keys,
                    const std::array<bool, 256>& special) {
    look((float(special[GLUT_KEY_RIGHT]) - float(special[GLUT_KEY_LEFT])) * 65 * dt,
         (float(special[GLUT_KEY_UP]) - float(special[GLUT_KEY_DOWN])) * 55 * dt);
    const float y = radians(yaw);
    Vec3 forward{std::cos(y), 0, std::sin(y)}, right{-std::sin(y), 0, std::cos(y)};
    Vec3 move = forward * (float(keys['w']) - float(keys['s'])) +
                right * (float(keys['d']) - float(keys['a']));
    move.y = float(keys['e']) - float(keys['q']);
    if (length(move) > 0)
        position = position + normalized(move) * (Constants::CAMERA_SPEED * dt);
    position.x = std::clamp(position.x, -34.0f, 34.0f);
    position.z = std::clamp(position.z, -34.0f, 38.0f);
    position.y = std::clamp(position.y, Constants::EYE_MIN, 30.0f);
}
void Camera::look(float dx, float dy) {
    yaw = std::remainder(yaw + dx, 360.0f);
    pitch = std::clamp(pitch + dy, -80.0f, 80.0f);
}
void Camera::apply() const {
    float y = radians(yaw), p = radians(pitch);
    Vec3 direction{std::cos(y) * std::cos(p), std::sin(p), std::sin(y) * std::cos(p)};
    Vec3 target = position + direction;
    gluLookAt(position.x, position.y, position.z, target.x, target.y, target.z, 0, 1, 0);
}
void Camera::overview() {
    position = {33, 27, 37};
    yaw = -132;
    pitch = -29.5f;
}
void Camera::fieldView() {
    position = {-10, 1.65f, 17};
    yaw = -90;
    pitch = -12;
}

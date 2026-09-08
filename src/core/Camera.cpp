#include "Camera.h"
#include "Text.h"
#include "../structures/Barn.h"
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
    if (length(move) > 0) {
        stopFollowing();
        customName = "view.free";
        position = position + normalized(move) * (Constants::CAMERA_SPEED * dt);
    }
    position.x = std::clamp(position.x, -34.0f, 34.0f);
    position.z = std::clamp(position.z, -34.0f, 38.0f);
    position.y = std::clamp(position.y, Constants::EYE_MIN, 30.0f);
}
void Camera::look(float dx, float dy) {
    if (dx != 0 || dy != 0) {
        followActive = false;
        customName = "view.free";
    }
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
    setView(View::Overview);
}
void Camera::fieldView() {
    followActive = false;
    selected = View::Overview;
    customName = "view.meadow";
    position = {-10, 1.65f, 17};
    yaw = -90;
    pitch = -12;
}
void Camera::aimAt(Vec3 target) {
    Vec3 direction = target - position;
    yaw = std::atan2(direction.z, direction.x) * 180 / Constants::PI;
    pitch = std::atan2(direction.y, std::hypot(direction.x, direction.z)) * 180 / Constants::PI;
}
void Camera::setView(View view) {
    if (view < View::Overview || view >= View::Count)
        return;
    selected = view;
    followActive = false;
    customName = nullptr;
    Vec3 target{0, 0, 0};
    switch (view) {
    case View::Overview:
        position = {33, 27, 37};
        target = {0, 0, 0};
        break;
    case View::Front:
        position = {0, 23, 38};
        target = {0, 0, 0};
        break;
    case View::Back:
        position = {0, 23, -34};
        target = {0, 0, 3};
        break;
    case View::Left:
        position = {-34, 23, 2};
        target = {3, 0, 2};
        break;
    case View::Right:
        position = {34, 23, 2};
        target = {-3, 0, 2};
        break;
    case View::Barn:
        position = Barn::viewPosition();
        target = {-17, 1.2f, -9.8f};
        break;
    case View::BarnLeft:
        position = {-19.8f, 3.1f, -5.0f};
        target = {-16.6f, 1.45f, -9.8f};
        break;
    case View::BarnRight:
        position = {-14.2f, 3.1f, -5.0f};
        target = {-17.4f, 1.45f, -9.8f};
        break;
    case View::Count:
        break;
    }
    aimAt(target);
}
void Camera::cycleView() {
    setView(static_cast<View>((static_cast<int>(selected) + 1) % static_cast<int>(View::Count)));
}
void Camera::followFarmer(Vec3 target, Vec3 forward) {
    followActive = true;
    customName = "view.farmer";
    Vec3 right{forward.z, 0, -forward.x};
    position = target + forward * 4.2f + right * 2.2f + Vec3{0, 2.65f, 0};
    aimAt(target + Vec3{0, 1.4f, 0});
}
void Camera::stopFollowing() {
    followActive = false;
    customName = "view.free";
}
const std::string& Camera::viewName() const {
    if (customName)
        return Text::get(customName);
    const char* names[]{"view.overview", "view.front", "view.back",      "view.left",
                        "view.right",    "view.barn",  "view.barn_left", "view.barn_right"};
    return Text::get(names[static_cast<int>(selected)]);
}

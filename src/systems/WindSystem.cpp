#include "WindSystem.h"
#include <algorithm>
void WindSystem::update(float dt) {
    time += std::max(dt, 0.0f);
}
void WindSystem::setStrength(float value) {
    strength = std::clamp(value, 0.0f, Constants::WIND_MAX);
}
float WindSystem::getDisplacement(float x, float z, float phase, float u, float frequency) const {
    u = std::clamp(u, 0.0f, 1.0f);
    const float gust = .74f + .26f * std::sin(time * .57f + x * .11f + z * .09f);
    const float wave = .65f +
                       .26f * std::sin(time * speed * frequency + x * .20f + z * .15f + phase) +
                       .09f * std::sin(time * 3.7f * frequency + phase * 1.7f);
    // A prevailing wind plus overlapping waves avoids perfectly synchronized plants.
    // u^2 is zero at the root; its derivative is also zero there. This bends vertices,
    // rather than rotating the plant as a rigid object around its base.
    return strength * gust * wave * u * u;
}
Vec3 WindSystem::bend(Vec3 base, float height, float u, float phase, float flexibility,
                      float frequency) const {
    return base + Vec3{0, height * u, 0} +
           direction() *
               (height * .32f * flexibility * getDisplacement(base.x, base.z, phase, u, frequency));
}

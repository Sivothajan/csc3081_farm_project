#pragma once
#include "../utils/MathUtils.h"
class WindSystem {
  public:
    void update(float dt);
    void setStrength(float value);
    float getStrength() const { return strength; }
    float getTime() const { return time; }
    Vec3 direction() const { return {0.8944272f, 0, 0.4472136f}; }
    float getDisplacement(float x, float z, float phase, float heightFactor,
                          float frequency = 1) const;
    Vec3 bend(Vec3 base, float height, float u, float phase, float flexibility = 1,
              float frequency = 1) const;

  private:
    float strength = 1.2f, time = 0, speed = 2.2f;
};

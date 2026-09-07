#pragma once
#include "../utils/MathUtils.h"
class Cow {
  public:
    Cow(Vec3 center, float offset) : center(center), offset(offset) {}
    void update(float time);
    void render() const;
    Vec3 position() const { return location; }

  private:
    Vec3 center, location;
    float offset, heading = 0, legSwing = 0, headDip = 0, time = 0;
};

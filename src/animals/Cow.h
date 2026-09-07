#pragma once
#include "../utils/MathUtils.h"
class Cow {
  public:
    Cow(Vec3 center, float offset) : center(center), offset(offset) {}
    void update(float time);
    bool walkTo(Vec3 target, float dt);
    void follow(Vec3 position, Vec3 direction, float dt);
    bool face(Vec3 direction, float dt);
    void rest(bool asleep, float dt);
    void render() const;
    Vec3 position() const { return location; }
    bool sleeping() const { return sleepAmount >= .99f; }

  private:
    Vec3 center, location;
    float offset, heading = 0, legSwing = 0, headDip = 0, time = 0;
    float sleepAmount = 0;
};

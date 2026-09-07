#pragma once
#include "../utils/MathUtils.h"
class Barn {
  public:
    void update(float dt, bool open);
    void light(float nightAmount, bool enabled) const;
    void render(float nightAmount) const;
    float doorOpening() const { return opening; }
    bool passable() const { return opening >= .99f; }
    static Vec3 aisle(int stall) { return {-17 + float(stall - 1) * 1.5f, 0, -5.7f}; }
    static Vec3 bed(int stall) { return {-17 + float(stall - 1) * 1.5f, 0, -9.6f}; }
    static Vec3 viewPosition() { return {-17, 3.1f, -4.55f}; }

  private:
    float opening = 0;
};

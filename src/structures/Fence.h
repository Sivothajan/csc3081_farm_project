#pragma once
#include "../utils/MathUtils.h"
class Fence {
  public:
    void render() const;

  private:
    void segment(Vec3 from, Vec3 to) const;
};

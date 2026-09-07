#pragma once
#include "../utils/MathUtils.h"
class HayBale {
  public:
    void render() const;
    static void bale(Vec3 center, float scale = 1);
};

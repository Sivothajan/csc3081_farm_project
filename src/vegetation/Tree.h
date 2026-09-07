#pragma once
#include "../systems/WindSystem.h"
class Tree {
  public:
    void render(const WindSystem& wind) const;

  private:
    void one(Vec3 p, float scale, float phase, const WindSystem& wind) const;
};

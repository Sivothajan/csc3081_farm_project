#pragma once
#include <vector>
#include "../systems/WindSystem.h"
struct CropStalk {
    Vec3 base;
    float height, phase;
};
class Crop {
  public:
    void initialize();
    void render(const WindSystem& wind) const;

  private:
    std::vector<CropStalk> stalks;
};

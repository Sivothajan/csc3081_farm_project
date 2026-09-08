#pragma once
#include <vector>
#include <array>
#include "../systems/WindSystem.h"
struct CropStalk {
    Vec3 base;
    float height, phase;
};
class Crop {
  public:
    static constexpr std::array<float, 11> rowCenters{-7.3f, -6.15f, -5.0f, -3.85f, -2.7f, 3.05f,
                                                      4.2f,  5.35f,  6.5f,  7.65f,  8.8f};
    void initialize();
    void render(const WindSystem& wind) const;

  private:
    std::vector<CropStalk> stalks;
};

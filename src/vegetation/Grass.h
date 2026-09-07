#pragma once
#include <vector>
#include "../systems/WindSystem.h"
struct GrassBlade {
    Vec3 base;
    float originalHeight, currentHeight, width, orientation, phase, response;
    bool cut = false, cuttable = true;
};
class Grass {
  public:
    void initialize();
    void render(const WindSystem& wind) const;
    int cutNear(Vec3 position, float radius);
    void reset();
    int cutCount() const;
    const std::vector<GrassBlade>& blades() const { return field; }

  private:
    std::vector<GrassBlade> field;
};

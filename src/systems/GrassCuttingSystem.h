#pragma once
#include "../vegetation/Grass.h"
class GrassCuttingSystem {
  public:
    bool enabled = false;
    void update(Grass& grass, Vec3 camera) const;
    void render(Vec3 camera) const;
};

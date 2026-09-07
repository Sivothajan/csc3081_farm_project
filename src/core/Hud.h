#pragma once
#include "../systems/WindSystem.h"
struct HudState {
    bool cutting, paused, diagram;
    int cutCount;
    float height;
};
class Hud {
  public:
    void render(int width, int height, const WindSystem& wind, HudState state) const;
};

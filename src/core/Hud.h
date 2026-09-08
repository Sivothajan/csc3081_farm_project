#pragma once
#include "../systems/WindSystem.h"
#include <string>
struct HudState {
    bool cutting, paused, diagram;
    int cutCount;
    float height;
    std::string herdStatus;
    int sleepingCows;
    bool night;
    std::string viewName;
    std::string textNotice;
    std::string farmerStatus;
};
class Hud {
  public:
    void render(int width, int height, const WindSystem& wind, HudState state) const;
};

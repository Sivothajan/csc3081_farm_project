#pragma once
class AnimationSystem {
  public:
    bool paused = false;
    float time = 0;
    float update(float dt) {
        if (paused)
            return 0;
        time += dt;
        return dt;
    }
};

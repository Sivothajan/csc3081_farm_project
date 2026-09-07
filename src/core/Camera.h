#pragma once
#include <array>
#include "../utils/MathUtils.h"
class Camera {
  public:
    enum class View { Overview, Front, Back, Left, Right, Barn, BarnLeft, BarnRight, Count };
    Vec3 position{33, 27, 37};
    float yaw = -132.0f, pitch = -29.5f;
    void update(float dt, const std::array<bool, 256>& keys, const std::array<bool, 256>& special);
    void apply() const;
    void look(float dx, float dy);
    void overview();
    void fieldView();
    void setView(View view);
    void cycleView();
    const char* viewName() const;
    bool focusedView() const { return selected != View::Overview; }

  private:
    View selected = View::Overview;
    const char* customName = nullptr;
    void aimAt(Vec3 target);
};

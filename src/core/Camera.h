#pragma once
#include <array>
#include <string>
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
    void followFarmer(Vec3 target, Vec3 forward);
    bool followingFarmer() const { return followActive; }
    void stopFollowing();
    const std::string& viewName() const;

  private:
    View selected = View::Overview;
    const char* customName = nullptr;
    bool followActive = false;
    void aimAt(Vec3 target);
};

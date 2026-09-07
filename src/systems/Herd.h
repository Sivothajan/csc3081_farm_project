#pragma once
#include "../animals/Cow.h"
#include "../structures/Barn.h"
#include <array>
#include <string>

class Herd {
  public:
    enum class Phase { Grazing, Gathering, Entering, Sleeping, Leaving, Returning };
    Herd();
    void update(float dt, bool night, Barn& barn);
    void render() const;
    const std::array<Cow, 3>& animals() const { return cows; }
    Phase phase() const { return state; }
    int sleepingCount() const;
    std::string status(bool night) const;

  private:
    std::array<Cow, 3> cows{{Cow({9, 0, 10}, 0), Cow({16, 0, 12}, 12), Cow({9, 0, 18}, 20)}};
    std::array<Vec3, 3> fieldPositions{};
    std::array<int, 3> order{{0, 1, 2}};
    std::array<float, 3> exitProgress{};
    std::array<bool, 3> turnedFromStall{}, turnedToDoor{};
    Phase state = Phase::Grazing;
    float fieldTime = 0, convoyDistance = 0, stallDistance = 0;
    int nextCow = 0;
    bool enteringStall = false;
    void step(float dt, bool night, Barn& barn);
    void enter(float dt, const Barn& barn);
    void leave(float dt, const Barn& barn);
};

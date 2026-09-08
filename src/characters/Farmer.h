#pragma once
#include "../core/Texture.h"
#include "../utils/MathUtils.h"
#include <vector>

// A distance-driven skeleton on a closed, rounded walking route. Simulation
// does not depend on rendering, so captures and live playback use the same pose.
class Farmer {
    friend class Verification;

  public:
    Farmer();
    void initialize(const std::filesystem::path& executable);
    void update(float dt, bool night = false);
    void render(bool texturesEnabled) const;
    void light(float nightAmount, bool enabled) const;
    Vec3 position() const { return location; }
    Vec3 forward() const { return {std::sin(radians(heading)), 0, std::cos(radians(heading))}; }
    void toggleWalking() { walkingEnabled = !walkingEnabled; }
    const char* statusKey() const;

  private:
    struct Sample {
        Vec3 point;
        double time, distance;
        float rampIn = 0, rampOut = 0;
    };
    std::vector<Sample> route;
    std::vector<Sample> dayRoute, nightRoute;
    enum class Routine { Day, ToBarn, Night, ToFields };
    Routine routine = Routine::Day;
    Vec3 location;
    double clock = 0, travelled = 0, completedDistance = 0;
    float heading = 90, gait = 0, motion = 1, inspection = 0;
    bool walkingEnabled = true;
    bool nightRequested = false, blendHeading = false;
    float entryHeading = 90;
    GLuint portrait = 0;
    void makeRoute(bool night);
    void transfer(bool toNight);
    void activate(bool night);
    void pose();
    void head(bool texturesEnabled) const;
};

#pragma once
#include <string>
class Scene;
struct RunOptions {
    std::string capture, view = "overview";
    int width = 1280, height = 800;
    int benchmarkFrames = 0;
    float time = 4, wind = 1.2f;
    bool selfTest = false, night = false, cutDemo = false, noTextures = false, wireframe = false;
    static RunOptions parse(int argc, char** argv);
};
class Verification {
  public:
    static void prepare(Scene& scene, const RunOptions& options);
    static bool run(Scene& scene);
};

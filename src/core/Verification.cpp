#include "Verification.h"
#include "Scene.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
RunOptions RunOptions::parse(int argc, char** argv) {
    RunOptions o;
    for (int i = 1; i < argc; ++i) {
        std::string key = argv[i];
        auto next = [&]() -> std::string {
            if (i + 1 >= argc)
                throw std::runtime_error("Missing value for " + key);
            return argv[++i];
        };
        if (key == "--capture")
            o.capture = next();
        else if (key == "--view")
            o.view = next();
        else if (key == "--time")
            o.time = std::stof(next());
        else if (key == "--wind")
            o.wind = std::stof(next());
        else if (key == "--width")
            o.width = std::stoi(next());
        else if (key == "--height")
            o.height = std::stoi(next());
        else if (key == "--benchmark")
            o.benchmarkFrames = std::stoi(next());
        else if (key == "--self-test")
            o.selfTest = true;
        else if (key == "--night")
            o.night = true;
        else if (key == "--cut-demo")
            o.cutDemo = true;
        else if (key == "--no-textures")
            o.noTextures = true;
        else if (key == "--wireframe")
            o.wireframe = true;
        else
            throw std::runtime_error("Unknown option: " + key);
    }
    if (o.width < 640 || o.width > 3840 || o.height < 480 || o.height > 2160 ||
        !std::isfinite(o.time) || o.time < 0 || o.time > 86400 || !std::isfinite(o.wind))
        throw std::runtime_error(
            "Invalid dimensions (640..3840 x 480..2160), time (0..86400), or wind.");
    if (o.view != "overview" && o.view != "meadow" && o.view != "crops" && o.view != "cows" &&
        o.view != "cutting")
        throw std::runtime_error("Unknown view preset.");
    if (o.benchmarkFrames < 0 || o.benchmarkFrames > 36000)
        throw std::runtime_error("Benchmark must be 0..36000 frames.");
    return o;
}
void Verification::prepare(Scene& scene, const RunOptions& o) {
    scene.wind.setStrength(o.wind);
    scene.update(o.time);
    if (o.view == "meadow")
        scene.key('g');
    if (o.view == "crops") {
        scene.camera.position = {-5, 2.2f, -2.7f};
        scene.camera.yaw = -90;
        scene.camera.pitch = -8;
    }
    if (o.view == "cows") {
        scene.camera.position = {4, 3.1f, 18};
        scene.camera.yaw = -24;
        scene.camera.pitch = -12;
    }
    if (o.view == "cutting") {
        scene.camera.position = {-11, 5.5f, 22};
        scene.camera.yaw = -90;
        scene.camera.pitch = -28;
    }
    if (o.cutDemo) {
        scene.key('c');
        for (float z = 18; z > 6; z -= .3f)
            scene.cutting.update(scene.grass, {-11, 1.65f, z});
    }
    if (o.night) {
        scene.key('n');
        scene.sky.nightAmount = 1;
    }
    if (o.noTextures)
        scene.key('t');
    if (o.wireframe)
        scene.key('f');
}
bool Verification::run(Scene& scene) {
    int failures = 0, checks = 0;
    auto check = [&](bool pass, const char* name) {
        ++checks;
        std::cout << (pass ? "PASS " : "FAIL ") << name << '\n';
        if (!pass)
            ++failures;
    };
    auto approximately = [](float a, float b) { return std::abs(a - b) < .0001f; };
    WindSystem wind;
    wind.setStrength(3);
    wind.update(4);
    Vec3 root{-10, .05f, 10};
    check(length(wind.bend(root, 1, 0, 1) - root) == 0, "Root is exactly fixed at maximum wind");
    float lower = wind.getDisplacement(root.x, root.z, 1, .25f),
          tip = wind.getDisplacement(root.x, root.z, 1, 1);
    check(tip > 0 && approximately(lower * 16, tip),
          "Quarter-height displacement is 1/16 of tip displacement");
    check(!approximately(wind.getDisplacement(2, 3, 0, 1), wind.getDisplacement(4, 5, 1, 1)),
          "Position and phase vary the wind response");
    wind.setStrength(0);
    Vec3 still = wind.bend(root, 2, 1, 1);
    wind.update(15);
    check(length(wind.bend(root, 2, 1, 1) - still) == 0 && approximately(still.x, root.x),
          "Zero wind stays still across time");
    wind.setStrength(10);
    check(approximately(wind.getStrength(), 3), "Wind clamps at maximum");
    wind.setStrength(-1);
    check(approximately(wind.getStrength(), 0), "Wind clamps at zero");
    WindSystem a, b;
    for (int i = 0; i < 60; ++i)
        a.update(1.0f / 60);
    for (int i = 0; i < 20; ++i)
        b.update(1.0f / 20);
    check(approximately(a.getDisplacement(1, 2, 3, 1), b.getDisplacement(1, 2, 3, 1)),
          "Wind agrees at 20 and 60 updates per second");
    Grass grass, other;
    grass.initialize();
    other.initialize();
    check(grass.blades().size() == 2800 &&
              length(grass.blades()[0].base - other.blades()[0].base) == 0,
          "Grass has a deterministic 2800-blade layout");
    GrassCuttingSystem cutter;
    Vec3 player{-11, 1.65f, 12};
    cutter.update(grass, player);
    check(grass.cutCount() == 0, "Cutting off preserves grass");
    cutter.enabled = true;
    cutter.update(grass, {-11, 10, 12});
    check(grass.cutCount() == 0, "Flying cannot cut grass");
    cutter.update(grass, player);
    int count = grass.cutCount();
    check(count > 0, "Ground-level cutting shortens nearby grass");
    bool cutCorrect = true;
    for (const auto& blade : grass.blades()) {
        if (blade.cut)
            cutCorrect &= blade.cuttable &&
                          distanceSquared2D(player, blade.base) <
                              Constants::CUT_RADIUS * Constants::CUT_RADIUS &&
                          approximately(blade.currentHeight, blade.originalHeight * .22f);
        else
            cutCorrect &= approximately(blade.currentHeight, blade.originalHeight);
    }
    check(cutCorrect, "Cutting affects only the radius and retains short blades");
    cutter.update(grass, player);
    check(grass.cutCount() == count, "Repeated cutting does not repeatedly shrink blades");
    grass.reset();
    check(grass.cutCount() == 0 &&
              approximately(grass.blades()[0].currentHeight, grass.blades()[0].originalHeight),
          "Reset restores original grass heights");
    Camera camera;
    std::array<bool, 256> keys{}, special{};
    camera.fieldView();
    Vec3 before = camera.position;
    keys['w'] = true;
    camera.update(1, keys, special);
    check(camera.position.z < before.z - 5.9f, "Forward camera movement follows yaw");
    keys = {};
    keys['q'] = true;
    for (int i = 0; i < 100; ++i)
        camera.update(.1f, keys, special);
    check(approximately(camera.position.y, Constants::EYE_MIN), "Camera cannot move below terrain");
    keys = {};
    keys['w'] = true;
    for (int i = 0; i < 200; ++i)
        camera.update(.1f, keys, special);
    check(camera.position.z >= -34 && camera.position.x >= -34,
          "Camera stays within farm exploration bounds");
    camera.look(0, 1000);
    check(camera.pitch <= 80, "Look pitch clamps before the vertical singularity");
    Windmill millA, millB;
    for (int i = 0; i < 60; ++i)
        millA.update(1.0f / 60, 2);
    for (int i = 0; i < 20; ++i)
        millB.update(1.0f / 20, 2);
    check(std::abs(millA.rotation() - millB.rotation()) < .001f,
          "Windmill rotation agrees across update rates");
    float angle = millA.rotation();
    millA.update(10, 0);
    check(approximately(angle, millA.rotation()), "Windmill stops with zero wind");
    Cow cow({10, 0, 10}, 0);
    cow.update(0);
    Vec3 start = cow.position();
    cow.update(5);
    check(length(cow.position() - start) > .5f, "Cow walks along its route");
    cow.update(11);
    start = cow.position();
    cow.update(20);
    check(length(cow.position() - start) < .0001f, "Cow remains in place while grazing");
    scene.key('0');
    check(scene.wind.getStrength() == 0, "Zero key disables wind");
    scene.key('3');
    check(scene.wind.getStrength() == 3, "Three key restores strong wind");
    scene.key('p');
    float time = scene.wind.getTime();
    scene.update(1);
    check(approximately(scene.wind.getTime(), time), "Pause freezes scene animation");
    scene.key('p');
    scene.key('c');
    check(scene.cutting.enabled, "C key enables cutting");
    scene.key('c');
    check(TextureSet::loadBmp("assets/nonexistent.bmp") == 0,
          "Missing texture gracefully returns a color fallback");
    std::filesystem::create_directories("build/verification");
    {
        std::ofstream bad("build/verification/invalid.bmp", std::ios::binary);
        bad << "BMbroken";
    }
    check(TextureSet::loadBmp("build/verification/invalid.bmp") == 0,
          "Malformed BMP is rejected safely");
    check(scene.textures.loadedCount() == 4, "All four bundled material textures load");
    check(glGetError() == GL_NO_ERROR, "Initialization and texture tests leave no OpenGL errors");
    std::cout << checks << " checks, " << failures << " failures\n";
    return failures == 0;
}

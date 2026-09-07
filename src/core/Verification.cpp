#include "Verification.h"
#include "Scene.h"
#include "../utils/Font.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <algorithm>
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
        else if (key == "--morning")
            o.morning = true;
        else if (key == "--cut-demo")
            o.cutDemo = true;
        else if (key == "--no-textures")
            o.noTextures = true;
        else if (key == "--wireframe")
            o.wireframe = true;
        else if (key == "--no-hud")
            o.noHud = true;
        else
            throw std::runtime_error("Unknown option: " + key);
    }
    if (o.width < 640 || o.width > 3840 || o.height < 480 || o.height > 2160 ||
        !std::isfinite(o.time) || o.time < 0 || o.time > 86400 || !std::isfinite(o.wind))
        throw std::runtime_error(
            "Invalid dimensions (640..3840 x 480..2160), time (0..86400), or wind.");
    if (o.view != "overview" && o.view != "meadow" && o.view != "crops" && o.view != "cows" &&
        o.view != "cutting" && o.view != "barn" && o.view != "barn-yard" && o.view != "front" &&
        o.view != "back" && o.view != "left" && o.view != "right" && o.view != "barn-left" &&
        o.view != "barn-right")
        throw std::runtime_error("Unknown view preset.");
    if (o.night && o.morning)
        throw std::runtime_error("Choose either --night or --morning.");
    if (o.benchmarkFrames < 0 || o.benchmarkFrames > 36000)
        throw std::runtime_error("Benchmark must be 0..36000 frames.");
    return o;
}
void Verification::prepare(Scene& scene, const RunOptions& o) {
    scene.wind.setStrength(o.wind);
    if (o.night || o.morning)
        scene.key('n');
    if (o.morning) {
        scene.update(60);
        scene.key('n');
    }
    scene.update(o.time);
    if (o.view == "barn")
        scene.key('h');
    const char* views[]{"overview", "front", "back",      "left",
                        "right",    "barn",  "barn-left", "barn-right"};
    for (int i = 1; i < 8; ++i)
        if (o.view == views[i])
            scene.specialKey(GLUT_KEY_F1 + i);
    if (o.view == "barn-yard") {
        scene.camera.position = {-8, 7, 7};
        scene.camera.yaw = -130;
        scene.camera.pitch = -24;
    }
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
    if (o.noTextures)
        scene.key('t');
    if (o.wireframe)
        scene.key('f');
    if (o.noHud)
        scene.specialKey(GLUT_KEY_F10);
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
    bool validViews = true;
    for (int i = 0; i < int(Camera::View::Count); ++i) {
        scene.specialKey(GLUT_KEY_F1 + i);
        Vec3 p = scene.camera.position;
        validViews &= std::isfinite(scene.camera.yaw) && std::isfinite(scene.camera.pitch) &&
                      std::abs(p.x) <= 34 && p.z >= -34 && p.z <= 38 && p.y >= Constants::EYE_MIN &&
                      p.y <= 30 && std::abs(scene.camera.pitch) < 80;
    }
    check(validViews, "All eight camera shortcuts stay within valid exploration bounds");
    scene.key('\t');
    check(std::string(scene.camera.viewName()) == "Overview",
          "Cycling after the last barn side view returns to the overview");
    auto cowBeforeView = scene.herd.animals()[0].position();
    scene.specialKey(GLUT_KEY_F7);
    scene.specialKey(GLUT_KEY_F8);
    check(length(scene.herd.animals()[0].position() - cowBeforeView) == 0,
          "Changing camera sides leaves the farm simulation untouched");
    scene.specialKey(GLUT_KEY_F10);
    bool hidden = !scene.showHud;
    scene.specialKey(GLUT_KEY_F10);
    check(hidden && scene.showHud, "F10 hides and restores the HUD without changing views");
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
    bool smoothTravel = true, safeDoor = true, singleFile = true, correctGate = true;
    auto advanceHerd = [&](Herd& group, Barn& house, bool night, float seconds) {
        for (int tick = 0; tick < int(std::round(seconds * 30)); ++tick) {
            std::array<Vec3, 3> beforeMove;
            for (size_t i = 0; i < 3; ++i)
                beforeMove[i] = group.animals()[i].position();
            group.update(1.0f / 30, night, house);
            int inDoor = 0;
            for (size_t i = 0; i < 3; ++i) {
                Vec3 p = group.animals()[i].position(), old = beforeMove[i];
                smoothTravel &=
                    std::isfinite(p.x) && std::isfinite(p.z) && length(p - old) <= .081f;
                if (std::abs(p.x + 17) < .85f && std::abs(p.z + 4.2f) < 1.6f) {
                    ++inDoor;
                    safeDoor &= house.passable();
                }
                if ((p.x - 3) * (old.x - 3) < 0)
                    correctGate &= p.z > 13.7f && p.z < 16.3f;
            }
            singleFile &= inDoor <= 1;
        }
    };
    auto inBeds = [](const Herd& group) {
        bool placed = group.phase() == Herd::Phase::Sleeping && group.sleepingCount() == 3;
        for (int i = 0; i < 3; ++i)
            placed &= length(group.animals()[i].position() - Barn::bed(i)) < .001f;
        return placed;
    };
    auto inField = [](const Herd& group) {
        bool placed = group.phase() == Herd::Phase::Grazing && group.sleepingCount() == 0;
        for (const auto& animal : group.animals())
            placed &= animal.position().x > 3 && animal.position().z > 3;
        return placed;
    };
    Herd group;
    Barn house;
    for (int tick = 0; tick < 600 && group.phase() != Herd::Phase::Entering; ++tick)
        group.update(1.0f / 30, true, house);
    std::array<float, 3> queueX;
    bool linedUp = group.phase() == Herd::Phase::Entering;
    for (size_t i = 0; i < 3; ++i) {
        queueX[i] = group.animals()[i].position().x;
        linedUp &= std::abs(group.animals()[i].position().z - 15) < .081f;
    }
    std::sort(queueX.begin(), queueX.end());
    linedUp &= std::abs(queueX[0] - 7) < .081f && std::abs(queueX[1] - 12) < .081f &&
               std::abs(queueX[2] - 17) < .081f;
    check(linedUp, "All cows line up before leaving through the pasture gate");
    advanceHerd(group, house, true, 60);
    check(inBeds(group) && house.doorOpening() == 0,
          "Night puts three sleeping cows in assigned stalls and closes the doors");
    advanceHerd(group, house, false, 70);
    check(inField(group) && house.doorOpening() == 0,
          "Morning returns all cows to grazing and closes the empty barn");
    advanceHerd(group, house, true, .5f);
    advanceHerd(group, house, false, 5);
    check(inField(group), "Day cancels gathering without teleporting cows");
    advanceHerd(group, house, true, 10);
    check(group.phase() == Herd::Phase::Entering, "Night starts a single-file barn journey");
    advanceHerd(group, house, false, 100);
    check(inField(group), "A day request during entry completes safe entry then returns the herd");
    advanceHerd(group, house, true, 60);
    advanceHerd(group, house, false, 8);
    check(group.phase() == Herd::Phase::Leaving, "Morning starts the controlled exit route");
    advanceHerd(group, house, true, 120);
    check(inBeds(group), "A night request during exit is honoured after the passage completes");
    bool requestedNight = true;
    for (int toggle = 0; toggle < 240; ++toggle) {
        requestedNight = !requestedNight;
        advanceHerd(group, house, requestedNight, .1f);
    }
    advanceHerd(group, house, false, 150);
    check(inField(group), "Rapid day/night toggles settle at the latest daytime request");
    for (int toggle = 0; toggle < 81; ++toggle)
        advanceHerd(group, house, toggle % 2 == 0, .1f);
    advanceHerd(group, house, true, 150);
    check(inBeds(group), "Rapid toggles also settle at the latest nighttime request");
    for (int cycle = 0; cycle < 3; ++cycle) {
        advanceHerd(group, house, false, 70);
        advanceHerd(group, house, true, 60);
    }
    check(inBeds(group), "Repeated complete day/night cycles preserve stall assignments");
    check(smoothTravel, "Herd transitions never teleport or exceed walking speed");
    check(safeDoor && singleFile,
          "Only one cow crosses the doorway at a time, with the door fully open");
    check(correctGate, "Barn journeys cross the pasture fence only through its gate");
    Herd largeStep;
    Barn largeStepBarn;
    largeStep.update(60, true, largeStepBarn);
    check(inBeds(largeStep), "Large simulation advances still complete the night routine safely");
    Herd slowFrames, fastFrames;
    Barn slowBarn, fastBarn;
    for (int i = 0; i < 200; ++i)
        slowFrames.update(1.0f / 20, true, slowBarn);
    for (int i = 0; i < 600; ++i)
        fastFrames.update(1.0f / 60, true, fastBarn);
    bool sameTravel = slowFrames.phase() == fastFrames.phase();
    for (size_t i = 0; i < 3; ++i)
        sameTravel &=
            length(slowFrames.animals()[i].position() - fastFrames.animals()[i].position()) < .02f;
    check(sameTravel, "Herd travel agrees at 20 and 60 updates per second");
    scene.key('0');
    check(scene.wind.getStrength() == 0, "Zero key disables wind");
    scene.key('3');
    check(scene.wind.getStrength() == 3, "Three key restores strong wind");
    scene.key('p');
    float time = scene.wind.getTime();
    scene.update(1);
    check(approximately(scene.wind.getTime(), time), "Pause freezes scene animation");
    auto pausedCow = scene.herd.animals()[0].position();
    float pausedDoor = scene.barn.doorOpening();
    scene.key('n');
    scene.update(1);
    check(length(scene.herd.animals()[0].position() - pausedCow) == 0 &&
              scene.barn.doorOpening() == pausedDoor,
          "Pause freezes cows and doors while retaining the requested night mode");
    scene.key('p');
    scene.update(.5f);
    check(scene.herd.phase() == Herd::Phase::Gathering,
          "Resuming applies the night request made while paused");
    scene.key('n');
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
    check(Font::available() && Font::width("Stall 1", 30) > Font::width("Stall 1", 20),
          "Bundled handwriting loads and its text width scales with font size");
    check(glGetError() == GL_NO_ERROR, "Initialization and texture tests leave no OpenGL errors");
    std::cout << checks << " checks, " << failures << " failures\n";
    return failures == 0;
}

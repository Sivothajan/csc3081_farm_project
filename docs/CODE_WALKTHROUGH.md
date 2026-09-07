# Willowfield Code Walkthrough

This document explains the project for a beginner who wants to understand the
code, tune parameters, or add small modifications quickly.

The project is an interactive 3D farm scene built with C++20, OpenGL, and
FreeGLUT. Most drawing is done with immediate-mode OpenGL calls such as
`glBegin`, `glVertex3f`, `glColor3f`, `glPushMatrix`, and `glPopMatrix`.

## How to Use This Guide

Use this document in three passes:

1. Read **Quick Mental Model** and **Program Entry** to understand how the app
   runs.
2. Use **Start Here When Modifying** when you only need to tune a parameter.
3. Use **Common Modification Recipes** when you want to add or change behavior.

For the fastest learning path, read the files in the order listed at the end of
this document.

## Table of Contents

- [Quick Mental Model](#quick-mental-model)
- [Start Here When Modifying](#start-here-when-modifying)
- [Coordinate System](#coordinate-system)
- [Program Entry: `src/main.cpp`](#program-entry-srcmaincpp)
- [Scene Owner: `src/core/Scene.*`](#scene-owner-srccorescene)
- [Camera: `src/core/Camera.*`](#camera-srccorecamera)
- [Constants: `src/utils/Constants.h`](#constants-srcutilsconstantsh)
- [Wind: `src/systems/WindSystem.*`](#wind-srcsystemswindsystem)
- [Grass: `src/vegetation/Grass.*`](#grass-srcvegetationgrass)
- [Grass Cutting System](#grass-cutting-system-srcsystemsgrasscuttingsystem)
- [Crops: `src/vegetation/Crop.*`](#crops-srcvegetationcrop)
- [Trees: `src/vegetation/Tree.*`](#trees-srcvegetationtree)
- [Cows: `src/animals/Cow.*`](#cows-srcanimalscow)
- [Herd State Machine](#herd-state-machine-srcsystemsherd)
- [Barn: `src/structures/Barn.*`](#barn-srcstructuresbarn)
- [Windmill: `src/structures/Windmill.*`](#windmill-srcstructureswindmill)
- [Fence: `src/structures/Fence.*`](#fence-srcstructuresfence)
- [Terrain: `src/environment/Terrain.*`](#terrain-srcenvironmentterrain)
- [Paths and Signs](#paths-and-signs-srcenvironmentpath)
- [Sky: `src/environment/Sky.*`](#sky-srcenvironmentsky)
- [HUD: `src/core/Hud.*`](#hud-srccorehud)
- [Text System](#text-system-srccoretext-and-assetstexttxt)
- [Drawing Helpers](#drawing-helpers-srcutilshelpers)
- [Math Helpers](#math-helpers-srcutilsmathutilsh)
- [Common Modification Recipes](#common-modification-recipes)
- [Build and Verify](#build-and-verify)
- [Troubleshooting](#troubleshooting)
- [Best Files to Read in Order](#best-files-to-read-in-order)

## Quick Mental Model

The program runs in this loop:

1. `src/main.cpp` creates the OpenGL window.
2. FreeGLUT calls input callbacks when the user presses keys or moves the mouse.
3. FreeGLUT calls `update()` around every 16 ms.
4. `update()` moves the camera, wind, cows, sky, windmill, and grass cutter.
5. FreeGLUT calls `display()`.
6. `display()` asks `Scene` to draw the whole farm.

The most important files are:

| File                                    | Purpose                                                                    |
| --------------------------------------- | -------------------------------------------------------------------------- |
| `src/main.cpp`                          | Application entry point, window setup, keyboard/mouse callbacks, main loop |
| `src/core/Scene.*`                      | Owns all farm objects and decides update/render order                      |
| `src/core/Camera.*`                     | Camera movement, fixed camera views, mouse/arrow look                      |
| `src/utils/Constants.h`                 | Small global tuning constants                                              |
| `src/systems/WindSystem.*`              | Wind strength, wave timing, plant bending math                             |
| `src/systems/GrassCuttingSystem.*`      | Grass cutting toggle and cutting radius marker                             |
| `src/vegetation/Grass.*`                | Meadow and pasture grass generation, rendering, cutting                    |
| `src/vegetation/Crop.*`                 | Wheat/crop row generation and rendering                                    |
| `src/vegetation/Tree.*`                 | Tree placement, branches, leaves, wind response                            |
| `src/systems/Herd.*`                    | Cow night/morning behavior state machine                                   |
| `src/animals/Cow.*`                     | Cow movement, walking animation, sleeping pose, drawing                    |
| `src/structures/Barn.*`                 | Barn geometry, stalls, doors, night light                                  |
| `src/structures/Windmill.*`             | Windmill tower and rotating blades                                         |
| `src/structures/Fence.*`                | Fence segment helper and fence layout                                      |
| `src/environment/Terrain.*`             | Ground, meadow, pasture, dirt field, furrows                               |
| `src/environment/Path.*`                | Paths and signboards                                                       |
| `src/environment/Sky.*`                 | Day/night color and moving clouds/sun                                      |
| `src/core/Hud.*`                        | 2D overlay text, wind meter, controls, study diagram                       |
| `src/core/Text.*` and `assets/text.txt` | User-facing text loaded from a plain text file                             |
| `src/utils/Helpers.*`                   | Shared drawing helpers: boxes, ellipsoids, beams, signs, text              |
| `src/utils/MathUtils.h`                 | Small vector and math helpers                                              |

## Start Here When Modifying

For simple tuning, start with these files:

| Goal                         | File                                           | What to change                                |
| ---------------------------- | ---------------------------------------------- | --------------------------------------------- |
| Make camera faster/slower    | `src/utils/Constants.h`                        | `CAMERA_SPEED`                                |
| Let camera go lower/higher   | `src/utils/Constants.h`, `src/core/Camera.cpp` | `EYE_MIN` and camera height clamps            |
| Make max wind stronger       | `src/utils/Constants.h`                        | `WIND_MAX`                                    |
| Change default wind          | `src/systems/WindSystem.h`                     | `strength = 1.2f`                             |
| Change wind animation speed  | `src/systems/WindSystem.h`                     | `speed = 2.2f`                                |
| Change cutting radius        | `src/utils/Constants.h`                        | `CUT_RADIUS`                                  |
| Add more/fewer grass blades  | `src/vegetation/Grass.cpp`                     | `for (int i = 0; i < 2800; ++i)`              |
| Move meadow grass area       | `src/vegetation/Grass.cpp`                     | meadow `x` and `z` formulas                   |
| Move crop rows               | `src/vegetation/Crop.cpp`                      | crop `x` and `z` loop ranges                  |
| Move trees                   | `src/vegetation/Tree.cpp`                      | calls to `one({x, y, z}, scale, phase, wind)` |
| Rename signs/stalls/HUD text | `assets/text.txt`                              | values after `key =`                          |
| Add a new keyboard shortcut  | `src/core/Scene.cpp`                           | `Scene::key()` or `Scene::specialKey()`       |
| Add a new object to the farm | `src/core/Scene.h`, `src/core/Scene.cpp`       | add member, update, render                    |

## Coordinate System

The farm uses a simple 3D coordinate system:

| Axis | Meaning                      |
| ---- | ---------------------------- |
| `x`  | left/right across the farm   |
| `y`  | height above ground          |
| `z`  | forward/back across the farm |

Most ground objects are placed with `y = 0`. A position like `{17, 0, -13}`
means:

- `x = 17`: right side of the farm
- `y = 0`: on the ground
- `z = -13`: toward the back side

OpenGL colors use floats from `0.0f` to `1.0f`. For example:

```cpp
const Vec3 red{.66f, .23f, .17f};
```

That means red is strongest, green is weaker, and blue is weakest.

## Program Entry: `src/main.cpp`

`main.cpp` is the bridge between FreeGLUT and the project code.

Important global variables:

| Variable          | Meaning                                                |
| ----------------- | ------------------------------------------------------ |
| `Scene scene`     | The whole farm scene                                   |
| `keys`            | Tracks normal keyboard keys such as `w`, `a`, `s`, `d` |
| `special`         | Tracks special keys such as arrows and function keys   |
| `width`, `height` | Current window size                                    |
| `previousTime`    | Last update timestamp                                  |
| `dragging`        | Whether right mouse drag is active                     |
| `capturePath`     | Used by automated screenshot tests                     |

Important functions:

| Function        | What it does                                                             |
| --------------- | ------------------------------------------------------------------------ |
| `main()`        | Parses run options, creates the window, initializes OpenGL and the scene |
| `display()`     | Clears the screen, applies camera, renders scene, swaps buffers          |
| `update(int)`   | Calculates `dt`, updates camera and scene, schedules next update         |
| `keyboard()`    | Handles key press start                                                  |
| `keyboardUp()`  | Handles key release                                                      |
| `specialDown()` | Handles arrow/F-key press start                                          |
| `specialUp()`   | Handles arrow/F-key release                                              |
| `mouse()`       | Starts/stops right mouse dragging                                        |
| `motion()`      | Rotates camera while dragging                                            |
| `reshape()`     | Stores new window size                                                   |

The line:

```cpp
glutTimerFunc(16, update, 0);
```

asks FreeGLUT to call `update()` again after about 16 ms. That is roughly 60
updates per second.

The line:

```cpp
float dt = std::clamp(float(now - previousTime) / 1000.0f, 0.0f, 0.05f);
```

converts elapsed milliseconds to seconds and limits one update step to at most
`0.05` seconds. This prevents a large jump if the app pauses briefly.

## Scene Owner: `src/core/Scene.*`

`Scene` owns the main farm components:

```cpp
Terrain terrain;
Path path;
Fence fence;
Windmill windmill;
Barn barn;
Rock rocks;
HayBale hay;
Lighting lighting;
WindSystem wind;
Grass grass;
Crop crops;
Tree trees;
Herd herd;
AnimationSystem animation;
GrassCuttingSystem cutting;
Hud hud;
Sky sky;
```

### `Scene::initialize()`

This loads the resources that need setup:

- `Text::initialize(executable)` loads `assets/text.txt`.
- `grass.initialize()` creates random grass blades.
- `crops.initialize()` creates crop stalks.
- `textures.initialize(executable)` loads bitmap textures.
- `Font::initialize(executable)` loads the bitmap font.

### `Scene::update(float dt)`

This runs simulation logic:

```cpp
textNoticeTime = std::max(0.0f, textNoticeTime - dt);
```

Counts down the temporary message shown after reloading text.

```cpp
const float step = animation.update(dt);
```

Returns either real `dt` or `0` if paused.

```cpp
wind.update(step);
windmill.update(step, wind.getStrength());
herd.update(step, sky.night, barn);
sky.update(step);
```

Moves wind time, windmill rotation, cow behavior, and day/night transition.

```cpp
if (!animation.paused)
    cutting.update(grass, camera.position);
```

Cuts nearby grass only while animation is not paused.

### `Scene::key(unsigned char key)`

This handles normal keyboard keys:

| Key                | Code effect                     |
| ------------------ | ------------------------------- |
| `v`                | overview camera                 |
| `g`                | meadow camera                   |
| `h`                | barn camera                     |
| `Tab`              | cycle camera                    |
| `+`, `=`           | increase wind                   |
| `-`                | decrease wind                   |
| `0`, `1`, `2`, `3` | set exact wind strength         |
| `c`                | toggle cutting                  |
| `r`                | reset grass                     |
| `p`                | pause animation                 |
| `b`                | toggle structural study diagram |
| `l`                | toggle lighting                 |
| `f`                | toggle wireframe                |
| `t`                | toggle textures                 |
| `n`                | toggle day/night                |

To add a new normal key, add another `if (key == 'x')` block here.

### `Scene::specialKey(int key)`

This handles function keys:

- `F9` reloads `assets/text.txt`.
- `F10` hides/shows HUD.
- `F1` to `F8` switch camera views.

### `Scene::render(int width, int height)`

This draws the farm in a deliberate order:

1. Apply lighting.
2. Apply barn night light.
3. Set wireframe or filled drawing.
4. Draw sky.
5. Draw terrain.
6. Draw paths.
7. Draw fence.
8. Draw windmill.
9. Draw barn.
10. Draw rocks.
11. Draw hay.
12. Draw grass.
13. Draw crops.
14. Draw trees.
15. Draw cows.
16. Draw cutting circle.
17. Draw HUD overlay.

If you add a new object, place it where it makes visual sense. For example, a
ground object should usually render after terrain/path and before vegetation.

## Camera: `src/core/Camera.*`

The camera stores:

```cpp
Vec3 position{33, 27, 37};
float yaw = -132.0f, pitch = -29.5f;
```

`position` is the camera location. `yaw` turns left/right. `pitch` looks
up/down.

### Movement

`Camera::update()` reads the `keys` and `special` arrays.

Arrow keys call:

```cpp
look(horizontalSpeed, verticalSpeed);
```

`W/S` move forward/back. `A/D` move left/right. `Q/E` lower/raise the camera.

This line builds a movement vector:

```cpp
Vec3 move = forward * (float(keys['w']) - float(keys['s'])) +
            right * (float(keys['d']) - float(keys['a']));
```

If `w` is pressed, the value is positive. If `s` is pressed, the value is
negative. If both are pressed, they cancel out.

The camera position is clamped:

```cpp
position.x = std::clamp(position.x, -34.0f, 34.0f);
position.z = std::clamp(position.z, -34.0f, 38.0f);
position.y = std::clamp(position.y, Constants::EYE_MIN, 30.0f);
```

Change these numbers if you want the user to explore a larger/smaller area.

### Fixed Views

`Camera::setView()` contains preset camera positions. Example:

```cpp
case View::Front:
    position = {0, 23, 38};
    target = {0, 0, 0};
    break;
```

This puts the camera in front of the farm and points it at the center.

To add another preset camera:

1. Add a new value to `enum class View` in `Camera.h`.
2. Add a `case` in `Camera::setView()`.
3. Add a text key to `Camera::viewName()`.
4. Add that text key to `assets/text.txt`.
5. Add a shortcut in `Scene::specialKey()` or `Scene::key()`.

## Constants: `src/utils/Constants.h`

This file is intentionally small and safe to edit.

```cpp
constexpr float CAMERA_SPEED = 6.0f;
```

Higher value means faster movement.

```cpp
constexpr float EYE_MIN = 1.35f;
```

The lowest allowed camera height.

```cpp
constexpr float WIND_MAX = 3.0f;
```

Maximum wind strength accepted by `WindSystem::setStrength()`.

```cpp
constexpr float CUT_RADIUS = 1.8f;
```

Radius around the camera where grass gets cut.

```cpp
constexpr int GRASS_SEGMENTS = 5;
```

How many sections each grass blade has. More segments can make bending smoother
but also increases drawing work.

## Wind: `src/systems/WindSystem.*`

`WindSystem` stores:

```cpp
float strength = 1.2f, time = 0, speed = 2.2f;
```

| Variable   | Meaning                  |
| ---------- | ------------------------ |
| `strength` | Current wind power       |
| `time`     | Wind animation time      |
| `speed`    | How fast wind waves move |

`setStrength()` clamps the value:

```cpp
strength = std::clamp(value, 0.0f, Constants::WIND_MAX);
```

So even if a key tries to set too much wind, the value stays inside a safe
range.

`direction()` returns:

```cpp
{0.8944272f, 0, 0.4472136f}
```

This is the prevailing wind direction. Change this vector if you want wind to
push plants in a different direction. Keep `y = 0` unless you intentionally want
vertical bending.

`getDisplacement()` combines two wave patterns:

- `gust`: slow variation across the farm
- `wave`: faster plant movement

The final formula includes:

```cpp
u * u
```

`u` is height from root to tip. At the root `u = 0`, so displacement is zero.
At the tip `u = 1`, displacement is strongest. This keeps plant roots fixed.

`bend()` takes a base point and returns a deformed point:

```cpp
return base + Vec3{0, height * u, 0} + direction() * (...);
```

That means the point first goes upward from the root, then shifts sideways with
the wind.

## Grass: `src/vegetation/Grass.*`

`GrassBlade` stores one blade:

| Field            | Meaning                                |
| ---------------- | -------------------------------------- |
| `base`           | Root position                          |
| `originalHeight` | Height before cutting                  |
| `currentHeight`  | Current height after cutting           |
| `width`          | Blade width                            |
| `orientation`    | Direction of the blade face            |
| `phase`          | Random animation offset                |
| `response`       | How strongly this blade reacts to wind |
| `cut`            | Whether it was cut                     |
| `cuttable`       | Whether cutting is allowed             |

### Grass Generation

`Grass::initialize()` uses a fixed random seed:

```cpp
std::mt19937 rng(3081);
```

Because the seed is fixed, the grass layout is repeatable every run.

This loop creates all blades:

```cpp
for (int i = 0; i < 2800; ++i)
```

Change `2800` for more or less grass. More grass looks richer but can reduce
performance.

The first 2300 blades are meadow grass:

```cpp
const bool meadow = i < 2300;
```

Meadow grass is taller and cuttable. Later blades are shorter pasture grass.

The meadow area is controlled here:

```cpp
float x = -21.5f + 18 * random(rng);
float z = 3.3f + 17 * random(rng);
```

The pasture area is controlled here:

```cpp
float x = 5 + 15 * random(rng);
float z = 6 + 11 * random(rng);
```

### Grass Rendering

`Grass::render()` draws each blade as triangles. Each blade is split into
`Constants::GRASS_SEGMENTS` pieces. Every segment calls `wind.bend()` twice:

- once for the lower point of the segment
- once for the upper point of the segment

The color changes as `u` increases, so blade tips are slightly lighter.

### Cutting

`Grass::cutNear()` checks distance from the camera:

```cpp
distanceSquared2D(p, blade.base) < radius * radius
```

It ignores height and uses only `x` and `z`. If the blade is inside the radius,
it becomes shorter:

```cpp
blade.currentHeight = blade.originalHeight * .22f;
```

Change `.22f` if you want cut grass to be taller or shorter.

## Grass Cutting System: `src/systems/GrassCuttingSystem.*`

`GrassCuttingSystem` is deliberately small.

```cpp
bool enabled = false;
```

Cutting starts off disabled.

```cpp
if (enabled && camera.y < 2.6f)
    grass.cutNear(camera, Constants::CUT_RADIUS);
```

The camera must be near the ground. This prevents flying over the meadow and
cutting everything below.

`render()` draws a yellow circle around the cutting area. It uses 64 points:

```cpp
for (int i = 0; i < 64; ++i)
```

Use more points for a smoother circle or fewer for a rougher/debug style circle.

## Crops: `src/vegetation/Crop.*`

`Crop::initialize()` places crop stalks in rows.

This controls row spacing:

```cpp
for (float x = -7.3f; x < 9; x += 1.15f)
```

This leaves the central path clear:

```cpp
if (std::abs(x) < 2.2f)
    continue;
```

This controls stalk spacing inside each row:

```cpp
for (float z = -20.2f; z < -4.4f; z += .42f)
```

The stalk height is:

```cpp
1.3f + random(rng) * .5f
```

So each stalk is between `1.3` and `1.8` units tall.

`Crop::render()` draws:

- two crossed thin stalk surfaces
- leaves
- seed heads near the top

All of them follow the same wind-deformed centerline.

## Trees: `src/vegetation/Tree.*`

`Tree::render()` places each tree:

```cpp
one({-19, 0, -18}, 1.1f, .2f, wind);
```

The parameters are:

1. position
2. scale
3. phase
4. wind system

To add a tree, add another `one(...)` call.

`Tree::one()` draws:

1. a trunk
2. five branches
3. ellipsoid leaf clusters
4. individual hanging leaves
5. a top leaf cluster

The branch loop:

```cpp
for (int branch = 0; branch < 5; ++branch)
```

Change `5` for more/fewer branches.

The leaf loop:

```cpp
for (int leaf = 0; leaf < 9; ++leaf)
```

Change `9` for denser/sparser hanging leaves.

## Cows: `src/animals/Cow.*`

Each `Cow` has:

| Field         | Meaning                                               |
| ------------- | ----------------------------------------------------- |
| `center`      | Center of the grazing route                           |
| `location`    | Current position                                      |
| `offset`      | Time/route offset so cows do not all move identically |
| `heading`     | Rotation angle                                        |
| `legSwing`    | Walking leg animation                                 |
| `headDip`     | Grazing/sleeping head angle                           |
| `time`        | Cow-local animation time                              |
| `sleepAmount` | 0 awake, 1 sleeping                                   |

### Grazing Movement

`Cow::update()` is used while cows are grazing.

This line creates a repeating 24-second cycle:

```cpp
float t = time + offset, phase = std::fmod(t, 24.0f);
```

The comment describes the cycle:

- 10 seconds walking
- 11 seconds grazing
- 3 seconds standing

The circular route radius is:

```cpp
2.1f
```

Change that value if you want wider/narrower grazing circles.

### Walking to a Target

`Cow::walkTo()` moves the cow toward a target at speed `2.4f`:

```cpp
float distance = length(delta), step = std::min(distance, 2.4f * dt);
```

Change `2.4f` to make cows walk faster/slower.

### Facing Direction

`Cow::follow()` rotates the cow gradually:

```cpp
heading += std::clamp(..., -240 * dt, 240 * dt);
```

`240` is the turn speed in degrees per second.

### Sleeping

`Cow::rest()` moves `sleepAmount` toward 1 when asleep and toward 0 when awake.
`Cow::render()` uses `sleepAmount` to lower the body, fold legs, dip the head,
and flatten the eyes.

## Herd State Machine: `src/systems/Herd.*`

`Herd` controls three cows together.

The possible phases are:

| Phase       | Meaning                                |
| ----------- | -------------------------------------- |
| `Grazing`   | Cows wander in pasture                 |
| `Gathering` | Cows walk into a queue                 |
| `Entering`  | Cows enter the barn one by one         |
| `Sleeping`  | Cows rest in stalls                    |
| `Leaving`   | Cows leave the barn one by one         |
| `Returning` | Cows return to saved pasture positions |

The cows are created here:

```cpp
std::array<Cow, 3> cows{{Cow({9, 0, 10}, 0), Cow({16, 0, 12}, 12), Cow({9, 0, 18}, 20)}};
```

To add more cows, this is not the only line to change. Many arrays are fixed to
size 3, and the barn has 3 stalls. For a beginner, it is safer to move or edit
the existing three cows first.

The main route into the barn is:

```cpp
const std::array<Vec3, 5> lane{{{7, 0, 15}, {0, 0, 15}, {0, 0, 0}, {-17, 0, 0}, Barn::aisle(1)}};
```

To adjust the cow path, edit these waypoints carefully. The path should still
lead through the barn door and aisle.

The herd speed is:

```cpp
constexpr float speed = 2.4f;
```

The spacing between queued cows is:

```cpp
constexpr float gap = 5;
```

`Herd::update()` splits large `dt` values into 1/60 second slices:

```cpp
float slice = std::min(dt, 1.0f / 60);
```

This keeps the door/cow behavior stable even during automated tests or frame
rate hiccups.

## Barn: `src/structures/Barn.*`

The barn stores one animation value:

```cpp
float opening = 0;
```

`0` means closed. `1` means fully open.

`Barn::update()` opens or closes the doors:

```cpp
opening = std::clamp(opening + (open ? dt : -dt) * .65f, 0.0f, 1.0f);
```

Change `.65f` to control door speed.

`Barn::passable()` returns true only when the door is almost fully open:

```cpp
return opening >= .99f;
```

The herd uses this so cows do not walk through closed doors.

Important barn helper positions:

```cpp
static Vec3 aisle(int stall)
static Vec3 bed(int stall)
static Vec3 viewPosition()
```

These are used by the herd and camera. If you move the barn interior layout,
update these helpers too.

`Barn::render()` is long because it draws the whole barn directly:

- floor
- walls
- doorway
- roof
- door leaves
- windows
- stall rails
- straw
- hay bales
- stall plaques
- night light

When editing this file, make small changes and rebuild often.

## Windmill: `src/structures/Windmill.*`

`Windmill::update()` rotates the blade angle:

```cpp
angle = std::fmod(angle + strength * 65 * dt, 360.0f);
```

`65` is the rotation speed multiplier. Higher means faster spinning for the
same wind strength.

The windmill position is in `render()`:

```cpp
glTranslatef(17, 0, -13);
```

Change those numbers to move the whole windmill.

The blade loop:

```cpp
for (int i = 0; i < 8; ++i)
```

The current windmill has 8 blades. Change this count and the rotation step if
you want a different design:

```cpp
glRotatef(float(i) * 45, 0, 0, 1);
```

For 6 blades, use `i < 6` and `float(i) * 60`.

## Fence: `src/structures/Fence.*`

`Fence::segment(Vec3 a, Vec3 b)` draws a fence line between two points.

The number of posts is based on distance:

```cpp
int(std::ceil(length(b - a) / 2.5f))
```

Change `2.5f` for tighter or wider post spacing.

`Fence::render()` lists all fence segments. To add a new fence:

```cpp
segment({x0, 0, z0}, {x1, 0, z1});
```

## Terrain: `src/environment/Terrain.*`

`Terrain::render()` draws the base farm ground.

This draws the thick soil base:

```cpp
Draw::box({0, -.7f, 0}, {50, 1.35f, 50}, {.38f, .30f, .20f});
```

This draws the main grass ground:

```cpp
Draw::ground(-25, -25, 25, 25, 0, {.49f, .61f, .29f}, 18);
```

`Draw::ground(x0, z0, x1, z1, y, color, repeat)` draws a flat rectangle.

The meadow patch:

```cpp
Draw::ground(-22, 3, -3, 21, .016f, {...}, 8);
```

The pasture patch:

```cpp
Draw::ground(3, 2, 22, 21, .016f, {...}, 8);
```

The dirt crop field:

```cpp
Draw::ground(-8, -21, 9, -4, .023f, {...}, 7);
```

The furrow loop matches the crop row spacing:

```cpp
for (float x = -7.5f; x < 9; x += 1.15f)
```

If you change crop row spacing in `Crop.cpp`, consider changing this too.

## Paths and Signs: `src/environment/Path.*`

`Path::render()` draws three dirt paths and three signs.

Example sign:

```cpp
Draw::sign({-3, 0, 21}, Text::get("sign.meadow"), Text::get("sign.meadow_hint"));
```

The text comes from `assets/text.txt`. That means you can rename signs without
rebuilding the C++ code.

To add a sign:

1. Add text keys in `assets/text.txt`.
2. Add a `Draw::sign(...)` call in `Path::render()`.

## Sky: `src/environment/Sky.*`

`Sky` stores:

```cpp
bool night = false;
float nightAmount = 0;
```

`night` is the target mode. `nightAmount` smoothly moves between day and night.

This controls transition speed:

```cpp
float change = dt * .4f;
```

Higher value means faster day/night transition.

`background()` sets the clear color:

```cpp
glClearColor(.66f - nightAmount * .61f, ...);
```

When `nightAmount` is 0, the sky is bright. When it is 1, the sky is dark.

`render()` draws moving cloud ellipsoids and the sun/moon-like sphere.

This controls cloud drift:

```cpp
float x = std::fmod(float(i) * 19 + time * .25f, 100.0f) - 50;
```

Change `.25f` for faster/slower clouds.

## HUD: `src/core/Hud.*`

`HudState` is the data passed into the HUD:

```cpp
struct HudState {
    bool cutting, paused, diagram;
    int cutCount;
    float height;
    std::string herdStatus;
    int sleepingCows;
    bool night;
    std::string viewName;
    std::string textNotice;
};
```

`Scene::render()` fills this struct from the current scene state.

`Hud::render()` switches from 3D rendering to 2D overlay rendering:

```cpp
gluOrtho2D(0, right, 0, float(windowHeight) / scale);
```

It draws:

- top-left farm/status panel
- bottom controls panel
- top-right wind panel
- right-side structural study diagram

The helper `fitted()` shrinks text if it is too wide:

```cpp
std::min(size, size * width / std::max(Font::width(value, size), 1.0f))
```

That keeps text inside its panel.

## Text System: `src/core/Text.*` and `assets/text.txt`

The app loads display text from `assets/text.txt`.

The format is:

```text
key = value
```

Blank lines and lines beginning with `#` are ignored.

`Text::initialize()` first tries:

```cpp
assets/text.txt
```

from the current project folder. If that does not exist, it tries the `assets`
folder beside the executable.

`Text::reload()` loads the file again. Press `F9` in the app to reload text
without rebuilding.

The loader rejects unsafe/malformed lines:

- no `=`
- empty key
- empty value
- line longer than 1024 characters
- non-ASCII display characters
- control characters

`Text::format()` replaces placeholders. For example, if the text contains:

```text
Wind {value}/{maximum}
```

and the code passes `value = 1.2`, `maximum = 3`, the result becomes:

```text
Wind 1.2/3
```

Keep placeholder names unchanged unless you also update the C++ code that
supplies them.

## Drawing Helpers: `src/utils/Helpers.*`

The `Draw` namespace contains reusable shape helpers.

| Helper              | Purpose                                         |
| ------------------- | ----------------------------------------------- |
| `Draw::box()`       | Draws a colored textured cuboid                 |
| `Draw::ellipsoid()` | Draws a stretched sphere                        |
| `Draw::beam()`      | Draws a cylinder between two points             |
| `Draw::ground()`    | Draws a flat rectangle with texture coordinates |
| `Draw::triangle()`  | Draws one triangle with calculated normal       |
| `Draw::text()`      | Draws text using the bitmap font                |
| `Draw::plaque()`    | Draws a camera-facing signboard/plaque          |
| `Draw::sign()`      | Draws a post plus plaque                        |

When adding new objects, use these helpers before writing raw OpenGL. They keep
the style consistent and reduce repeated code.

### Matrix Push/Pop Pattern

Many render functions use:

```cpp
glPushMatrix();
glTranslatef(...);
glRotatef(...);
glScalef(...);
// draw local object
glPopMatrix();
```

`glPushMatrix()` saves the current transform. `glPopMatrix()` restores it. This
prevents one object's movement/rotation from accidentally affecting the next
object.

### Attribute Push/Pop Pattern

Some functions use:

```cpp
glPushAttrib(...);
glDisable(GL_LIGHTING);
// draw special object
glPopAttrib();
```

This temporarily changes OpenGL render state and then restores it.

## Math Helpers: `src/utils/MathUtils.h`

`Vec3` is a small 3D vector:

```cpp
struct Vec3 {
    float x = 0, y = 0, z = 0;
};
```

It supports:

- `a + b`
- `a - b`
- `a * scale`

Other helpers:

| Function              | Purpose                               |
| --------------------- | ------------------------------------- |
| `radians()`           | Converts degrees to radians           |
| `length()`            | 3D vector length                      |
| `normalized()`        | Same direction, length 1              |
| `cross()`             | Cross product for triangle normals    |
| `distanceSquared2D()` | Distance check using only `x` and `z` |

`distanceSquared2D()` avoids `sqrt`, so it is faster for repeated radius
checks like grass cutting.

## Common Modification Recipes

### Make the Grass Denser

Open `src/vegetation/Grass.cpp`.

Change:

```cpp
for (int i = 0; i < 2800; ++i)
```

to something like:

```cpp
for (int i = 0; i < 4000; ++i)
```

Also update:

```cpp
const bool meadow = i < 2300;
```

For example, with 4000 blades:

```cpp
const bool meadow = i < 3300;
```

Then rebuild and check performance.

### Make Wind Stronger

Open `src/utils/Constants.h`.

Change:

```cpp
constexpr float WIND_MAX = 3.0f;
```

to:

```cpp
constexpr float WIND_MAX = 5.0f;
```

Then open `src/systems/WindSystem.h` and optionally change default strength:

```cpp
float strength = 1.2f;
```

to:

```cpp
float strength = 2.0f;
```

### Make Cows Walk Faster

There are two main speed values.

For individual cow walking, open `src/animals/Cow.cpp` and change:

```cpp
2.4f * dt
```

inside `Cow::walkTo()`.

For herd route movement, open `src/systems/Herd.cpp` and change:

```cpp
constexpr float speed = 2.4f;
```

Keep both values similar unless you intentionally want different behavior.

### Move the Barn

The barn is currently translated in `Barn::render()`:

```cpp
glTranslatef(-17, 0, -8.3f);
```

Moving the barn visually is only part of the job. You must also update:

- `Barn::aisle()`
- `Barn::bed()`
- `Barn::viewPosition()`
- herd route points in `src/systems/Herd.cpp`
- camera barn views in `src/core/Camera.cpp`

For beginners, moving smaller objects like trees, rocks, signs, or the windmill
is much safer than moving the barn.

### Add a New Sign

Open `assets/text.txt` and add:

```text
sign.new_area = New Area
sign.new_area_hint = Walk this way
```

Open `src/environment/Path.cpp` and add:

```cpp
Draw::sign({x, 0, z}, Text::get("sign.new_area"), Text::get("sign.new_area_hint"));
```

Replace `x` and `z` with the desired position.

### Add a New Decorative Object

1. Create `src/objects/NewObject.h`.
2. Create `src/objects/NewObject.cpp`.
3. Add the `.cpp` file to `CMakeLists.txt` and the Visual Studio project if
   needed.
4. Include it in `src/core/Scene.h`.
5. Add it as a private member in `Scene`.
6. Call `newObject.render()` inside `Scene::render()`.

A minimal object looks like:

```cpp
#pragma once
class NewObject {
  public:
    void render() const;
};
```

```cpp
#include "NewObject.h"
#include "../utils/Helpers.h"

void NewObject::render() const {
    Draw::box({0, .5f, 0}, {1, 1, 1}, {.7f, .2f, .2f});
}
```

### Add a New Key Toggle

Suppose you add:

```cpp
bool showNewObject = true;
```

to `Scene.h`.

In `Scene::key()`:

```cpp
if (key == 'o')
    showNewObject = !showNewObject;
```

In `Scene::render()`:

```cpp
if (showNewObject)
    newObject.render();
```

## Build and Verify

Build:

```powershell
.\scripts\build.ps1 -Configuration Release
```

Run:

```powershell
.\build\x64\Release\csc3081_farm_project.exe
```

Run project verification:

```powershell
.\scripts\verify.ps1
```

Format code:

```powershell
.\scripts\format.ps1
```

Check formatting only:

```powershell
.\scripts\format.ps1 -Check
```

## Beginner Safety Checklist

Before changing code:

1. Decide the exact behavior you want.
2. Find the smallest file that controls it.
3. Change one thing at a time.
4. Build after each small change.
5. Run the app and test the related control/view.
6. If the change affects visuals, check both overview and close camera views.
7. If the change affects cows, test both day and night using `N`.
8. If the change affects text, press `F9` before rebuilding.

Good first modifications:

- rename signs and stall names
- change wind strength/speed
- change cutting radius
- add/move trees
- add/move signs
- change grass density
- change cow walking speed
- change camera presets

More advanced modifications:

- add more cows
- move the barn
- add a new animated object
- add collision
- add new texture types
- replace immediate-mode rendering with modern OpenGL

## Troubleshooting

### Text shows `?`

The text key is missing from `assets/text.txt` or from the built-in defaults.
Check the exact spelling of the key used in code.

### Text reload fails

Check `assets/text.txt` for:

- missing `=`
- empty value
- non-ASCII characters
- changed placeholder names

### Grass does not cut

Check:

- Press `C` to enable cutting.
- Camera height must be below `2.6f`.
- The blade must have `cuttable = true`.
- The blade must be inside `Constants::CUT_RADIUS`.

### Wind does nothing

Check:

- Wind strength is not `0`.
- `Scene::update()` is running.
- Animation is not paused.
- Vegetation render functions are receiving `wind`.

### Cows do not enter the barn

Check:

- Press `N` to switch to night.
- Wait for cows to gather first.
- Barn doors must reach `Barn::passable()`.
- Herd route points must still lead to the barn aisle.

### Screen looks flat or strange

Check:

- Press `L` to toggle lighting.
- Press `T` to toggle textures.
- Press `F` to toggle wireframe.
- Verify texture files exist in `assets/textures`.

## Best Files to Read in Order

If you want to learn the full project gradually, read in this order:

1. `src/main.cpp`
2. `src/core/Scene.h`
3. `src/core/Scene.cpp`
4. `src/utils/Constants.h`
5. `src/utils/MathUtils.h`
6. `src/utils/Helpers.h`
7. `src/utils/Helpers.cpp`
8. `src/core/Camera.cpp`
9. `src/vegetation/Grass.cpp`
10. `src/systems/WindSystem.cpp`
11. `src/systems/GrassCuttingSystem.cpp`
12. `src/environment/Terrain.cpp`
13. `src/environment/Path.cpp`
14. `src/structures/Fence.cpp`
15. `src/structures/Windmill.cpp`
16. `src/animals/Cow.cpp`
17. `src/systems/Herd.cpp`
18. `src/structures/Barn.cpp`
19. `src/core/Hud.cpp`
20. `src/core/Text.cpp`

That order starts with the main program flow, then small helpers, then visual
objects, then the more complex cow/barn/HUD systems.

#pragma once
#include "Camera.h"
#include "../environment/Terrain.h"
#include "../environment/Path.h"
#include "Lighting.h"
#include "../structures/Fence.h"
#include "../structures/Windmill.h"
#include "../structures/Barn.h"
#include "../objects/Rock.h"
#include "../objects/HayBale.h"
#include "../vegetation/Grass.h"
#include "../vegetation/Crop.h"
#include "../vegetation/Tree.h"
#include "../systems/Herd.h"
#include "../systems/AnimationSystem.h"
#include "../systems/GrassCuttingSystem.h"
#include "Hud.h"
#include "Texture.h"
#include "../environment/Sky.h"
#include "../characters/Farmer.h"
class Scene {
    friend class Verification;

  public:
    Camera camera;
    void initialize(const std::filesystem::path& executable);
    void background() const { sky.background(); }
    void update(float dt);
    void render(int width, int height) const;
    void key(unsigned char key);
    void specialKey(int key);

  private:
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
    Farmer farmer;
    AnimationSystem animation;
    GrassCuttingSystem cutting;
    Hud hud;
    bool showDiagram = true, lightingEnabled = true, wireframe = false;
    bool showHud = true;
    float textNoticeTime = 0;
    std::string textNotice;
    TextureSet textures;
    bool texturesEnabled = true;
    Sky sky;
};

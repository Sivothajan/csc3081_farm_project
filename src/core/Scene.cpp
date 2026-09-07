#include "Scene.h"
#include <GL/freeglut.h>
void Scene::initialize(const std::filesystem::path& executable) {
    grass.initialize();
    crops.initialize();
    textures.initialize(executable);
    for (auto& cow : cows)
        cow.update(0);
}
void Scene::update(float dt) {
    const float step = animation.update(dt);
    wind.update(step);
    windmill.update(step, wind.getStrength());
    for (auto& cow : cows)
        cow.update(animation.time);
    sky.update(step);
    if (!animation.paused)
        cutting.update(grass, camera.position);
}
void Scene::key(unsigned char key) {
    if (key == 'v')
        camera.overview();
    if (key == 'g')
        camera.fieldView();
    if (key == '+' || key == '=')
        wind.setStrength(wind.getStrength() + .25f);
    if (key == '-')
        wind.setStrength(wind.getStrength() - .25f);
    if (key >= '0' && key <= '3')
        wind.setStrength(float(key - '0'));
    if (key == 'c')
        cutting.enabled = !cutting.enabled;
    if (key == 'r')
        grass.reset();
    if (key == 'p')
        animation.paused = !animation.paused;
    if (key == 'b')
        showDiagram = !showDiagram;
    if (key == 'l')
        lightingEnabled = !lightingEnabled;
    if (key == 'f')
        wireframe = !wireframe;
    if (key == 't')
        texturesEnabled = !texturesEnabled;
    if (key == 'n')
        sky.night = !sky.night;
}
void Scene::render(int width, int height) const {
    lighting.apply(sky.nightAmount, lightingEnabled);
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    sky.render(animation.time);
    terrain.render(textures, texturesEnabled);
    textures.bind(Surface::Dirt, texturesEnabled);
    path.render();
    textures.bind(Surface::Wood, texturesEnabled);
    fence.render();
    glDisable(GL_TEXTURE_2D);
    windmill.render();
    barn.render();
    rocks.render();
    textures.bind(Surface::Hay, texturesEnabled);
    hay.render();
    glDisable(GL_TEXTURE_2D);
    grass.render(wind);
    crops.render(wind);
    trees.render(wind);
    for (const auto& cow : cows)
        cow.render();
    cutting.render(camera.position);
    hud.render(
        width, height, wind,
        {cutting.enabled, animation.paused, showDiagram, grass.cutCount(), camera.position.y});
}

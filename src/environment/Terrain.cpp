#include "Terrain.h"
#include "../utils/Helpers.h"
void Terrain::render(const TextureSet& textures, bool enabled) const {
    Draw::box({0, -.7f, 0}, {50, 1.35f, 50}, {.38f, .30f, .20f});
    textures.bind(Surface::Ground, enabled);
    Draw::ground(-25, -25, 25, 25, 0, {.49f, .61f, .29f}, 18);
    Draw::ground(-22, 3, -3, 21, .016f, {.34f, .49f, .22f}, 8);
    Draw::ground(3, 2, 22, 21, .016f, {.52f, .63f, .32f}, 8);
    textures.bind(Surface::Dirt, enabled);
    Draw::ground(-8, -21, 9, -4, .023f, {.43f, .29f, .16f}, 7);
    // Raised furrows organize the repeated crop rows.
    for (float x = -7.5f; x < 9; x += 1.15f)
        Draw::ground(x, -20.5f, x + .35f, -4.5f, .034f, {.52f, .37f, .21f}, 5);
}

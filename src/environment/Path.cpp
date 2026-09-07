#include "Path.h"
#include "../core/Text.h"
#include "../utils/Helpers.h"
void Path::render() const {
    Draw::ground(-1.8f, -24, 1.8f, 25, .05f, {.76f, .64f, .43f}, 16);
    Draw::ground(-24, -1.8f, 24, 1.8f, .052f, {.76f, .64f, .43f}, 16);
    Draw::ground(10, -18, 14, -1.8f, .05f, {.76f, .64f, .43f}, 6);
    Draw::sign({-3, 0, 21}, Text::get("sign.meadow"), Text::get("sign.meadow_hint"));
    Draw::sign({4, 0, 19}, Text::get("sign.pasture"));
    Draw::sign({-4, 0, -3}, Text::get("sign.wheat"));
}

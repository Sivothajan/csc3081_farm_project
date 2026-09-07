#pragma once
#include <filesystem>
#include <string>

namespace Font {
void initialize(const std::filesystem::path& executable);
bool available();
float width(const std::string& value, float size);
// Coordinates use a baseline; size is in pixels for HUD text or world units for signs.
void draw(float x, float y, const std::string& value, float size);
} // namespace Font

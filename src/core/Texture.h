#pragma once
#include <GL/freeglut.h>
#include <array>
#include <filesystem>
enum class Surface { Ground, Dirt, Wood, Hay, Count };
class TextureSet {
  public:
    void initialize(const std::filesystem::path& executable);
    void bind(Surface surface, bool enabled) const;
    static GLuint loadBmp(const std::filesystem::path& path);
    int loadedCount() const;

  private:
    std::array<GLuint, 4> ids{};
};

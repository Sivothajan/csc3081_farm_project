#include "Texture.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
GLuint TextureSet::loadBmp(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file)
        return 0;
    unsigned char header[54]{};
    file.read(reinterpret_cast<char*>(header), 54);
    auto u16 = [&](int i) { return uint16_t(header[i]) | uint16_t(header[i + 1]) << 8; };
    auto u32 = [&](int i) {
        return uint32_t(header[i]) | uint32_t(header[i + 1]) << 8 | uint32_t(header[i + 2]) << 16 |
               uint32_t(header[i + 3]) << 24;
    };
    int32_t width = static_cast<int32_t>(u32(18)), signedHeight = static_cast<int32_t>(u32(22));
    if (!file || header[0] != 'B' || header[1] != 'M' || u32(14) < 40 || u16(26) != 1 ||
        u16(28) != 24 || u32(30) != 0 || width <= 0 || width > 4096 || signedHeight == 0 ||
        signedHeight < -4096 || signedHeight > 4096 || u32(10) < 54 ||
        uint64_t(u32(10)) < 14ULL + u32(14))
        return 0;
    int height = std::abs(signedHeight), stride = (width * 3 + 3) & ~3;
    file.seekg(0, std::ios::end);
    if (file.tellg() < std::streamoff(u32(10)) + std::streamoff(stride) * height)
        return 0;
    file.seekg(u32(10));
    std::vector<unsigned char> row(stride), rgb(size_t(width) * height * 3);
    for (int y = 0; y < height; ++y) {
        file.read(reinterpret_cast<char*>(row.data()), stride);
        if (!file)
            return 0;
        int target = signedHeight > 0 ? y : height - 1 - y;
        for (int x = 0; x < width; ++x)
            for (int channel = 0; channel < 3; ++channel)
                rgb[(size_t(target) * width + x) * 3 + channel] = row[x * 3 + 2 - channel];
    }
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    int result = gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE,
                                   rgb.data());
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    if (result != 0) {
        glDeleteTextures(1, &texture);
        return 0;
    }
    return texture;
}
void TextureSet::initialize(const std::filesystem::path& executable) {
    std::filesystem::path base = std::filesystem::absolute(executable).parent_path() / "assets";
    if (!std::filesystem::exists(base))
        base = "assets";
    const char* names[] = {"textures/terrain/grass_ground.bmp", "textures/terrain/dirt.bmp",
                           "textures/structures/wood.bmp", "textures/structures/hay.bmp"};
    for (size_t i = 0; i < ids.size(); ++i) {
        ids[i] = loadBmp(base / names[i]);
        if (!ids[i])
            std::cerr << "Texture unavailable; using material color: " << (base / names[i]).string()
                      << '\n';
    }
    glDisable(GL_TEXTURE_2D);
}
void TextureSet::bind(Surface surface, bool enabled) const {
    GLuint id = ids[size_t(surface)];
    if (enabled && id) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    } else
        glDisable(GL_TEXTURE_2D);
}
int TextureSet::loadedCount() const {
    return int(std::count_if(ids.begin(), ids.end(), [](GLuint id) { return id != 0; }));
}

#include "Capture.h"
#include <GL/freeglut.h>
#include <fstream>
#include <vector>
#include <filesystem>
#include <cstdint>
bool captureFrame(const std::string& path, int width, int height) {
    if (width <= 0 || height <= 0)
        return false;
    std::filesystem::path p(path);
    if (p.has_parent_path())
        std::filesystem::create_directories(p.parent_path());
    std::ofstream out(p, std::ios::binary);
    if (!out)
        return false;
    const int stride = (width * 3 + 3) & ~3;
    std::vector<unsigned char> rgb(size_t(stride) * height);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, rgb.data());
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            std::swap(rgb[size_t(y) * stride + x * 3], rgb[size_t(y) * stride + x * 3 + 2]);
    auto u16 = [&out](uint16_t n) {
        for (int i = 0; i < 2; ++i)
            out.put(char((n >> (8 * i)) & 255));
    };
    auto u32 = [&out](uint32_t n) {
        for (int i = 0; i < 4; ++i)
            out.put(char((n >> (8 * i)) & 255));
    };
    out.write("BM", 2);
    u32(54 + uint32_t(rgb.size()));
    u32(0);
    u32(54);
    u32(40);
    u32(uint32_t(width));
    u32(uint32_t(height));
    u16(1);
    u16(24);
    u32(0);
    u32(uint32_t(rgb.size()));
    u32(2835);
    u32(2835);
    u32(0);
    u32(0);
    out.write(reinterpret_cast<const char*>(rgb.data()), std::streamsize(rgb.size()));
    return bool(out);
}

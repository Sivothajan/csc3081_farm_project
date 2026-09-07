#pragma once
#include <filesystem>
#include <initializer_list>
#include <string>
#include <unordered_map>

class TextCatalog {
  public:
    TextCatalog();
    bool load(const std::filesystem::path& path);
    const std::string& get(const std::string& key) const;

  private:
    std::unordered_map<std::string, std::string> entries;
};

namespace Text {
void initialize(const std::filesystem::path& executable);
bool reload();
const std::string& get(const std::string& key);
std::string format(const std::string& key,
                   std::initializer_list<std::pair<std::string, std::string>> values);
} // namespace Text

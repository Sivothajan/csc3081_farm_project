#include "Text.h"
#include "TextDefaults.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace {
using Entries = std::unordered_map<std::string, std::string>;
std::string trim(std::string value) {
    auto first = value.find_first_not_of(" \t\r");
    return first == std::string::npos
               ? ""
               : value.substr(first, value.find_last_not_of(" \t\r") - first + 1);
}
bool read(std::istream& input, Entries& entries) {
    std::string line;
    bool first = true;
    while (std::getline(input, line)) {
        if (first && line.starts_with("\xEF\xBB\xBF"))
            line.erase(0, 3);
        first = false;
        line = trim(line);
        if (line.empty() || line.front() == '#')
            continue;
        auto equal = line.find('=');
        if (equal == std::string::npos)
            return false;
        auto key = trim(line.substr(0, equal)), value = trim(line.substr(equal + 1));
        if (key.empty() || value.empty() || line.size() > 1024 ||
            std::any_of(value.begin(), value.end(),
                        [](unsigned char c) { return c < 32 || c > 126; }))
            return false;
        entries[key] = value;
    }
    return !input.bad();
}
Entries defaults() {
    Entries result;
    std::istringstream input(defaultFarmText);
    read(input, result);
    return result;
}
TextCatalog catalog;
std::filesystem::path source;
} // namespace
TextCatalog::TextCatalog() : entries(defaults()) {}
bool TextCatalog::load(const std::filesystem::path& path) {
    std::ifstream input(path);
    if (!input)
        return false;
    auto next = defaults();
    if (!read(input, next))
        return false;
    entries.swap(next);
    return true;
}
const std::string& TextCatalog::get(const std::string& key) const {
    const auto found = entries.find(key);
    static const std::string missing = "?";
    return found == entries.end() ? missing : found->second;
}
namespace Text {
void initialize(const std::filesystem::path& executable) {
    source = std::filesystem::absolute("assets/text.txt");
    if (!std::filesystem::exists(source))
        source = std::filesystem::absolute(executable).parent_path() / "assets/text.txt";
    reload();
}
bool reload() {
    return catalog.load(source);
}
const std::string& get(const std::string& key) {
    return catalog.get(key);
}
std::string format(const std::string& key,
                   std::initializer_list<std::pair<std::string, std::string>> values) {
    const auto& pattern = get(key);
    std::string result;
    for (size_t i = 0; i < pattern.size();) {
        auto end = pattern[i] == '{' ? pattern.find('}', i) : std::string::npos;
        if (end != std::string::npos) {
            auto name = pattern.substr(i + 1, end - i - 1);
            auto value = std::find_if(values.begin(), values.end(),
                                      [&](const auto& entry) { return entry.first == name; });
            if (value != values.end()) {
                result += value->second;
                i = end + 1;
                continue;
            }
        }
        result += pattern[i++];
    }
    return result;
}
} // namespace Text

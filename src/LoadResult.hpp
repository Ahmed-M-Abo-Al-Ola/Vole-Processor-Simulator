#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Vole {

struct LoadResult {
    bool ok = false;
    std::string err;
    std::vector<std::uint16_t> words;
};

}

#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct LoadResult {
    bool                  ok = false;
    std::string           error;
    int                   line = 0;
    std::vector<uint16_t> words;
};


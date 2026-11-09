#pragma once

#include <cstdint>
#include <fstream>
#include <string>

#include "LoadResult.hpp"

namespace Vole {

class ProgramLoader {
public:
    LoadResult loadFromFile(const std::string& filePath, std::uint8_t address);

private:
    LoadResult _loadFromStream(std::ifstream& strm, std::uint8_t address);
    std::string _trim(const std::string& str);
    int _hexVal(char c);
};

}

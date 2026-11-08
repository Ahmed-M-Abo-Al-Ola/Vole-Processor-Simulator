#pragma once
#include <istream>
#include "LoadResult.hpp"

namespace vole {
    class ProgramLoader {
        public:
            LoadResult loadFromFile(const string& filePath, const uint8_t address);
        private:
            LoadResult _loadFromStream(ifstream& strm, const uint8_t address);
    };
}
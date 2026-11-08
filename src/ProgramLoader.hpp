#pragma once
#include <istream>
#include <string>
#include <cstdint>
#include "LoadResult.hpp"

namespace Vole {
    class ProgramLoader {
        public:
            LoadResult loadFromFile(const string& filePath, const uint8_t address);
        private:
            LoadResult _loadFromStream(ifstream& strm, const uint8_t address);
            string _trim(const string& str);
            uint16_t _hexVal(const char& c);
    };
}
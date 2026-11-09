#include "ProgramLoader.hpp"

#include <cstdint>
#include <string>
#include <vector>

#include "Instruction.hpp"
#include "Text.hpp"

namespace Vole {

LoadResult ProgramLoader::loadFromFile(const std::string& filePath, std::uint8_t address) {
    std::ifstream strm(filePath);

    if (!strm.is_open()) {
        return {false, "could not open the file: " + filePath, {}};
    }

    if ((address & 1u) != 0u) {
        return {false, "address must be even", {}};
    }

    return _loadFromStream(strm, address);
}

LoadResult ProgramLoader::_loadFromStream(std::ifstream& strm, std::uint8_t address) {
    std::vector<std::uint16_t> words;
    int lineNumber = 0;
    std::string raw;

    while (std::getline(strm, raw)) {
        lineNumber++;

        const std::size_t comment = raw.find(';');
        if (comment != std::string::npos) {
            raw.erase(comment);
        }

        const std::string line = trim(raw);
        if (line.empty()) {
            continue;
        }

        if (line.size() != 4) {
            return {false,
                    "line number: " + std::to_string(lineNumber) + ": instructions must consist of 4 hex digits",
                    {}};
        }

        std::uint16_t word = 0;
        for (char c : line) {
            const int digit = _hexVal(c);
            if (digit < 0) {
                return {false, "line number: " + std::to_string(lineNumber) + ": invalid hex digit", {}};
            }
            word = static_cast<std::uint16_t>((word << 4) | static_cast<std::uint16_t>(digit));
        }

        const Instruction instruction(word);
        if (!instruction.isValid()) {
            return {false,
                    "line number: " + std::to_string(lineNumber) + ": invalid instruction: " + instruction.error(),
                    {}};
        }

        words.push_back(word);
    }

    if (static_cast<std::size_t>(address) + words.size() * 2 > 256) {
        return {false, "the program does not fit in memory", {}};
    }

    return {true, "", words};
}

int ProgramLoader::_hexVal(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1;
}

}

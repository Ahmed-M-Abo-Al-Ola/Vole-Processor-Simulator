#include "RegisterFile.hpp"

namespace Vole {

uint8 RegisterFile::get(std::size_t index) const {
    if (index >= values.size()) {
        return 0;
    }

    return values[index];
}

void RegisterFile::set(std::size_t index, uint8 value) {
    if (index < values.size()) {
        values[index] = value;
    }
}

void RegisterFile::clear() {
    values.fill(0);
}

}
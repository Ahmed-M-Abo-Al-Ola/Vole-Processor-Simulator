#include "RegisterFile.hpp"

namespace Vole {

uint8 RegisterFile::get(uint8 index) const {
    return values[index];
}

void RegisterFile::set(uint8 index, uint8 value) {
    values[index] = value;
}

void RegisterFile::clear() {
    values.fill(0);
}

}
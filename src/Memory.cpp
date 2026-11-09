#include "Memory.hpp"

namespace Vole {

uint8 Memory::read(uint8 address) const {
    return cells[address];
}

void Memory::write(uint8 address, uint8 value) {
    cells[address] = value;
}

void Memory::clear() {
    cells.fill(0);
}

}
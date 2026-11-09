#include "Screen.hpp"

#include <cstdint>
#include <vector>

namespace Vole {

void Screen::write(std::uint8_t value) {
    bytes_.push_back(value);
}

void Screen::clear() {
    bytes_.clear();
}

const std::vector<std::uint8_t>& Screen::bytes() const {
    return bytes_;
}

}

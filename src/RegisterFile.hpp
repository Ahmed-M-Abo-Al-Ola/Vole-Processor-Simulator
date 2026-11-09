#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

using uint8 = std::uint8_t;

namespace Vole {

class RegisterFile {
public:
    uint8 get(std::size_t index) const;
    void set(std::size_t index, uint8 value);
    void clear();

private:
    std::array<uint8, 16> values{};
};

}
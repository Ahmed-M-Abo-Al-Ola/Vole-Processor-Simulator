#pragma once

#include <cstdint>
#include <vector>

namespace Vole {

class Screen {
public:
    void write(std::uint8_t value);
    void clear();
    const std::vector<std::uint8_t>& bytes() const;

private:
    std::vector<std::uint8_t> bytes_;
};

}

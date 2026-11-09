#pragma once

#include <cstdint>

using uint8 = std::uint8_t;

namespace Vole {

class ALU {
public:
    uint8 addTwosComplement(uint8 a, uint8 b) const;
    uint8 addFloat(uint8 a, uint8 b) const;
    uint8 bitwiseOr(uint8 a, uint8 b) const;
    uint8 bitwiseAnd(uint8 a, uint8 b) const;
    uint8 bitwiseXor(uint8 a, uint8 b) const;
    uint8 rotateRight(uint8 value, unsigned count) const;

private:
    std::int32_t decodeFloat(uint8 value) const;
    uint8 encodeFloat(std::int32_t units) const;
};

}
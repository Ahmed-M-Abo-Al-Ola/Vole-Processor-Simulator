#include "ALU.hpp"

#include <cstdint>

namespace Vole {

uint8 ALU::addTwosComplement(uint8 a, uint8 b) const {
    return static_cast<uint8>((a + b) & 0xFFu);
}

uint8 ALU::addFloat(uint8 a, uint8 b) const {
    return encodeFloat(decodeFloat(a) + decodeFloat(b));
}

uint8 ALU::bitwiseOr(uint8 a, uint8 b) const {
    return static_cast<uint8>(a | b);
}

uint8 ALU::bitwiseAnd(uint8 a, uint8 b) const {
    return static_cast<uint8>(a & b);
}

uint8 ALU::bitwiseXor(uint8 a, uint8 b) const {
    return static_cast<uint8>(a ^ b);
}

uint8 ALU::rotateRight(uint8 value, unsigned count) const {
    for (unsigned i = 0; i < count; i++) {
        uint8 LSB = value & 0b00000001;
        LSB = LSB << 7;
        value = value >> 1;
        value = value | LSB;
    }

    return value;
}

std::int32_t ALU::decodeFloat(uint8 value) const {
    int sign = (value & 0b10000000) ? -1 : 1;
    int exponent = (value & 0b01110000) >> 4;
    int mantissa = (value & 0b00001111);

    std::int32_t decoded = sign * (mantissa << exponent);
    return decoded;
}

uint8 ALU::encodeFloat(std::int32_t units) const {
    const int minNormalizedUnits = 8;
    const int maxFloat = 1920;

    if (units > maxFloat) {
        return 0b01111111;
    }

    if (units < (maxFloat * -1)) {
        return 0b11111111;
    }

    int negative = units < 0;
    int magnitude = (units < 0) ? (-1 * units) : units;

    if (magnitude < minNormalizedUnits) {
        return 0;
    }

    int exponent = 0;
    while (magnitude > 15) {
        magnitude >>= 1;
        exponent++;
    }

    return (negative ? 0b10000000 : 0b00000000) | (exponent << 4) | magnitude;
}

}
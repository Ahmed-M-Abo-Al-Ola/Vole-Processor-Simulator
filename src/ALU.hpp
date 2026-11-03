#pragma once

#include <cstdint>

using uint8 = std::uint8_t;
using uint32 = std::uint32_t;

namespace Vole {
    class ALU {
        public:
            uint8 addTwosComplement(uint8 a, uint8 b) const;
            uint8 addFloat(uint8 a, uint8 b) const;
            uint8 bitwiseOr(uint8 a, uint8 b) const;
            uint8 bitwiseAnd(uint8 a, uint8 b) const;
            uint8 bitwiseXor(uint8 a, uint8 b) const;
            uint8 rotateRight(uint8 value, uint8 count);

        private:
            uint32 decodeFloat(uint8 value) const;
            uint8 encodeFloat(uint32 units) const;
    };
};
#pragma once
#include <cstdint>
#include <array>

using uint8 = std::uint8_t;

namespace Vole {
    class RegisterFile {
        public:
            uint8 get(uint8 index) const;
            void set(uint8 index, uint8 value);
            void clear();
        private:
            std::array<uint8, 16> data;
    };
}
#include <cstdint>
#include <array>

using uint8 = std::uint8_t;

namespace Vole {
    class Memory {
        public:
            uint8 read(uint8 address) const;
            void write(uint8 address, uint8 value);
            void clear();
        private:
            std::array<uint8, 256> cells;
    };
};
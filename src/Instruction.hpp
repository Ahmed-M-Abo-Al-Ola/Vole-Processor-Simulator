#include <cstdint>
#include <string>

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;


namespace Vole {
    class Instruction {
        public:
            // without explicit, compiler will silently convert uint16 to Instruction type
            explicit Instruction(uint16 word);

            uint8 getOpcode() const;
            uint8 getDigit2() const;
            uint8 getDigit3() const;
            uint8 getDigit4() const;
            uint8 xy() const;
            bool isValid() const;
            std::string error() const;

        private:
            uint8 opcode;
            uint8 digit2;
            uint8 digit3;
            uint8 digit4;
            bool valid;
            std::string errorMessage;
    };
};
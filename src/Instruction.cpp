#include "Instruction.hpp"
#include <string>

namespace Vole {
	Vole::Instruction::Instruction(uint16 word) 
	:	opcode{static_cast<uint8>((word >> 12) & 0b1111)},
		digit2{static_cast<uint8>((word >> 8) & 0b1111)},
		digit3{static_cast<uint8>((word >> 4) & 0b1111)},
		digit4{static_cast<uint8>(word & 0b1111)}
	{
		valid = false;
		if (opcode < 1 || opcode > 12) {
			errorMessage = "Unknown opcode " + std::to_string(static_cast<int>(opcode));
		} else if (opcode == 4 && digit2 != 0) {
			errorMessage = "Digit 2 must be 0 in MOVE operation";
		} else if (opcode == 10 && digit3 != 0) {
			errorMessage = "Digit 3 must be 0 in ROTATE operation";
		} else if (opcode == 12 && (digit2 != 0 || digit3 != 0 || digit4 != 0)) {
			errorMessage = "All digits must be zero in HALT operation";
		} else {
			valid = true;
		}
	}

    uint8 Vole::Instruction::getOpcode() const {
    	return opcode;
    }
    
    uint8 Vole::Instruction::getDigit2() const {
    	return digit2;
    }
    
    uint8 Vole::Instruction::getDigit3() const {
    	return digit3;
    }
    
    uint8 Vole::Instruction::getDigit4() const {
    	return digit4;
    }
    
    uint8 Vole::Instruction::xy() const {
    	return (digit3 << 4) | digit4;
    }
    
    bool Vole::Instruction::isValid() const {
    	return valid;
    }
    
    std::string Vole::Instruction::error() const {
    	return errorMessage;
    }
}

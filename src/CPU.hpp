#include <cstdint>
#include "ALU.hpp"
#include "Memory.hpp"
#include "RegisterFile.hpp"
#include "Instruction.hpp"
#include "Types.hpp"

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;

namespace Vole {
	class CPU {
		public:
			explicit CPU(Memory& memory); // add Screen&
			StepResult step();
			void reset();
			void setPc(uint8 value);
			void setRegister(std::size_t index, uint8 value);
			const uint8& getPc() const;
			const uint8& getIr() const;
			const RegisterFile& getRegisters() const;
			const bool& isHalted() const;
			
		private:
			Memory& memory;
			RegisterFile registers;
			ALU alu;
			uint8 pc;
			uint8 ir;
			bool halted;

			uint16 fetchWord() const;
			StepResult execute(const Instruction& instruction);
			void store(uint8 address, uint8 value);
	};
};
#include "CPU.hpp"
#include "Types.hpp"

namespace Vole {
	CPU::CPU(Memory& memory)
	: memory{memory}
	{ // add Screen

	}

	StepResult CPU::step() {
		if (halted) {
			return {StepStatus::HALTED, "CPU was halted"};
		}

		if (pc == 0xFF) {
			return {StepStatus::FETCH_ERROR, "Error while fetching instruction"};
		}

		ir = fetchWord();
		pc += 2;
		Instruction instruction{ir};
		if (!instruction.isValid()) {
			return {StepStatus::INVALID_INSTRUCTION, instruction.error()};
		}

		return execute(instruction);
	}

	void CPU::reset() {
		pc = 0;
		ir = 0;
		registers.clear();
		halted = false;
	}

	void CPU::setPc(uint8 value) {
		pc = value;
	}

	void CPU::setRegister(std::size_t index, uint8 value) {
		// TODO: Check if it is out of bounds
		registers.set(index, value);
	}

	const uint8& CPU::getPc() const {
		return pc;
	}

	const uint8& CPU::getIr() const {
		return ir;
	}

	const RegisterFile& CPU::getRegisters() const {
		return registers;
	}

	const bool& CPU::isHalted() const {
		return halted;
	}

	uint16 CPU::fetchWord() const {
		return ((static_cast<uint16>(memory.read(pc)) << 8) | memory.read(pc+1));
	}

	StepResult CPU::execute(const Instruction& instruction) {
		const uint8 opcode = instruction.getOpcode();
		const uint8 digit2 = instruction.getDigit2();
		const uint8 digit3 = instruction.getDigit3();
		const uint8 digit4 = instruction.getDigit4();
		const uint8 xy = instruction.xy();
		const uint8 S = registers.get(digit3);
		const uint8 T = registers.get(digit4);

		
		switch (opcode) {
			case 0x1:
				setRegister(digit2, memory.read(xy));
				break;
			case 0x2:
				setRegister(digit2, xy);
				break;
			case 0x3:
				memory.write(xy, digit2);
				break;
			case 0x4:
				registers.set(digit3, registers.get(digit4));
				break;
			case 0x5:
				registers.set(digit2, alu.addTwosComplement(S, T));
				break;
			case 0x6:
				registers.set(digit2, alu.addFloat(S, T));
				break;
			case 0x7:
				registers.set(digit2, alu.bitwiseOr(S, T));
				break;
			case 0x8:
				registers.set(digit2, alu.bitwiseAnd(S, T));
				break;
			case 0x9:
				registers.set(digit2, alu.bitwiseXor(S, T));
				break;
			case 0xA:
				registers.set(digit2, alu.rotateRight(registers.get(digit2), digit4));
				break;	
			case 0xB:
				if (registers.get(digit2) == 0) {
					pc = xy;
				}
				break;
			case 0xC:
				halted = true;
				break;
		}

		if (halted == true) {
			return {StepStatus::HALTED, "Execution was halted"};
		} else {
			return {StepStatus::EXECUTED, "Instruction was executed"};
		}
		
	}

	void CPU::store(uint8 address, uint8 value) {
		if (value == 0xFF) {
			// value == 0xFF, it goes to screen
		} else {
			memory.write(address, value);
		}
	}
}

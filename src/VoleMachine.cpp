#include "VoleMachine.hpp"

namespace Vole {

VoleMachine::VoleMachine() : memory_(), screen_(), cpu_(memory_, screen_), loader_() {
    reset();
}

LoadResult VoleMachine::loadProgram(const std::string& path, std::uint8_t address) {
    LoadResult result = loader_.loadFromFile(path, address);
    if (!result.ok) {
        return result;
    }

    const std::size_t bytes = result.words.size() * 2;
    if (static_cast<std::size_t>(address) + bytes > 256) {
        result.ok = false;
        result.err = "program does not fit in memory at the given address";
        return result;
    }

    reset();
    for (std::size_t i = 0; i < result.words.size(); ++i) {
        const std::uint16_t word = result.words[i];
        const std::size_t at = static_cast<std::size_t>(address) + 2 * i;
        memory_.write(static_cast<std::uint8_t>(at), static_cast<std::uint8_t>(word >> 8));
        memory_.write(static_cast<std::uint8_t>(at + 1), static_cast<std::uint8_t>(word & 0xFF));
    }
    cpu_.setPc(address);
    return result;
}

StepResult VoleMachine::step() {
    return cpu_.step();
}

StepResult VoleMachine::run(std::size_t maxSteps) {
    for (std::size_t i = 0; i < maxSteps; ++i) {
        StepResult result = cpu_.step();
        if (result.status != StepStatus::EXECUTED) {
            return result;
        }
    }
    return StepResult{StepStatus::STEP_LIMIT, "step limit reached"};
}

void VoleMachine::reset() {
    memory_.clear();
    screen_.clear();
    cpu_.reset();
}

void VoleMachine::setRegister(std::size_t index, std::uint8_t value) {
    cpu_.setRegister(index, value);
}

void VoleMachine::setMemory(std::uint8_t address, std::uint8_t value) {
    memory_.write(address, value);
}

const Memory& VoleMachine::memory() const {
    return memory_;
}
const CPU& VoleMachine::cpu() const {
    return cpu_;
}
const Screen& VoleMachine::screen() const {
    return screen_;
}

}
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "CPU.hpp"
#include "LoadResult.hpp"
#include "Memory.hpp"
#include "ProgramLoader.hpp"
#include "Screen.hpp"
#include "StepResult.hpp"

namespace Vole {

class VoleMachine {
private:
    Memory memory_;
    Screen screen_;
    CPU cpu_;
    ProgramLoader loader_;

public:
    VoleMachine();
    LoadResult loadProgram(const std::string& path, std::uint8_t address);
    StepResult step();
    StepResult run(std::size_t maxSteps);
    void reset();
    void setRegister(std::size_t index, std::uint8_t value);
    void setMemory(std::uint8_t address, std::uint8_t value);
    const Memory& memory() const;
    const CPU& cpu() const;
    const Screen& screen() const;
};

}

#pragma once
#include "Memory.hpp"
#include "Screen.hpp"
#include "CPU.hpp"
#include "ProgramLoader.hpp"
#include "LoadResult.hpp"
#include "StepResult.hpp"
#include <cstdint>
#include <cstddef>
#include <string>

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

#endif
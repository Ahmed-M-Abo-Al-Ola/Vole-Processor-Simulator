#pragma once

#include <string>

namespace Vole {

class CPU;
class Memory;
class Screen;

class StateRenderer {
public:
    std::string render(const Memory& memory, const CPU& cpu, const Screen& screen) const;

private:
    std::string renderMemory(const Memory& memory) const;
    std::string renderRegisters(const CPU& cpu) const;
    std::string renderCpu(const CPU& cpu) const;
    std::string renderScreen(const Screen& screen) const;
};

}

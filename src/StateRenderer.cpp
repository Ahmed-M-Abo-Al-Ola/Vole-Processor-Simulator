#include "StateRenderer.hpp"

#include <cstdint>
#include <string>

#include "CPU.hpp"
#include "Screen.hpp"
#include "Text.hpp"

namespace Vole {
namespace {

std::string hexWord(std::uint16_t value) {
    std::string text;
    text += hexByte(value / 256);
    text += hexByte(value % 256);
    return text;
}

}

std::string StateRenderer::render(const Memory& memory, const CPU& cpu, const Screen& screen) const {
    std::string text;

    text += renderMemory(memory);
    text += '\n';

    text += renderRegisters(cpu);
    text += '\n';

    text += renderCpu(cpu);
    text += '\n';

    text += renderScreen(screen);

    return text;
}

std::string StateRenderer::renderMemory(const Memory& memory) const {
    std::string text = "Memory (16x16):\n";

    text += "    ";
    for (unsigned column = 0; column < 16; ++column) {
        text += hexDigit(column);
        text += "  ";
    }
    text += '\n';

    for (unsigned row = 0; row < 16; ++row) {
        text += hexDigit(row);
        text += "0:";

        for (unsigned column = 0; column < 16; ++column) {
            const unsigned address = row * 16 + column;
            text += ' ';
            text += hexByte(memory.read(static_cast<std::uint8_t>(address)));
        }

        text += '\n';
    }

    return text;
}

std::string StateRenderer::renderRegisters(const CPU& cpu) const {
    std::string text = "Registers:\n";

    for (unsigned index = 0; index < 16; ++index) {
        text += 'R';
        text += hexDigit(index);
        text += '=';
        text += hexByte(cpu.getRegisters().get(static_cast<std::uint8_t>(index)));

        if ((index + 1) % 4 == 0) {
            text += '\n';
        } else {
            text += "  ";
        }
    }

    return text;
}

std::string StateRenderer::renderCpu(const CPU& cpu) const {
    std::string text;

    text += "PC=";
    text += hexByte(cpu.getPc());
    text += "  IR=";
    text += hexWord(cpu.getIr());
    text += "  halted=";
    text += cpu.isHalted() ? "yes" : "no";
    text += '\n';

    return text;
}

std::string StateRenderer::renderScreen(const Screen& screen) const {
    const auto& bytes = screen.bytes();
    std::string text = "Screen: ";

    if (bytes.empty()) {
        text += "(empty)";
    } else {
        for (unsigned index = 0; index < bytes.size(); ++index) {
            if (index != 0) {
                text += ' ';
            }
            text += hexByte(bytes[index]);
        }
    }

    text += '\n';
    return text;
}

}

#include "MenuUI.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <istream>
#include <ostream>
#include <string>

#include "LoadResult.hpp"
#include "StepResult.hpp"
#include "Text.hpp"
#include "VoleMachine.hpp"

namespace Vole {

MenuUI::MenuUI(VoleMachine& machine, std::istream& in, std::ostream& out) : machine_(machine), in_(in), out_(out) {}

void MenuUI::run() {
    while (running_) {
        showMenu();

        unsigned choice = 0;
        if (!readHex("Choice: ", 7, choice)) {
            break;
        }

        switch (choice) {
            case 1:
                handleLoad();
                break;
            case 2:
                handleEditRegister();
                break;
            case 3:
                handleEditMemory();
                break;
            case 4:
                handleDisplay();
                break;
            case 5:
                handleStep();
                break;
            case 6:
                handleRun();
                break;
            case 7:
                running_ = false;
                break;
            default:
                out_ << "Enter a number from 1 to 7.\n";
                break;
        }
    }
}

void MenuUI::showMenu() const {
    out_ << "\n=== Vole Machine ===\n";
    out_ << "1. Load a program\n";
    out_ << "2. Edit a register\n";
    out_ << "3. Edit memory\n";
    out_ << "4. Display machine state\n";
    out_ << "5. Step\n";
    out_ << "6. Run\n";
    out_ << "7. Exit\n";
}

void MenuUI::handleLoad() {
    const std::string path = readLine("Program file: ");
    if (!running_) {
        return;
    }

    unsigned address = 0;
    if (!readHex("Load address (even, 00-FE): ", 0xFE, address)) {
        return;
    }

    if ((address & 1u) != 0) {
        out_ << "Load address must be even.\n";
        return;
    }

    const LoadResult result = machine_.loadProgram(path, static_cast<std::uint8_t>(address));

    if (!result.ok) {
        out_ << "Load failed: " << result.error << '\n';
        return;
    }

    out_ << "Loaded " << result.words.size() << " word(s) at 0x" << hexByte(address) << ".\n";
}

void MenuUI::handleEditRegister() {
    unsigned index = 0;
    if (!readHex("Register (0-F): ", 0xF, index)) {
        return;
    }

    unsigned value = 0;
    if (!readHex("Value (00-FF): ", 0xFF, value)) {
        return;
    }

    machine_.setRegister(index, static_cast<std::uint8_t>(value));
    out_ << 'R' << hexDigit(index) << " = 0x" << hexByte(value) << '\n';
}

void MenuUI::handleEditMemory() {
    unsigned address = 0;
    if (!readHex("Address (00-FF): ", 0xFF, address)) {
        return;
    }

    unsigned value = 0;
    if (!readHex("Value (00-FF): ", 0xFF, value)) {
        return;
    }

    machine_.setMemory(static_cast<std::uint8_t>(address), static_cast<std::uint8_t>(value));
    out_ << "memory[" << hexByte(address) << "] = 0x" << hexByte(value) << '\n';

    if (address == 0xFFu) {
        out_ << "Note: direct memory edits write RAM; CPU 3RFF writes the screen.\n";
    }
}

void MenuUI::handleDisplay() {
    displayState();
}

void MenuUI::handleStep() {
    displayState();

    StepResult result;
    while (true) {
        if (!waitForEnter()) {
            return;
        }

        result = machine_.step();
        displayState();

        if (result.status != StepStatus::EXECUTED) {
            break;
        }
    }

    if (!result.message.empty()) {
        out_ << result.message << '\n';
    }

    if (promptAfterRun()) {
        handleLoad();
    } else {
        running_ = false;
    }
}

void MenuUI::handleRun() {
    const std::size_t maxSteps = 1000000;
    const StepResult result = machine_.run(maxSteps);

    displayState();

    if (!result.message.empty()) {
        out_ << result.message << '\n';
    }

    if (promptAfterRun()) {
        handleLoad();
    } else {
        running_ = false;
    }
}

void MenuUI::displayState() {
    out_ << renderer_.render(machine_.memory(), machine_.cpu(), machine_.screen());
}

bool MenuUI::promptAfterRun() {
    while (true) {
        const std::string answer = trim(readLine("Load another program? [l]oad / [x]it: "));

        if (!running_) {
            return false;
        }

        if (answer == "l" || answer == "L" || answer == "load" || answer == "LOAD") {
            return true;
        }

        if (answer == "x" || answer == "X" || answer == "exit" || answer == "EXIT") {
            return false;
        }

        out_ << "Please enter l or x.\n";
    }
}

bool MenuUI::readHex(const std::string& prompt, unsigned max, unsigned& value) {
    while (true) {
        const std::string text = trim(readLine(prompt));
        if (!running_) {
            return false;
        }

        if (text.empty()) {
            out_ << "Enter a hex value.\n";
            continue;
        }

        try {
            std::size_t consumed = 0;
            const unsigned long parsed = std::stoul(text, &consumed, 16);

            if (consumed != text.size()) {
                out_ << "Invalid hex value.\n";
                continue;
            }

            if (parsed > max) {
                out_ << "Value must be between 00 and " << hexByte(max) << ".\n";
                continue;
            }

            value = static_cast<unsigned>(parsed);
            return true;
        } catch (const std::exception&) {
            out_ << "Invalid hex value.\n";
        }
    }
}

std::string MenuUI::readLine(const std::string& prompt) {
    out_ << prompt;

    std::string line;
    if (!std::getline(in_, line)) {
        running_ = false;
        return {};
    }

    return line;
}

bool MenuUI::waitForEnter() {
    out_ << "Press Enter to step, q to return to menu: ";

    std::string line;
    if (!std::getline(in_, line)) {
        running_ = false;
        return false;
    }

    const std::string answer = trim(line);
    return answer != "q" && answer != "Q";
}

}

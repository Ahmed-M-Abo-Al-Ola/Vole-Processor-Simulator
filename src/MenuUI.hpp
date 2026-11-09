#pragma once

#include <iosfwd>
#include <string>

#include "StateRenderer.hpp"

namespace Vole {

class VoleMachine;

class MenuUI {
public:
    MenuUI(VoleMachine& machine, std::istream& in, std::ostream& out);

    void run();

private:
    void showMenu() const;
    void handleLoad();
    void handleEditRegister();
    void handleEditMemory();
    void handleDisplay();
    void handleStep();
    void handleRun();
    void displayState();
    bool promptAfterRun();

    bool readHex(const std::string& prompt, unsigned max, unsigned& value);
    std::string readLine(const std::string& prompt);
    bool waitForEnter();

    VoleMachine& machine_;
    StateRenderer renderer_;
    std::istream& in_;
    std::ostream& out_;
    bool running_ = true;
};

}

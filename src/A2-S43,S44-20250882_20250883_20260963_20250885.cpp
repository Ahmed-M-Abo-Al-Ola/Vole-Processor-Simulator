#include <iostream>

#include "MenuUI.hpp"
#include "VoleMachine.hpp"

int main() {
    Vole::VoleMachine machine;
    Vole::MenuUI ui(machine, std::cin, std::cout);
    ui.run();
    return 0;
}

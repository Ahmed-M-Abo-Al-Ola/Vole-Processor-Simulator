#include <iostream>
#include "MenuUI.hpp"
#include "VoleMachine.hpp"
int main() {
    VoleMachine machine;
    MenuUI ui(machine, std::cin, std::cout);
    ui.run();
    return 0;
}

#include "Screen.hpp"
#include <vector>
using namespace std;

void vole::Screen::write(uint8_t val) {
    bytes_.push_back(val);
}

void vole::Screen::clear() {
    bytes_.clear();
}

vector<uint8_t> vole::Screen::getBytes() const {
    return bytes_;
}
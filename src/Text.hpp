#pragma once

#include <cstddef>
#include <string>

namespace Vole {

std::string trim(const std::string& text);
char hexDigit(unsigned value);
std::string hexByte(unsigned value);

}

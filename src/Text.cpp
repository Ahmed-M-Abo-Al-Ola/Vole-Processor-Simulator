#include "Text.hpp"

#include <cctype>
#include <cstddef>

namespace Vole {

std::string trim(const std::string& text) {
    std::size_t begin = 0;
    while (begin < text.size() && std::isspace(static_cast<unsigned char>(text[begin])) != 0) {
        ++begin;
    }

    std::size_t end = text.size();
    while (end > begin && std::isspace(static_cast<unsigned char>(text[end - 1])) != 0) {
        --end;
    }

    return text.substr(begin, end - begin);
}

char hexDigit(unsigned value) {
    const char digits[] = "0123456789ABCDEF";
    return digits[value % 16];
}

std::string hexByte(unsigned value) {
    std::string text;
    text += hexDigit(value / 16);
    text += hexDigit(value % 16);
    return text;
}

}

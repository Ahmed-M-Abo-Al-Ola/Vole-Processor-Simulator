#include "RegisterFile.hpp"

using namespace std;

namespace Vole {
    uint8 RegisterFile::get(uint8 index) const {
        return data[index];
    }

    void RegisterFile::set(uint8 index, uint8 value) {
        data[index] = value;
    }

    void RegisterFile::clear() {
        data.fill(0); // array elements cannot be removed entirely
    }
}
#pragma once
#include <vector>
#include <cstdint>
using namespace std;

namespace vole
{
    class Screen
    {
    public:
        void write(uint8_t val);
        void clear();
        vector<uint8_t> getBytes() const;

    private:
        vector<uint8_t> bytes_;
    };
}
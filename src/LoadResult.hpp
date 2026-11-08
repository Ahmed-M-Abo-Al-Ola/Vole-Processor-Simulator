#pragma once
#include <string>
#include <vector>
using namespace std;

namespace Vole
{
    struct LoadResult
    {
        bool ok = false;
        string err;
        int line = 0;
        vector<uint16_t> words;
    };
}

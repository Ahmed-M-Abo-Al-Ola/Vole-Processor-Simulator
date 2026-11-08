#include <fstream>
#include "ProgramLoader.hpp"
#include "LoadResult.hpp"
using namespace std;

vole::LoadResult vole::ProgramLoader::loadFromFile(const string &filePath, const uint8_t address)
{
    ifstream strm(filePath);

    // check if file couldn't be opened
    if (!strm.is_open())
    {
        return {false, "could not open the file: " + filePath, 0, {}};
    }

    // check if address is odd
    if (address & 1u != 0)
    {
        return {false, "address must be even", 0, {}};
    }

    // check if address is 0xFF or greater
    if (address > 0xFE)
    {
        return {false, "address must be withing 0x00 and 0xFE", 0, {}};
    }

    return _loadFromStream(strm, address);
}
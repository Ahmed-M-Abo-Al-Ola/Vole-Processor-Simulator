#include <fstream>
#include <vector>
#include <string>
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

vole::LoadResult vole::ProgramLoader::_loadFromStream(ifstream &strm, const uint8_t address)
{
    vector<uint16_t> words;
    int lineNumber = 0;
    string raw;

    while (getline(strm, raw))
    {
        lineNumber++;

        // remove comments
        const int comment = raw.find(';');
        if (comment != string::npos)
            raw.erase(comment);
    }

    return {true, "", 0, words};
}

string vole::ProgramLoader::_trim(const string &str)
{
    int l = 0, n = str.length();
    while (l < n && str[l] == ' ')
        l++;

    int r = n - 1;
    while (n >= 0 && str[r] == ' ')
        r--;

    return str.substr(l, r - l + 1);
}

uint16_t vole::ProgramLoader::hexVal(const char &c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}
#include "ProgramLoader.hpp"
#include "LoadResult.hpp"
#include "Instruction.hpp"
#include <fstream>
#include <vector>
#include <string>
using namespace std;

Vole::LoadResult Vole::ProgramLoader::loadFromFile(const string &filePath, const uint8_t address)
{
    ifstream strm(filePath);

    // check if file couldn't be opened
    if (!strm.is_open())
    {
        return LoadResult{false, "could not open the file: " + filePath, 0, {}};
    }

    // check if address is odd
    if (address & 1u != 0)
    {
        return LoadResult{false, "address must be even", 0, {}};
    }

    // check if address is 0xFF or greater
    if (address > 0xFE)
    {
        return LoadResult{false, "address must be withing 0x00 and 0xFE", 0, {}};
    }

    return _loadFromStream(strm, address);
}

Vole::LoadResult Vole::ProgramLoader::_loadFromStream(ifstream &strm, const uint8_t address)
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

        // remove spaces
        string line = _trim(raw);
        if (line.empty())
            continue;

        const auto fail = [lineNumber](const string &message)
        {
            return LoadResult{
                false,
                "line number: " + to_string(lineNumber) + ": " + message,
                0,
                {}};
        };

        // check for words longer or shorter than 4 characters
        if (line.size() != 4)
            return fail("instructions must consist of 4 hex digits");

        // decode the digits into an uint16_t
        uint16_t word = 0;
        for (const char &c : line)
        {
            uint16_t digit = _hexVal(c);
            if (digit < 0)
                return fail("invalid hex digit");
            word = (word << 4) | static_cast<uint16_t>(digit);
        }

        // verify the word is a valid instruction
        const Instruction instruction(word);
        if (!instruction.isValid())
            return fail("invalid instruction: " + instruction.error());

        words.push_back(word);
    }

    if (address + (words.size() * 2) > 256)
        return LoadResult{false, "the program does not fit in memory", 0, {}};

    return {true, "", 0, words};
}

string Vole::ProgramLoader::_trim(const string &str)
{
    int l = 0, n = str.length();
    while (l < n && str[l] == ' ')
        l++;

    int r = n - 1;
    while (n >= 0 && str[r] == ' ')
        r--;

    return str.substr(l, r - l + 1);
}

uint16_t Vole::ProgramLoader::_hexVal(const char &c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}
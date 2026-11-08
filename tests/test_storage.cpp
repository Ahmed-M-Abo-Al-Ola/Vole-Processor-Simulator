#include "../src/ProgramLoader.hpp"
#include "../src/LoadResult.hpp"
#include "check.hpp"
#include <string>
#include <vector>
#include <cstdint>
using namespace std;

void testProgramLoader()
{
    Vole::ProgramLoader loader;

    // loading program 1
    {
        const Vole::LoadResult res = loader.loadFromFile("programs/program1.vol", 0x00);
        check(res.ok, "program1.vol did not load");
        check(res.words.size() == 8, "program1.vol collected an incorrect number of instructions");
        vector<uint16_t> expected = {0x2005, 0x2107, 0x2203, 0x5301, 0x5423, 0x3400, 0x34FF, 0xC000};
        for (int i = 0; i < res.words.size(); i++)
        {
            check(res.words[i] == expected[i], "program1.vol, instruction no." +
                                                   to_string(i) + " did not load correctly.\nLoaded value: " + to_string(res.words[i]));
        }
    }

    // loading program 2
    {
        const Vole::LoadResult res = loader.loadFromFile("programs/program2.vol", 0x00);
        check(res.ok, "program2.vol did not load");
        check(res.words.size() == 7, "program2.vol collected an incorrect number of instructions");
        vector<uint16_t> expected = {0x2000, 0x213A, 0x22C5, 0x8312, 0x3300, 0x33FF, 0xC000};
        for (int i = 0; i < res.words.size(); i++)
        {
            check(res.words[i] == expected[i], "program1.vol, instruction no." +
                                                   to_string(i) + " did not load correctly.\nLoaded value: " + to_string(res.words[i]));
        }
    }

    // loading an imaginary file
    {
        const Vole::LoadResult res = loader.loadFromFile("programs/program3.vol", 0x00);
        check(!res.ok, "program3.vol did load even though it does not exist");
        check(res.err == "could not open the file: programs/program3.vol",
              "program3.vol loading generated an incorrect error message: " + res.err);
        check(res.words.size() == 0,
              "program3.vol collected instructions even though it does not exist");
    }

    // loading with an uneven address
    {
        const Vole::LoadResult res = loader.loadFromFile("programs/program1.vol", 0x0B);
        check(!res.ok, "program1.vol loaded with an odd address");
        check(res.err == "address must be even",
              "program1.vol loading with an even address generated an incorrect error message: " + res.err);
    }

    // loading with no enough space for instructions
    {
        const Vole::LoadResult res = loader.loadFromFile("programs/program2.vol", 0xFA);
        check(!res.ok, "program2.vol loaded even though there is not enough space for instructions");
        check(res.err == "the program does not fit in memory",
              "program2.vol loading without enough memory space generated an incorrect error message: " + res.err);
    }
}

int main(int argc, char const *argv[])
{
    testProgramLoader();
    return 0;
}

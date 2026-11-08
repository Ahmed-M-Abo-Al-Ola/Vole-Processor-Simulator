#include <fstream>
#include "ProgramLoader.hpp"
#include "LoadResult.hpp"
using namespace std;

vole::LoadResult vole::ProgramLoader::loadFromFile(const string& filePath, const uint8_t address) {
    ifstream strm(filePath);
    if (!strm.is_open()) {
        return {false, "could not open the file: " + filePath, 0, {}};
    } 
    return _loadFromStream(strm, address);
}
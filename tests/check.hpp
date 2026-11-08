#include <string>
#include <iostream>
using namespace std;

void check(bool condition, const string &label)
{
    if (!condition)
        cerr << "FAIL: " << label << "\n";
}
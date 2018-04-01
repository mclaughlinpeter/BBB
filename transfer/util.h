#ifndef UTIL_H_
#define UTIL_H_
#include<string>

using std::string;

namespace exploringBB
{
    int write(string path, string filename, string value);
    int write(string path, string filename, int value);
    string read(string path, string filename);
} // namespace exploringBB

#endif
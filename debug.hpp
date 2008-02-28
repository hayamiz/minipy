#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <string>
#include <vector>
#include <map>

#include "symbol.hpp"

using namespace std;

class SrcFiles {
private:
    static map<symbol_t, vector<symbol_t> *> files;
    SrcFiles(){};
public:
    static void addfile(const string & file);
    static void addline(const string & file, const string & line);

    static string* getline(const string & file, int lineno);
};

#endif

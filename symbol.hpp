#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <map>
#include <string>

using namespace std;

typedef string* symbol_t;

class Symbol {
private:
    Symbol(){};
    static map<string, symbol_t> data;
public:
    static symbol_t get(const string & str);
};

#endif

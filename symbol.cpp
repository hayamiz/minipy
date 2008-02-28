#include "symbol.hpp"

map<string, symbol_t> Symbol::data;

symbol_t Symbol::get(const string & str){
    if( Symbol::data.find(str) == Symbol::data.end()){
        symbol_t sym = new string(str);
        Symbol::data[str] = sym;
    }

    return Symbol::data[str];
}

#ifndef ENV_HPP
#define ENV_HPP

#include <string>
#include <map>
#include "pyvalues.hpp"
#include "syntax_tree.hpp"
#include "symbol.hpp"

class Env{
public:
    Env();
    void set_sym(symbol_t s, Py_val* v);
    py_val_t lookup_sym(symbol_t s);
//    void set(string s, Py_val* v);
//    py_val_t lookup(string s);
    void set_global(symbol_t s);
    bool is_global(symbol_t s);
    void debug_print_all();
    uint size();
private:
    map<symbol_t, py_val_t> env;
};

#endif

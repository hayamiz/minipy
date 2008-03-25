#ifndef PRINTER_HPP
#define PRINTER_HPP

#include <ostream>
#include <string>
#include <cstdio>

#include "tokenizer.hpp"
#include "parser.hpp"
#include "syntax_tree.hpp"

using namespace std;

class Printer {
private:
    Printer(){};
    static map<token_kind_t, string> operator_map;
    static bool initp;
    static void init();

    static void print_expr_list_with_comma(vector<Expr*> & expr_list);
    static void print_if_branches(vector<StmIfBranch*> & branches, int indent_level);
public:
    static void print_expr(Expr & expr);
    static void print_stm(Stm & stm, int indent_level);
};

#endif

#ifndef EVAL_HPP
#define EVAL_HPP

#include <stack>
#include <string>
#include <vector>
#include "pyvalues.hpp"
#include "syntax_tree.hpp"
#include "env.hpp"
#include "native.hpp"

class Eval{
public:
    py_val_t expr(Expr & e, Env & lenv, Env & genv,stack<Stack_trace_entry> & bt);
    py_val_t stmt(Stm & s, Env & lenv, Env & genv,stack<Stack_trace_entry> & bt);
    py_val_t file_input(vector<Stm> & u, Env & genv,stack<Stack_trace_entry> & bt);
    
private:
    py_val_t expr_function_call(py_val_t f,py_val_t * args, Expr & e, Env & lenv, Env & genv,stack<Stack_trace_entry> & bt);
    symbol_t operator_fun_name(token_kind_t k, fix_kind_t fix, stack<Stack_trace_entry> & bt, SrcPos & p);
    py_val_t stmt_vec(vector<Stm> s, Env & lenv, Env & genv, stack<Stack_trace_entry> & bt);
};


#endif

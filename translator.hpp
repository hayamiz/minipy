#ifndef TRANSLATOR_HPP
#define TRANSLATOR_HPP

class Translator;

#include <string>
#include <queue>
#include "syntax_tree.hpp"
#include "vminsns.hpp"
#include "pyvalues.hpp"
#include "env.hpp"

using namespace std;

class LocalEnv {
private:
    int offset;
    map<symbol_t, int> table;

    // if the instance is local table, 'offset' holds the offset from frame pointer FP.
    map<symbol_t, int> globals;

public:
    LocalEnv(const symbol_t name);

    symbol_t name;
    py_val_t self;

    bool is_toplevel;
    queue<int> loopvar_queue;

    int current_offset();
    void mark_global(const symbol_t var);
    bool is_global(const symbol_t var);
    void add_var(const symbol_t var);
    void add_var();
    void pop_var();
    int look_var(const symbol_t var);
    bool exists(const symbol_t var);
    int size();
};

class Translator {
private:
    Env * genv;

    void insns_val_infix_operator(const Expr & expr, vm_assembler & vmasm, LocalEnv * lenv);
    void scan_defun_local_var(const vector<Stm*> & stms, LocalEnv * lenv);

    bool numeric_expr(const Expr & expr);
    void insns_val(const Expr & expr, vm_assembler & vmasm, LocalEnv * lenv);
    void insns_push(const Expr & expr, vm_assembler & vmasm, LocalEnv * lenv);
    void insns_stm(const Stm & stm, vm_assembler & vmasm, LocalEnv * lenv);
    void insns_stms(const vector<Stm*> & stm, vm_assembler & vmasm, LocalEnv * lenv);

    void insns_loop_stms(const vector<Stm*> & stm, vm_assembler & vmasm, int start_addr, vector<int> & break_addrs, LocalEnv * lenv);
    
    void compile_error(const string & msg, const SrcPos & pos);

public:
    Translator();

    void compile_file(const vector<Stm*> & stms, vm_assembler & vmasm, Env & genv);
};

#endif

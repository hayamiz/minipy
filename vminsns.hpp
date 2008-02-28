#ifndef VMINSNS_HPP
#define VMINSNS_HPP

#include "pyvalues.hpp"
#include "symbol.hpp"
#include "env.hpp"
#include "native.hpp"
#include <vector>

using namespace std;

#include "vminsns.inc"

struct vm_inst {
    vm_insn_type type;
    void* operand;
    const SrcPos * p;
};

typedef enum {
    vm_operand_name,
    vm_operand_pyval,
    vm_operand_int
} vm_operand_kind;

class vm_operand{
public:
    vm_operand_kind kind;
    union {
        symbol_t name;
        py_val_t pyval;
        int i;
    } u;

    static vm_operand name(const symbol_t name);
    static vm_operand pyval(py_val_t pyval);
    static vm_operand integer(int i);
};

class vm_assembler {
private:
    map<symbol_t, int> labels;
    
public:
    int label_index;
    vector<vm_inst> insns;

    vm_assembler();
    
    vm_assembler & add(vm_insn_type type, const SrcPos & p);
    vm_assembler & add(vm_insn_type type, vm_operand operand, const SrcPos & p);
    
    vm_assembler & label(const string & str);
    void fundef(const symbol_t name, int arity, int locals, Env & genv, const SrcPos & p);

    vm_inst * assemble();
    void print(Env & genv);
    string operand_str(vm_insn_type type, py_val_t operand, Env & genv);
};

#endif

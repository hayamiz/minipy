#ifndef VM_HPP
#define VM_HPP

#include <stack>
#include "parser.hpp"
#include "translator.hpp"
#include "native.hpp"

using namespace std;

#define MAX_STACK_SIZE 100000

class VmEnv {
public:
    VmEnv * up;
    int argp;
    int fp;
};

struct tivi_insn {
    vm_insn_type type;
    void * operand;
    SrcPos * p;
};

class Tivi {
private:
    Env genv;
    vm_assembler vmasm;
    Translator trans;

    int lifetime;

    // stack
    py_val_t * machine_stack;
    stack<int> return_stack;
    
    // set of registers
    int program_counter; // PC
    int stack_pointer; // SP
    int frame_pointer; // FP
    py_val_t value; // VAL
    VmEnv * env; // ENV
    vm_insn_type inst; // INST


#define PC (program_counter)
#define SP (stack_pointer)
#define FP (frame_pointer)
#define VAL (value)
#define ENV (env)
#define ARGP (env->argp)
#define INST (inst = insns[PC].type)


// #define TIVI_FETCH() (inst = this->vmasm.insns[PC].type)
// #define TIVI_FETCH_OPERAND() (py_val_t)(this->vmasm.insns[PC].operand)
// #define TIVI_FETCH_SRCPOS() (*this->vmasm.insns[PC].p)


public:
    Tivi();

    void load_file(const string & filename);
    void run(int entry_point);
    void presetup(tivi_insn ** insns, symbol_t ** genv_rev);

    void runtime_error(const string & msg, stack<Stack_trace_entry> & bt, int program_counter);

    void disasm();
};

#endif

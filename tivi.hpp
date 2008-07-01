#ifndef VM_HPP
#define VM_HPP

class Tivi;

#include <stack>
#include <errno.h>
#include "parser.hpp"
#include "translator.hpp"
#include "native.hpp"
#include "thread.hpp"

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
    vm_assembler * vmasm;
    Translator * trans;

    int lifetime;

    // stack
    py_val_t * machine_stack;
    stack<int> return_stack;
    bool is_thread;
    
    // set of registers
    int program_counter; // PC
    int stack_pointer; // SP
    int frame_pointer; // FP
    py_val_t value; // VAL
    VmEnv * env; // ENV
    vm_insn_type inst; // INST

    tivi_insn * insns;
    int insns_size;

    // dictionary of global environment
    py_val_t * globals;
    // reverse dictionary of symbols in global environment
    symbol_t * genv_rev;
    /*
     * You know all the symbols used in a python program,
     * so you can access to a value bound to a symbol in a O(1) time
     * by numbering each symbol.
     *
     * ex)
     * symbol: foo, and bar appears in a program.
     * Say 0:foo, 1:bar,
     * python code:'foo = bar + 1' is executed like this:
     * C++ code:'genv[0] = genv[1] + 1;'
     *
     * To remember what the index '0' means,
     * correspondences between indexes and symbols are stored in
     * genv_rev
     * ex)
     * genv_rev[0] == 'foo'; genv_rev[1] == 'bar';
     */

    // array of minipython byte code


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
    virtual ~Tivi();

    void bootstrap();
    
    void load_file(const string & filename);
    void run(int entry_point, ConsStack<Stack_trace_entry*> * bt);
    void presetup();
    
    void runtime_error(const string & msg, ConsStack<Stack_trace_entry*> * bt, int program_counter);

    void disasm();
    tivi_insn * get_insns();
    static Tivi * fork(Tivi * parent);

    static void * thread_nfun_dispatch(void * a);
    static void * thread_vfun_dispatch(void * a);
};

#endif

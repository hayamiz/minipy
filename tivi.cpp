#include "tivi.hpp"

#define TIVI_GOTO(addr) (program_counter = (addr))

#define TIVI_FETCH() (inst = insns[PC].type)
#define TIVI_FETCH_OPERAND() (py_val_t)(insns[PC].operand)
#define TIVI_FETCH_SRCPOS() (*insns[PC].p)

#define TIVI_LREF(offset) (this->machine_stack[FP + (offset) + 2])
#define TIVI_LSET(offset, value) TIVI_LREF(offset) = (value)
#define TIVI_STACK(offset_from_top) (this->machine_stack[SP - 1 - (offset_from_top)])

#define TIVI_PUSH() (this->machine_stack[SP++] = VAL)
#define TIVI_POP() (VAL = this->machine_stack[--SP])
#define TIVI_PUSHI(arg) (this->machine_stack[SP++] = (py_val_t)arg)

#define TIVI_EXIT() goto loop_exit;

#define TIVI_SET_ARGS(var, arity)                                     \
    (var)[arity] = NULL;                                              \
    for (int i = 0;i < arity;i++){                                    \
        (var)[i] = (this->machine_stack[ARGP + i]);                   \
    }


Tivi::Tivi(){
    this->machine_stack = new py_val_t[MAX_STACK_SIZE];
    
    SP = 0;
    TIVI_PUSHI(NULL); // frame pointer
    TIVI_PUSHI(NULL); // debug info
    FP = 0;
    VAL = NULL;
    PC = 0;
    ENV = new VmEnv();
    ENV->up = NULL;
    ENV->argp = 2;

#include "bootstrap.inc"
}

void Tivi::load_file(const string & filename){
    Parser parser(filename);
    vector<Stm*> stms = parser.file_input();

    this->trans.compile_file(stms, this->vmasm, this->genv);
}

void Tivi::runtime_error(const string & msg, stack<Stack_trace_entry> & bt, int program_counter){
    SrcPos p = *this->vmasm.insns[PC].p;
    cerr << "Tivi runtime error (lifetime: " << lifetime << ") :::" << endl
         << "  " << msg << " at " << p.filename << ":" << p.line_no << endl
         << "----" << endl;

    if (!bt.empty()){
        cerr << "* Backtrace *" << endl;
        while(!bt.empty()){
            cerr << "-- " << bt.top().name
                 << " at " << bt.top().call_site.filename
                 << ":" << bt.top().call_site.line_no << endl;
            bt.pop();
        }
    }
    
    exit(1);
}

void Tivi::presetup(tivi_insn ** insns, symbol_t ** genv_rev){
    uint size = this->vmasm.insns.size();

    *insns = new tivi_insn[size];
    *genv_rev = new symbol_t[this->genv.size()];

    map<symbol_t, int> sym_to_index;

    int gvar_index = 0;
    for (uint i = 0; i < size; i++){
        vm_inst & insn = this->vmasm.insns[i];
        (*insns)[i].p = (SrcPos *)insn.p;
        switch ((*insns)[i].type = insn.type){
        case VM_GREF:
        case VM_GREF_PUSH:
        case VM_GSET:
        case VM_GINC:
        case VM_GREF_CALL:
            if (sym_to_index.find((symbol_t) insn.operand) == sym_to_index.end()){
                (*insns)[i].operand = (void*) gvar_index;
                sym_to_index[(symbol_t) insn.operand] = gvar_index;
                (*genv_rev)[gvar_index] = (symbol_t)insn.operand;
                gvar_index++;
            } else {
                (*insns)[i].operand = (void *)sym_to_index[(symbol_t) insn.operand];
            }
            break;
        default:
            (*insns)[i].operand = insn.operand;
            break;
        }
    }
}



void Tivi::run(int entry_point){
    srand((unsigned) time(NULL));

    register int pc_limit = vmasm.insns.size();
    stack<Stack_trace_entry> bt;
    // py_val_t op;
    symbol_t sym;
    int num;
    py_val_t a1, a2;
    py_val_t a[4];
    register py_val_t f = NULL;
    register int numarg;

    // reverse dictionary of symbols in global environment
    symbol_t * genv_rev = NULL;
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
    tivi_insn * insns = NULL;

    // construct genv_rev, and modify byte code to use genv_rev
    presetup(&insns, &genv_rev);
    py_val_t globals[this->genv.size()];
    for (uint i = 0; i < this->genv.size();i++){
        globals[i] = this->genv.lookup_sym(genv_rev[i]);
    }


// #undef __GNUC__
#ifdef __GNUC__

#define SWITCH(var) goto *gototable[PC];
#define CASE(var) var##_CASE:
#define BREAK

#define TIVI_NEXTI() (PC++); TIVI_NEXT()
#define TIVI_NEXT() goto main_loop

#include "dispatchtable.inc"
    void * gototable[pc_limit];
    for(int i = 0; i < pc_limit; i++){
        gototable[i] = dispatchtable[insns[i].type];
    }
#else

#define SWITCH(var) switch(var)
#define CASE(var) case (var):
#define BREAK break
#define TIVI_NEXTI() (PC++); TIVI_NEXT()
#define TIVI_NEXT() goto main_loop
    
#endif

        
#ifndef RELEASE_BUILD
    lifetime = 0;
#endif

    TIVI_GOTO(entry_point);

main_loop:
    if (PC >= pc_limit) {
        return;
    }

#ifndef RELEASE_BUILD
    lifetime++;
#endif

    // cerr << "lifetime: " << lifetime++ << endl;

    SWITCH(TIVI_FETCH()){
    CASE(VM_NOP)
    {
        // no operand
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_END)
    {
        TIVI_EXIT();
    }
    BREAK;
    CASE(VM_IMMVAL)
    {
        // one operand
        VAL = TIVI_FETCH_OPERAND();
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_IMMVAL_NUM)
    {
        // one operand
        VAL = TIVI_FETCH_OPERAND();
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_IMMVAL_CHAR)
    {
        // one operand
        VAL = TIVI_FETCH_OPERAND();
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_PUSH)
    {
        // no operand
        TIVI_PUSH();

        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_POP)
    {
        // no operand
        TIVI_POP();
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_PUSHI)
    {
        // one operand
        TIVI_PUSHI(TIVI_FETCH_OPERAND());
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_PUSHI_NUM)
    {
        // one operand
        TIVI_PUSHI(TIVI_FETCH_OPERAND());
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_PUSHI_CHAR)
    {
        // one operand
        TIVI_PUSHI(TIVI_FETCH_OPERAND());
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_LREF)
    {
        // one operand
        num = (int)TIVI_FETCH_OPERAND();
        VAL = TIVI_LREF(num);
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_LREF_PUSH)
    {
        // one operand
        num = (int)TIVI_FETCH_OPERAND();
        TIVI_PUSHI(TIVI_LREF(num));
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_LREF_LOOK)
    {
        runtime_error("VM_LREF_LOOK is obsolete", bt, PC);
        // one operand
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_LSET)
    {
        // one operand
        num = (int)TIVI_FETCH_OPERAND();
        TIVI_LSET(num, VAL);
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_LINC)
    {
        // one operand
        num = (int)TIVI_FETCH_OPERAND();
        TIVI_LSET(num, (py_val_t)(((int)TIVI_LREF(num)) + 2));
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_LDEFUN)
    {
        // one operand
        TIVI_PUSHI(TIVI_FETCH_OPERAND());
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_GREF)
    {
        // one operand
        VAL = globals[(int)TIVI_FETCH_OPERAND()];
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_GREF_PUSH)
    {
        // one operand
        TIVI_PUSHI(globals[(int)TIVI_FETCH_OPERAND()]);
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_GSET)
    {
        // one operand
        globals[(int)TIVI_FETCH_OPERAND()] = VAL;
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_GINC)
    {
        // one operand
        globals[(int)TIVI_FETCH_OPERAND()] 
            = (py_val_t)((int)globals[(uint)TIVI_FETCH_OPERAND()] + 2);
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_GDEFUN)
    {
        // one operand
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_ADD2)
    CASE(VM_ADD2_NUM)
    CASE(VM_ADD2_STR)
    {
        // no operand
        a[2] = NULL;

        a[1] = VAL;
        a[0] = TIVI_POP();

        VAL = native::add(bt, TIVI_FETCH_SRCPOS(), a);
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_SUB2)
    CASE(VM_SUB2_NUM)
    {
        // no operand
        a[2] = NULL;

        a[1] = VAL;
        a[0] = TIVI_POP();

        VAL = native::sub(bt, TIVI_FETCH_SRCPOS(), a);
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_MUL2)
    CASE(VM_MUL2_NUM)
    {
        // no operand
        a[2] = NULL;

        a[1] = VAL;
        a[0] = TIVI_POP();

        VAL = native::mul(bt, TIVI_FETCH_SRCPOS(), a);
        
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_DIV2)
    CASE(VM_DIV2_NUM)
    {
        // no operand
        a[2] = NULL;

        a[1] = VAL;
        a[0] = TIVI_POP();

        VAL = native::div(bt, TIVI_FETCH_SRCPOS(), a);
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_AND2)
    {
        // no operand
        
        a2 = VAL;
        a1 = TIVI_POP();

        if(Py_val::is_false(a1)){
            VAL = Py_val::mk_int(0);
        } else {
            if(Py_val::is_false(a2)){
                VAL = Py_val::mk_int(0);
            } else {
                VAL = Py_val::mk_int(1);
            }
        }
        
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_OR2)
    {
        // no operand
        a2 = VAL;
        a1 = TIVI_POP();

        if(Py_val::is_false(a1)){
            if(Py_val::is_false(a2)){
                VAL = Py_val::mk_int(0);
            } else {
                VAL = Py_val::mk_int(1);
            }
        } else {
            VAL = Py_val::mk_int(1);
        }
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_NOT)
    {
        // no operand
        if (Py_val::is_false(VAL)) {
            VAL = Py_val::mk_int(1);
        } else {
            VAL = Py_val::mk_int(0);
        }
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_BITAND2)
    {
        // no operand
        a2 = VAL;
        a1 = TIVI_POP();

        if (!Py_val::is_int(a1) || !Py_val::is_int(a2)){
            runtime_error("type error : & (and) requires integer", bt, PC);
        }

        VAL = Py_val::mk_int(getint(a1)
                             & getint(a2));
        
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_BITOR2)
    {
        // no operand
        a2 = VAL;
        a1 = TIVI_POP();

        if (!Py_val::is_int(a1) || !Py_val::is_int(a2)){
            runtime_error("type error : << (shift) requires integer", bt, PC);
        }

        VAL = Py_val::mk_int(getint(a1)
                             | getint(a2));
        
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_BITXOR2)
    {
        // no operand
        a2 = VAL;
        a1 = TIVI_POP();

        if (!Py_val::is_int(a1) || !Py_val::is_int(a2)){
            runtime_error("type error : << (shift) requires integer", bt, PC);
        }

        VAL = Py_val::mk_int(getint(a1)
                             ^ getint(a2));
        
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_BITINV)
    {
        // no operand
        a1 = VAL;

        if (!Py_val::is_int(a1)){
            runtime_error("type error : ~ (bit inv) requires integer", bt, PC);
        }

        VAL = Py_val::mk_int(- getint(a1) - 1);
        
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_RSHIFT2)
    {
        // no operand
        a2 = VAL;
        a1 = TIVI_POP();

        if (!Py_val::is_int(a1) || !Py_val::is_int(a2)){
            runtime_error("type error : << (shift) requires integer", bt, PC);
        }

        VAL = Py_val::mk_int(getint(a1) >> getint(a2));
        
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_LSHIFT2)
    {
        // no operand
        a2 = VAL;
        a1 = TIVI_POP();

        if (!Py_val::is_int(a1) || !Py_val::is_int(a2)){
            runtime_error("type error : << (shift) requires integer", bt, PC);
        }

        VAL = Py_val::mk_int(getint(a1) << getint(a2));
        
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_EQL2)
    {
        // no operand
        a2 = VAL;
        a1 = TIVI_POP();

        if (Py_val::compare(a1, a2) == 0){
            VAL = mkint(1);
        } else {
            VAL = mkint(0);
        }
        
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_NEQL2)
    {
        // no operand
        a2 = VAL;
        a1 = TIVI_POP();

        if (Py_val::compare(a1, a2) != 0){
            VAL = mkint(1);
        } else {
            VAL = mkint(0);
        }

        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_NUMLT2)
    {
        // no operand
        a2 = VAL;
        a1 = TIVI_POP();

        if (Py_val::compare(a1, a2) < 0){
            VAL = mkint(1);
        } else {
            VAL = mkint(0);
        }

        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_NUMLEQ2)
    {
        // no operand
        a2 = VAL;
        a1 = TIVI_POP();

        if (Py_val::compare(a1, a2) <= 0){
            VAL = mkint(1);
        } else {
            VAL = mkint(0);
        }
        
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_NUMGT2)
    {
        // no operand
        a2 = VAL;
        a1 = TIVI_POP();

        if (Py_val::compare(a1, a2) > 0){
            VAL = mkint(1);
        } else {
            VAL = mkint(0);
        }
        
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_NUMGEQ2)
    {
        // no operand
        a2 = VAL;
        a1 = TIVI_POP();

        if (Py_val::compare(a1, a2) >= 0){
            VAL = mkint(1);
        } else {
            VAL = mkint(0);
        }
        
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_IS2)
    {
        // no operand
        a2 = VAL;
        a1 = TIVI_POP();

        a[2] = NULL;

        a[0] = a1;
        a[1] = a2;
        
        if (Py_val::is_int(a1) && Py_val::is_int(a2) && a1 == a2){
            VAL = Py_val::mk_int(1);
        } else if (Py_val::is_float(a1) && Py_val::is_float(a2)
                   && a1->u.f == a2->u.f) {
            VAL = Py_val::mk_int(1);
        } else if (Py_val::is_string(a1) && Py_val::is_string(a2)) {
            VAL = native::eq_string(bt, TIVI_FETCH_SRCPOS(), a);
        } else {
            VAL = Py_val::mk_int(a1 == a2);
        }
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_IN2)
    {
        // no operand
        a[2] = NULL;

        a[1] = VAL;
        a[0] = TIVI_POP();

        VAL = native::contains(bt, TIVI_FETCH_SRCPOS(), a);
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_GOTO)
    {
        // one operand
        num = (int)TIVI_FETCH_OPERAND();
        TIVI_GOTO(num);
        TIVI_NEXT();
    }
    BREAK;
    CASE(VM_GOTOIF)
    {
        // one operand
        cerr << "not implemented @ " << __FILE__ << ":" << __LINE__ << endl; exit(1);
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_GOTOIFNOT)
    {
        // one operand
        if(Py_val::is_false(VAL)){
            TIVI_GOTO((int)TIVI_FETCH_OPERAND());
        } else {
            TIVI_NEXTI();
        }
        TIVI_NEXT();
    }
    BREAK;
    CASE(VM_PUSH_ENV)
    {
        TIVI_PUSHI(FP); // static link
        TIVI_PUSHI(NULL); // debug info will be here.
        
        VmEnv * newenv = new VmEnv();
        newenv->up = ENV;
        newenv->argp = SP;
        ENV = newenv;
        
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_SELF_CALL)
    CASE(VM_VREF_CALL)
    CASE(VM_GREF_CALL)
    CASE(VM_LREF_CALL)
    {
        // one operand
        numarg = SP - ARGP;

        if (INST == VM_GREF_CALL){
            if ((f = globals[(int)TIVI_FETCH_OPERAND()]) == py_val_not_found){
                runtime_error("no such function ", bt, PC);
            }
            bt.push(Stack_trace_entry(*(symbol_t)vmasm.insns[PC].operand
                                      , TIVI_FETCH_SRCPOS()));
        } else if (INST == VM_VREF_CALL){
            f = VAL;
            if (f->type == py_type_nfun){
                bt.push(Stack_trace_entry(*f->u.n->name, TIVI_FETCH_SRCPOS()));
            } else if (f->type == py_type_vm_ifun){
                bt.push(Stack_trace_entry(*f->u.vm_i->name, TIVI_FETCH_SRCPOS()));
            } else {
                runtime_error("type error : no function object was called.", bt, PC);
            }
        } else if (INST == VM_LREF_CALL){
            f = TIVI_LREF((int)TIVI_FETCH_OPERAND());
            if (f->type == py_type_nfun){
                bt.push(Stack_trace_entry(*f->u.n->name, TIVI_FETCH_SRCPOS()));
            } else if (f->type == py_type_vm_ifun){
                bt.push(Stack_trace_entry(*f->u.vm_i->name, TIVI_FETCH_SRCPOS()));
            } else {
                runtime_error("type error : no function object was called.", bt, PC);
            }
        } else if (INST == VM_SELF_CALL){
            f = TIVI_FETCH_OPERAND();
            bt.push(Stack_trace_entry(*f->u.vm_i->name, TIVI_FETCH_SRCPOS()));
        }

        switch(f->type){
        case py_type_vm_ifun:
        {
            if (f->u.vm_i->arity >= 0 && numarg != f->u.vm_i->arity){
                stringstream ss;
                ss << f->u.vm_i->name << " requires " << f->u.vm_i->arity
                   << ", but got " << numarg;
                runtime_error("wrong number of argument : " + ss.str(), bt, PC);
            }

            FP = ARGP - 2;
            this->return_stack.push(PC+1);
            PC = f->u.vm_i->addr;
            SP += f->u.vm_i->locals; // align SP for local vars
            bt.pop();
        }
        break;
        case py_type_nfun:
        {
            if (f->u.n->arity >= 0 && numarg != f->u.n->arity){
                stringstream ss;
                ss << f->u.vm_i->name << " requires " << f->u.vm_i->arity
                   << ", but got " << numarg;
                runtime_error("wrong number of argument : " + ss.str(), bt, PC);
            }

            py_val_t args[numarg + 1];
            TIVI_SET_ARGS(args, numarg);
            // if (lifetime > 3040000) cerr << lifetime << " " ;
            VAL = f->u.n->f(bt, TIVI_FETCH_SRCPOS(), args);

            SP = ARGP - 2;
            ENV = ENV->up;

            bt.pop();
            TIVI_NEXTI();
        }
        break;
        default:
            runtime_error("type error : " + *sym + " is not a function.", bt, PC);
        }
        TIVI_NEXT();
    }
    BREAK;
    CASE(VM_RET)
    {
        // no operand
        if (this->return_stack.size() == 0){
            runtime_error("no place to return", bt, PC);
        }
        ENV = ENV->up;
        SP = FP;
        FP = (int)machine_stack[FP];

        PC = this->return_stack.top();
        this->return_stack.pop();
        TIVI_NEXT();
    }
    BREAK;
    CASE(VM_MKLIST)
    {
        // one operand
        num = (int)TIVI_FETCH_OPERAND();

        vector<py_val_t> * tmp = new vector<py_val_t>();
        for(int i = 0; i < num;i++){
            py_val_t foo = TIVI_STACK(num - i - 1);
            tmp->push_back(foo);
        }
        VAL = Py_val::mk_list_byptr(tmp);

        SP -= num;
        
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_MKTUPLE)
    {
        // one operand
        num = (int)TIVI_FETCH_OPERAND();

        Py_tuple * tmp = new Py_tuple(num);
        for(int i = 0; i < num;i++){
            py_val_t foo = TIVI_STACK(num - i - 1);
            tmp->push_back(foo);
        }
        VAL = Py_val::mk_newtuple(tmp);
        
        SP -= num;

        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_MKDICT)
    {
        // one operand

        a1 = Py_val::mk_newdict();

        num = (int)TIVI_FETCH_OPERAND();
        for(int i = num;i > 0;i--){
            Py_dict_entry* entry
                = new Py_dict_entry(TIVI_STACK(2 * i - 1), TIVI_STACK(2 * i - 2));
            getnewdict(a1)->set_entry(entry, bt, TIVI_FETCH_SRCPOS());
        }

        SP -= num * 2;

        VAL = a1;

        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_LEN)
    {
        a[1] = NULL;
        a[0] = VAL;
        VAL = native::len(bt, TIVI_FETCH_SRCPOS(), a);
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_GETITEM)
    {
        a[2] = NULL;

        a[1] = VAL;
        a[0] = TIVI_POP();

        VAL = native::getitem(bt, TIVI_FETCH_SRCPOS(), a);
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_SETITEM)
    {
        a[3] = NULL;
        a[0] = TIVI_STACK(1);
        a[1] = TIVI_STACK(0);
        a[2] = VAL;

        SP -= 2;

        VAL = native::setitem(bt, TIVI_FETCH_SRCPOS(), a);
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_DELITEM)
    {
        a[2] = NULL;
        a[1] = VAL;
        a[0] = TIVI_POP();

        VAL = native::delitem(bt, TIVI_FETCH_SRCPOS(), a);
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_PRINT)
    {
        a[1] = NULL;
        a[0] = VAL;

        VAL = native::print_(bt, TIVI_FETCH_SRCPOS(), a);
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_PRINTSTR)
    {
        a[0] = VAL;
        a[1] = NULL;
        
        VAL = native::print_string(bt, TIVI_FETCH_SRCPOS(), a);
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_LREF_ADD2)
    {
        a[2] = NULL;

        a[1] = TIVI_LREF((int)TIVI_FETCH_OPERAND());
        a[0] = TIVI_POP();

        VAL = native::add(bt, TIVI_FETCH_SRCPOS(), a);
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_LREF_SUB2)
    {
        a[2] = NULL;

        a[1] = TIVI_LREF((int)TIVI_FETCH_OPERAND());
        a[0] = TIVI_POP();

        VAL = native::sub(bt, TIVI_FETCH_SRCPOS(), a);
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_LREF_MUL2)
    {
	a[2] = NULL;
	a[1] = TIVI_LREF((int)TIVI_FETCH_OPERAND());
	a[0] = TIVI_POP();

	VAL = native::mul(bt, TIVI_FETCH_SRCPOS(), a);
        TIVI_NEXTI();
    }
    BREAK;
    CASE(VM_LREF_DIV2)
    {
        a[2] = NULL;
	a[1] = TIVI_LREF((int)TIVI_FETCH_OPERAND());
	a[0] = TIVI_POP();

	VAL = native::div(bt, TIVI_FETCH_SRCPOS(), a);
        TIVI_NEXTI();
    }
    BREAK;
    }

    goto main_loop;

loop_exit:

    for (uint i = 0; i < this->genv.size();i++){
        this->genv.set_sym(genv_rev[i], globals[i]);
    }
    delete[](genv_rev);
    delete[](insns);
}

void Tivi::disasm(){
    this->vmasm.print(this->genv);
}

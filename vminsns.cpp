#include "vminsns.hpp"

vm_assembler::vm_assembler(){
    this->label_index = 0;
}

vm_assembler & vm_assembler::add(vm_insn_type type, const SrcPos & p){
    vm_inst inst = {type, NULL, &p};

    this->insns.push_back(inst);
    this->label_index++;
    return *this;
}

vm_assembler & vm_assembler::add(vm_insn_type type, vm_operand operand, const SrcPos & p){

    vm_inst inst = {type, NULL, &p};

    switch(operand.kind){
    case vm_operand_pyval:
        inst.operand = operand.u.pyval;
        break;
    case vm_operand_int:
        inst.operand = (void*) operand.u.i;
        break;
    case vm_operand_name:
        inst.operand = (void*) operand.u.name;
        break;
    }
    this->insns.push_back(inst);

    this->label_index++;
    
    return *this;
}

void vm_assembler::fundef(const symbol_t name, int arity, int locals, Env & genv, const SrcPos & pos){
    genv.set_sym(name, Py_val::mk_vm_ifun(name, arity, locals, this->label_index, pos));
}

vm_assembler & vm_assembler::label(const string & str){
    this->labels[Symbol::get(str)] = this->label_index;
    return *this;
}

vm_inst * vm_assembler::assemble(){
    vm_inst * ret;
    ret = new vm_inst[this->insns.size()];

    // do it later
   
    return ret;
}


void vm_assembler::print(Env & genv){
    // genv.debug_print_all();
#include "vminsns_print.inc"
}

string vm_assembler::operand_str(vm_insn_type type, py_val_t operand, Env & genv){
    stringstream ss;
    ConsStack<Stack_trace_entry*> * bt; SrcPos p; // dummy

    switch(type){
    case VM_GREF_CALL:
    {
        py_val_t f = genv.lookup_sym((symbol_t)operand);
        ss << *((symbol_t)operand);
        if (f == py_val_not_found){
            ss << ": not found";
        } else if (f->type == py_type_vm_ifun){
            ss << ": start at " << f->u.vm_i->addr;
        } else {
            ss << ": native function";
        }
    }
    break;
    case VM_LDEFUN:
    {
        py_val_t f = operand;
        ss << f->u.vm_i->name << ": start at "
           << f->u.vm_i->addr;

    }
    break;
    case VM_GDEFUN:
    case VM_GREF:
    case VM_GSET:
    case VM_GINC:
    case VM_GREF_PUSH:
        ss << *((symbol_t)operand);
        break;
    case VM_GOTO:
    case VM_GOTOIF:
    case VM_GOTOIFNOT:
    case VM_LSET:
    case VM_LINC:
    case VM_LREF:
    case VM_LREF_ADD2:
    case VM_LREF_SUB2:
    case VM_LREF_MUL2:
    case VM_LREF_DIV2:
    case VM_LREF_CALL:
    case VM_LREF_PUSH:
    case VM_MKLIST:
    case VM_MKTUPLE:
    case VM_MKDICT:
        ss << (int)operand;
        break;
    default:
    {
        Py_val::to_str(ss, operand, bt, p);
    }
    }

    return ss.str();
}

vm_operand vm_operand::name(const symbol_t name){
    vm_operand ret;
    ret.kind = vm_operand_name;
    ret.u.name = name;

    return ret;
}

vm_operand vm_operand::pyval(py_val_t pyval){
    vm_operand ret;
    ret.kind = vm_operand_pyval;
    ret.u.pyval = pyval;

    return ret;
}


vm_operand vm_operand::integer(int i){
   vm_operand ret;
    ret.kind = vm_operand_int;
    ret.u.i = i;
    
    return ret;
}

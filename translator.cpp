#include "translator.hpp"

Translator::Translator(){
    
}

void Translator::compile_error(const string & msg, const SrcPos & pos){
    cerr << "compile error:::" << endl
         << "  " << msg << " at " << pos.filename << ":" << pos.line_no << endl;
    exit(1);
}

bool Translator::numeric_expr(const Expr & expr){
    bool ret;

    switch(expr.kind){
    case expr_kind_literal_int:
        ret = true;
        break;
    case expr_kind_literal_float:
        ret = true;
        break;
    case expr_kind_paren:
        ret = this->numeric_expr(*expr.u.paren);
        break;
    case expr_kind_operator:
        if (expr.u.op->fix == fix_kind_prefix){
            switch(expr.u.op->kind){
            case TOK_PLUS:
            case TOK_MINUS:
            case TOK_TILDE:
                ret = numeric_expr(*expr.u.op->args[0]);
                break;
            default:
                ret = false;
            }
        } else if(expr.u.op->fix == fix_kind_infix){
            switch(expr.u.op->kind){
            case TOK_PLUS:
            case TOK_MINUS:
            case TOK_MUL:
            case TOK_DIV:
            case TOK_LSHIFT:
            case TOK_RSHIFT:
            case TOK_XOR:
            case TOK_AMP:
            case TOK_BAR:
                ret = numeric_expr(*expr.u.op->args[0])
                    && numeric_expr(*expr.u.op->args[1]);
                break;
            default:
                ret = false;
            }
        } else {
            return false;
        }
        break;
    default:
        ret = false;
    }
    
    return ret;
}

void Translator::insns_val_infix_operator(const Expr & expr, vm_assembler & vmasm, LocalEnv * lenv){
    // infix operators
    // TOK_PLUS, TOK_MINUS, TOK_MUL, TOK_DIV
    // TOK_EQ_EQ, TOK_NEQ, TOK_GT, TOK_GEQ, TOK_LT, TOK_LEQ
    // TOK_LSHIFT, TOK_RSHIFT, TOK_XOR, TOK_AMP, TOK_BAR
    // TOK_KW_OR, TOK_KW_AND,  TOK_KW_IS, TOK_KW_IN
    // TOK_KW_IS_NOT, TOK_KW_NOT_IN
    switch(expr.u.op->kind){
    case TOK_PLUS:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        if (expr.u.op->args[1]->kind == expr_kind_var
            && !lenv->is_global(expr.u.op->args[1]->u.var)
            && !lenv->is_toplevel
            && lenv->exists(expr.u.op->args[1]->u.var)) {
            vmasm.LREF_ADD2(vm_operand::integer(
                                lenv->look_var(expr.u.op->args[1]->u.var))
                            ,expr.pos);
        } else {
            this->insns_val(*expr.u.op->args[1], vmasm, lenv);
            vmasm.ADD2(expr.pos);
        }
//         if (this->numeric_expr(expr)){
//             vmasm.ADD2_NUM(expr.pos);
//         } else {
//             vmasm.ADD2(expr.pos);
//         }
        break;
    case TOK_MINUS:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        if (expr.u.op->args[1]->kind == expr_kind_var
            && !lenv->is_global(expr.u.op->args[1]->u.var)
            && !lenv->is_toplevel
            && lenv->exists(expr.u.op->args[1]->u.var)) {
            vmasm.LREF_SUB2(vm_operand::integer(
                                lenv->look_var(expr.u.op->args[1]->u.var))
                            ,expr.pos);
        } else {
            this->insns_val(*expr.u.op->args[1], vmasm, lenv);
            vmasm.SUB2(expr.pos);
        }
//         if (this->numeric_expr(expr)){
//             vmasm.SUB2_NUM(expr.pos);
//         } else {
//             vmasm.SUB2(expr.pos);
//         }
        
        break;
    case TOK_MUL:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        if (expr.u.op->args[1]->kind == expr_kind_var
            && !lenv->is_global(expr.u.op->args[1]->u.var)
            && !lenv->is_toplevel
            && lenv->exists(expr.u.op->args[1]->u.var)) {
	    // stacktop(0) * LREF(n)
            vmasm.LREF_MUL2(vm_operand::integer(
                                lenv->look_var(expr.u.op->args[1]->u.var))
                            ,expr.pos);
        } else {
            // stacktop(0) * VAL
            this->insns_val(*expr.u.op->args[1], vmasm, lenv);
            vmasm.MUL2(expr.pos);
        }
//         if (this->numeric_expr(expr)){
//             vmasm.MUL2_NUM(expr.pos);
//         } else {
//             vmasm.MUL2(expr.pos);
//         }
        break;
    case TOK_DIV:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        if (expr.u.op->args[1]->kind == expr_kind_var
            && !lenv->is_global(expr.u.op->args[1]->u.var)
            && !lenv->is_toplevel
            && lenv->exists(expr.u.op->args[1]->u.var)) {
            vmasm.LREF_DIV2(vm_operand::integer(
                                lenv->look_var(expr.u.op->args[1]->u.var))
                            ,expr.pos);
        } else {
            this->insns_val(*expr.u.op->args[1], vmasm, lenv);
            vmasm.DIV2(expr.pos);
        }
//         if (this->numeric_expr(expr)){
//             vmasm.DIV2_NUM(expr.pos);
//         } else {
//             vmasm.DIV2(expr.pos);
//         }
        
        break;
    case TOK_MOD:
        vmasm.PUSH_ENV(expr.pos);
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        this->insns_push(*expr.u.op->args[1], vmasm, lenv);
        vmasm.GREF_CALL(vm_operand::name(Symbol::get("mod")), expr.pos);
        break;
    case TOK_EQ_EQ:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        this->insns_val(*expr.u.op->args[1], vmasm, lenv);
        vmasm.EQL2(expr.pos);
        break;
    case TOK_NEQ:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        this->insns_val(*expr.u.op->args[1], vmasm, lenv);
        vmasm.NEQL2(expr.pos);
        break;
    case TOK_GT:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        this->insns_val(*expr.u.op->args[1], vmasm, lenv);
        vmasm.NUMGT2(expr.pos);
        break;
    case TOK_GEQ:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        this->insns_val(*expr.u.op->args[1], vmasm, lenv);
        vmasm.NUMGEQ2(expr.pos);
        break;
    case TOK_LT:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        this->insns_val(*expr.u.op->args[1], vmasm, lenv);
        vmasm.NUMLT2(expr.pos);
        break;
    case TOK_LEQ:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        this->insns_val(*expr.u.op->args[1], vmasm, lenv);
        vmasm.NUMLEQ2(expr.pos);
        break;
    case TOK_LSHIFT:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        this->insns_val(*expr.u.op->args[1], vmasm, lenv);
        vmasm.LSHIFT2(expr.pos);
        break;
    case TOK_RSHIFT:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        this->insns_val(*expr.u.op->args[1], vmasm, lenv);
        vmasm.RSHIFT2(expr.pos);
        break;
    case TOK_XOR:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        this->insns_val(*expr.u.op->args[1], vmasm, lenv);
        vmasm.BITXOR2(expr.pos);
        break;
    case TOK_AMP:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        this->insns_val(*expr.u.op->args[1], vmasm, lenv);
        vmasm.BITAND2(expr.pos);
        break;
    case TOK_BAR:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        this->insns_val(*expr.u.op->args[1], vmasm, lenv);
        vmasm.BITOR2(expr.pos);
        break;
    case TOK_KW_OR:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        this->insns_val(*expr.u.op->args[1], vmasm, lenv);
        vmasm.OR2(expr.pos);
        break;
    case TOK_KW_AND:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        this->insns_val(*expr.u.op->args[1], vmasm, lenv);
        vmasm.AND2(expr.pos);
        break;
    case TOK_KW_IS:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        this->insns_val(*expr.u.op->args[1], vmasm, lenv);
        vmasm.IS2(expr.pos);
        break;
    case TOK_KW_IN:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        this->insns_val(*expr.u.op->args[1], vmasm, lenv);
        vmasm.IN2(expr.pos);
        break;
    case TOK_KW_IS_NOT:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        this->insns_val(*expr.u.op->args[1], vmasm, lenv);
        vmasm.IS2(expr.pos);
        vmasm.NOT(expr.pos);
        break;
    case TOK_KW_NOT_IN:
        this->insns_push(*expr.u.op->args[0], vmasm, lenv);
        this->insns_val(*expr.u.op->args[1], vmasm, lenv);
        vmasm.IN2(expr.pos);
        vmasm.NOT(expr.pos);
        break;
    default:
        perror("Invalid infix operator");
        exit(1);
    }
}

void Translator::scan_defun_local_var(const vector<Stm*> & stms, LocalEnv * lenv){
    for(int i = 0;i < (int)stms.size();i++){
        const Stm & stm = *stms[i];
        switch(stm.kind){
        case stm_kind_expression:
        case stm_kind_del:
        case stm_kind_pass:
        case stm_kind_return:
        case stm_kind_break:
        case stm_kind_print:
        case stm_kind_fundef:
        case stm_kind_continue:
            break;
        case stm_kind_if:
            for(int j = 0;j < (int)stm.u.i->size();j++){
                this->scan_defun_local_var((*stm.u.i)[j]->s, lenv);
            }
            break;
        case stm_kind_while:
            this->scan_defun_local_var(stm.u.w->b, lenv);
            break;
        case stm_kind_for:
            if (lenv->is_toplevel){
            }else{
                if (!lenv->is_global(stm.u.f->x)){ // if local env
                    lenv->add_var(stm.u.f->x); // allocate loop var
                }
                lenv->loopvar_queue.push(lenv->current_offset());
                if (stm.u.f->e->kind == expr_kind_call
                    && stm.u.f->e->u.call->f->kind == expr_kind_var
                    && stm.u.f->e->u.call->f->u.var->compare("range") == 0){
                    // range() for-loop
                    lenv->add_var(); // loop var limit
                    lenv->add_var(); // loop var step
                } else {
                    lenv->add_var(); // loop var index
                    lenv->add_var(); // loop var index limit
                    lenv->add_var(); // loop sequence
                }
            }
            this->scan_defun_local_var(stm.u.f->b, lenv);
            break;
        case stm_kind_global:
            lenv->mark_global(stm.u.g);
            break;
        case stm_kind_assignment:
            if (lenv->is_toplevel) {
                break;
            }
            if (stm.u.a->target->kind == expr_kind_var){
                if (!lenv->is_global(stm.u.a->target->u.var)){
                    lenv->add_var(stm.u.a->target->u.var);
                }
            }
            break;
        }
    }
}

void Translator::insns_val(const Expr & expr, vm_assembler & vmasm, LocalEnv * lenv){

    vector<Expr*>::iterator itr;

    switch(expr.kind){
    case expr_kind_var:
        if (lenv->is_global(expr.u.var)){
            vmasm.GREF(vm_operand::name(expr.u.var), expr.pos);
        } else if (! lenv->is_toplevel && lenv->exists(expr.u.var)) {
            vmasm.LREF(vm_operand::integer(lenv->look_var(expr.u.var)), expr.pos);
        } else {
            vmasm.GREF(vm_operand::name(expr.u.var), expr.pos);
        }
        break;
    case expr_kind_none:
        vmasm.IMMVAL(vm_operand::pyval(Py_val::mk_none()), expr.pos);
        break;
    case expr_kind_literal_int:
        vmasm.IMMVAL_NUM(vm_operand::pyval(Py_val::mk_int(expr.u.lit_i)), expr.pos);
        break;
    case expr_kind_literal_float:
        vmasm.IMMVAL_NUM(vm_operand::pyval(Py_val::mk_float(expr.u.lit_f)), expr.pos);
        break;
    case expr_kind_literal_string:
        vmasm.IMMVAL(vm_operand::pyval(Py_val::mk_string(*expr.u.lit_s)), expr.pos);
        break;
    case expr_kind_display_list:
        for(itr = expr.u.disp->begin();itr != expr.u.disp->end()
                ;itr++){
            this->insns_push(**itr, vmasm, lenv);
        }
        vmasm.MKLIST(vm_operand::integer(expr.u.disp->size()), expr.pos);
        break;
    case expr_kind_display_tuple:
        for(itr = expr.u.disp->begin(); itr != expr.u.disp->end()
                ;itr++){
            this->insns_push(**itr, vmasm, lenv);
        }
        vmasm.MKTUPLE(vm_operand::integer(expr.u.disp->size()), expr.pos);
        break;
    case expr_kind_display_dict:
        for(itr = expr.u.disp->begin(); itr != expr.u.disp->end()
                ;itr++){
            this->insns_push(*(*itr)->u.op->args[0], vmasm, lenv); // push key
            this->insns_push(*(*itr)->u.op->args[1], vmasm, lenv); // push value
        }
        vmasm.MKDICT(vm_operand::integer(expr.u.disp->size()), expr.pos);
        break;
    case expr_kind_paren:
        this->insns_val(*expr.u.paren, vmasm, lenv);
        break;
    case expr_kind_operator:
        // operators
        // TOK_PLUS, TOK_MINUS, TOK_MUL, TOK_DIV
        // TOK_EQ_EQ, TOK_NEQ, TOK_GT, TOK_GEQ, TOK_LT, TOK_LEQ
        // TOK_TILDE, TOK_LSHIFT, TOK_RSHIFT, TOK_XOR, TOK_AMP, TOK_BAR
        // TOK_KW_OR, TOK_KW_AND, TOK_KW_NOT, TOK_KW_IS, TOK_KW_IN
        if (expr.u.op->fix == fix_kind_prefix){
            switch(expr.u.op->kind){
            case TOK_PLUS:
                this->insns_val(*expr.u.op->args[0], vmasm, lenv);
                break;
            case TOK_MINUS:
                this->insns_push(*expr.u.op->args[0], vmasm, lenv);
                vmasm.IMMVAL_NUM(vm_operand::integer(-1), expr.pos);
                vmasm.MUL2_NUM(expr.pos);
                break;
            case TOK_TILDE:
                this->insns_val(*expr.u.op->args[0], vmasm, lenv);
                vmasm.BITINV(expr.pos);
                break;
            case TOK_KW_NOT:
                this->insns_val(*expr.u.op->args[0], vmasm, lenv);
                vmasm.NOT(expr.pos);
                break;
            default:
                perror("Invalid prefix operator.");
                exit(1);
            }
        } else {
            this->insns_val_infix_operator(expr, vmasm, lenv);
        }
        break;
    case expr_kind_attref:
        // cases like 'foo.bar()' are treated in case expr_kind_call.
        perror("Attref must be used as function name.");
        exit(1);
        break;
    case expr_kind_subscript:
        this->insns_push(*(*expr.u.sub)[0], vmasm, lenv);
        this->insns_val(*(*expr.u.sub)[1], vmasm, lenv);
        vmasm.GETITEM(expr.pos);
        break;
    case expr_kind_call:
        // optimize some functions as instructions
        if (expr.u.call->f->kind == expr_kind_var){
            symbol_t func = expr.u.call->f->u.var;
            if(func == Symbol::get("print_string")){
                this->insns_val(*expr.u.call->args[0], vmasm, lenv);
                vmasm.PRINTSTR(expr.pos);
                break;
            } else if (func == Symbol::get("start_new_thread")){
                this->insns_push(*expr.u.call->args[0], vmasm, lenv);
                this->insns_val(*expr.u.call->args[1], vmasm, lenv);
                vmasm.THREAD_FORK(expr.pos);
                break;
            } else if (func == Symbol::get("thread_join")){
                this->insns_val(*expr.u.call->args[0], vmasm, lenv);
                vmasm.THREAD_JOIN(expr.pos);
            }
        }
        
        vmasm.PUSH_ENV(expr.pos); // make new env
        // set up args
        if (expr.u.call->f->kind == expr_kind_attref){
            this->insns_push(*expr.u.call->f->u.atr->a, vmasm, lenv);
        }
        for(itr = expr.u.call->args.begin(); itr != expr.u.call->args.end(); itr++){
            this->insns_push(**itr, vmasm, lenv);
        }

        if (! lenv->is_toplevel
            && expr.u.call->f->kind == expr_kind_var
            && lenv->name->compare(*expr.u.call->f->u.var) == 0){
            vmasm.SELF_CALL(vm_operand::pyval(lenv->self), expr.pos);
        } else {
            switch(expr.u.call->f->kind){
            case expr_kind_attref:
                vmasm.GREF_CALL(vm_operand::name(expr.u.call->f->u.atr->f), expr.pos);
                break;
            case expr_kind_var:
                if (! lenv->is_toplevel && lenv->exists(expr.u.call->f->u.var)){
                    vmasm.LREF_CALL(
                        vm_operand::integer(lenv->look_var(expr.u.call->f->u.var))
                        , expr.pos);
                } else {
                    vmasm.GREF_CALL(vm_operand::name(expr.u.call->f->u.var), expr.pos);
                }
                break;
            default:
                this->insns_val(*expr.u.call->f, vmasm, lenv);
                vmasm.VREF_CALL(expr.pos);
            }
        }
        break;
    case expr_kind_list:
        for(itr = expr.u.list->begin(); itr != expr.u.list->end()
                ;itr++){
            this->insns_push(**itr, vmasm, lenv);
        }
        vmasm.MKTUPLE(vm_operand::integer(expr.u.list->size()), expr.pos);
        break;
    }
}

void Translator::insns_push(const Expr & expr, vm_assembler & vmasm, LocalEnv * lenv){
    vector<Expr*>::iterator itr;

    switch(expr.kind){
    case expr_kind_var:
        if (! lenv->is_toplevel  && lenv->exists(expr.u.var)) {
            vmasm.LREF_PUSH(vm_operand::integer(lenv->look_var(expr.u.var)), expr.pos);
        } else {
            vmasm.GREF_PUSH(vm_operand::name(expr.u.var), expr.pos);
        }
        break;
    case expr_kind_none:
        vmasm.PUSHI(vm_operand::pyval(Py_val::mk_none()), expr.pos);
        break;
    case expr_kind_literal_int:
        vmasm.PUSHI_NUM(vm_operand::pyval(Py_val::mk_int(expr.u.lit_i)), expr.pos);
        break;
    case expr_kind_literal_float:
        vmasm.PUSHI_NUM(vm_operand::pyval(Py_val::mk_float(expr.u.lit_f)), expr.pos);
        break;
    case expr_kind_literal_string:
        vmasm.PUSHI(vm_operand::pyval(Py_val::mk_string(*expr.u.lit_s)), expr.pos);
        break;
    case expr_kind_display_list:
        for(itr = expr.u.disp->begin();itr != expr.u.disp->end()
                ;itr++){
            this->insns_push(**itr, vmasm, lenv);
        }
        vmasm.MKLIST(vm_operand::integer(expr.u.disp->size()), expr.pos);
        vmasm.PUSH(expr.pos);
        break;
    case expr_kind_paren:
        this->insns_push(*expr.u.paren, vmasm, lenv);
        break;
    case expr_kind_display_tuple:
    case expr_kind_display_dict:
    case expr_kind_operator:
    case expr_kind_attref:
    case expr_kind_subscript:
    case expr_kind_list:
    case expr_kind_call:
        this->insns_val(expr, vmasm, lenv);
        vmasm.PUSH(expr.pos);
        break;
    default:
        perror("Invalid expression");
    }
}

void Translator::insns_stms(const vector<Stm*> & stm, vm_assembler & vmasm, LocalEnv * lenv){
    for(int i = 0;i < (int)stm.size();i++){
        this->insns_stm(*stm[i], vmasm, lenv);
    }
}

void Translator::insns_loop_stms(const vector<Stm*> & stms, vm_assembler & vmasm, int start_addr, vector<int> & break_addrs, LocalEnv * lenv){
    for(int i = 0;i < (int)stms.size();i++){
        const Stm & stm = *stms[i];
        switch(stm.kind){
        case stm_kind_expression:
        case stm_kind_assignment:
        case stm_kind_del:
        case stm_kind_pass:
        case stm_kind_return:
        case stm_kind_print:
        case stm_kind_global:
        case stm_kind_while:
        case stm_kind_for:
        case stm_kind_fundef:
            this->insns_stm(stm, vmasm, lenv);
            break;
        case stm_kind_break:
            break_addrs.push_back(vmasm.insns.size());
            vmasm.GOTO(vm_operand::integer(-1), stm.pos);
            break;
        case stm_kind_continue:
            vmasm.GOTO(vm_operand::integer(start_addr), stm.pos);
            break;
        case stm_kind_if:
        {
            queue<int> goto_endif_queue;

            for(vector<StmIfBranch*>::iterator itr = stm.u.i->begin()
                    ; itr != stm.u.i->end(); itr++){
                if( (*itr)->elsep ){
                    this->insns_loop_stms((*itr)->s, vmasm, start_addr, break_addrs, lenv);
                } else {
                    this->insns_val(*(*itr)->e, vmasm, lenv);
                    int gotoinst = vmasm.insns.size();
                    vmasm.GOTOIFNOT(vm_operand::integer(-1), (*itr)->e->pos);
                    this->insns_loop_stms((*itr)->s, vmasm, start_addr, break_addrs, lenv);
                    if (itr != stm.u.i->end() - 1){
                        goto_endif_queue.push(vmasm.insns.size());
                        vmasm.GOTO(vm_operand::integer(-1), (*itr)->s.back()->pos);
                    }
                    vmasm.insns[gotoinst].operand = (void *)vmasm.insns.size();
                }
            }

            while(! goto_endif_queue.empty()){
                vmasm.insns[goto_endif_queue.front()].operand = (void *)vmasm.insns.size();
                goto_endif_queue.pop();
            }
        }
        break;
        }
    }
}

void Translator::insns_stm(const Stm & stm, vm_assembler & vmasm, LocalEnv * lenv){

    switch(stm.kind){
    case stm_kind_expression:
        this->insns_val(*stm.u.e, vmasm, lenv);
        break;
    case stm_kind_assignment:
        this->insns_val(*stm.u.a->val, vmasm, lenv);
        if (stm.u.a->target->kind == expr_kind_var){
            if (lenv->is_toplevel || lenv->is_global(stm.u.a->target->u.var)){
                vmasm.GSET(vm_operand::name(stm.u.a->target->u.var), stm.pos);
            } else if (lenv != NULL && lenv->exists(stm.u.a->target->u.var)){
                vmasm.LSET(vm_operand::integer(lenv->look_var(stm.u.a->target->u.var)), stm.pos);
            } else if(! lenv->is_toplevel) {
                cerr << "compile error:::" << endl;
                cerr << "   " << stm.pos.filename << ":" << stm.pos.line_no << endl;
            } else {
                vmasm.GSET(vm_operand::name(stm.u.a->target->u.var), stm.pos);
            }
        } else if (stm.u.a->target->kind == expr_kind_subscript){
            Expr & sub = *stm.u.a->target;
            this->insns_push(*(*sub.u.sub)[0], vmasm, lenv);
            this->insns_push(*(*sub.u.sub)[1], vmasm, lenv);
            this->insns_val(*stm.u.a->val, vmasm, lenv);
            vmasm.SETITEM(stm.pos);
        }
        break;
    case stm_kind_del:
        if (stm.u.e->kind == expr_kind_subscript){
            this->insns_push(*(*stm.u.e->u.sub)[0], vmasm, lenv);
            this->insns_val(*(*stm.u.e->u.sub)[1], vmasm, lenv);
            vmasm.DELITEM(stm.pos);
        } else {
            cerr << "not implemented @ " << __FILE__ << ":" << __LINE__ << endl; exit(1);
        }
        break;
    case stm_kind_pass:
        // do nothing
        break;
    case stm_kind_return:
        this->insns_val(*stm.u.e, vmasm, lenv);
        vmasm.RET(stm.pos);
        break;
    case stm_kind_break:
        compile_error("no loop to break", stm.pos);
        break;
    case stm_kind_continue:
        compile_error("no loop to continue", stm.pos);
        break;
    case stm_kind_print:
        this->insns_val(*stm.u.e, vmasm, lenv);
        vmasm.PRINT(stm.pos);
        break;
    case stm_kind_global:
        if (! lenv->is_global(stm.u.g)){
            compile_error("invalid global declare", stm.pos);
        }
        break;
    case stm_kind_if:
    {
        queue<int> goto_endif_queue;

        for(vector<StmIfBranch*>::iterator itr = stm.u.i->begin()
                ; itr != stm.u.i->end(); itr++){
            if( (*itr)->elsep ){
                this->insns_stms((*itr)->s, vmasm, lenv);
            } else {
                this->insns_val(*(*itr)->e, vmasm, lenv);
                int gotoinst = vmasm.insns.size();
                vmasm.GOTOIFNOT(vm_operand::integer(-1), (*itr)->e->pos);
                this->insns_stms((*itr)->s, vmasm, lenv);
                if (itr != stm.u.i->end() - 1){
                    goto_endif_queue.push(vmasm.insns.size());
                    vmasm.GOTO(vm_operand::integer(-1), (*itr)->s.back()->pos);
                }
                vmasm.insns[gotoinst].operand = (void *)vmasm.insns.size();
            }
        }

        while(! goto_endif_queue.empty()){
            vmasm.insns[goto_endif_queue.front()].operand = (void *)vmasm.insns.size();
            goto_endif_queue.pop();
        }
    }
        break;
    case stm_kind_while:
    {
        int start_addr = vmasm.insns.size();
        this->insns_val(*stm.u.w->e, vmasm, lenv);
        int gotoif_addr = vmasm.insns.size();
        vmasm.GOTOIFNOT(vm_operand::integer(-1), stm.pos);
        
        vector<int> break_addrs;
        this->insns_loop_stms(stm.u.w->b, vmasm, start_addr, break_addrs, lenv);
        
        vmasm.GOTO(vm_operand::integer(start_addr), stm.pos);
        
        int endwhile_addr = vmasm.insns.size();
        vmasm.insns[gotoif_addr].operand = (void *)endwhile_addr;
        for(int i = 0;i < (int)break_addrs.size();i++){
            vmasm.insns[break_addrs[i]].operand = (void *)endwhile_addr;
        }
    }
        break;
    case stm_kind_for:
    {
        if (!lenv->is_toplevel && !lenv->exists(stm.u.f->x)){
            compile_error("loop variable '" + *stm.u.f->x + "' is not allocated.", stm.pos);
        }
        if (!lenv->is_toplevel && lenv->loopvar_queue.empty()){
            compile_error("loop local variables are not allocated.", stm.pos);
        }
        
        int loopvar = 0;
        int loopvar_index = 0;
        int loopvar_limit = 0;
        int loopseq = 0;

        // for range-for-loop
        int loopvar_step = 0;

        // 0 ... normal loop
        // 1 ... range(N) loop
        // 2 ... range(N, M) loop
        // 3 ... range(N, M, L) loop
        int loop_type = 0;
        if (stm.u.f->e->kind == expr_kind_call
            && stm.u.f->e->u.call->f->kind == expr_kind_var
            && stm.u.f->e->u.call->f->u.var->compare("range") == 0){
            loop_type = stm.u.f->e->u.call->args.size();
        }

        if (lenv->is_toplevel){
            switch (loop_type){
            case 1:
                loopvar_limit = lenv->current_offset();
                lenv->add_var(); // loop var limit
                vmasm.PUSH(stm.pos);

                loopvar_step = lenv->current_offset();
                lenv->add_var(); // loop var step
                vmasm.PUSH(stm.pos);
                break;
            case 2:
                loopvar_limit = lenv->current_offset();
                lenv->add_var(); // loop var limit
                vmasm.PUSH(stm.pos);

                loopvar_step = lenv->current_offset();
                lenv->add_var(); // loop var step
                vmasm.PUSH(stm.pos);
                break;
            case 3:
                loopvar_limit = lenv->current_offset();
                lenv->add_var(); // loop var limit
                vmasm.PUSH(stm.pos);

                loopvar_step = lenv->current_offset();
                lenv->add_var(); // loop var step
                vmasm.PUSH(stm.pos);
                break;
            default:
                loopvar_index = lenv->current_offset();
                lenv->add_var(); // loop var index
                vmasm.PUSH(stm.pos);

                loopvar_limit = lenv->current_offset();
                lenv->add_var(); // loop var index limit
                vmasm.PUSH(stm.pos);

                loopseq = lenv->current_offset();
                lenv->add_var(); // loop sequence
                vmasm.PUSH(stm.pos);
            }
        } else {
            if (! lenv->is_global(stm.u.f->x)){
                loopvar = lenv->look_var(stm.u.f->x);
            }
            switch (loop_type){
            case 1:
                loopvar_limit = lenv->loopvar_queue.front();
                loopvar_step = lenv->loopvar_queue.front() + 1;
                break;
            case 2:
                loopvar_limit = lenv->loopvar_queue.front();
                loopvar_step = lenv->loopvar_queue.front() + 1;
                break;
            case 3:
                loopvar_limit = lenv->loopvar_queue.front();
                loopvar_step = lenv->loopvar_queue.front() + 1;
                break;
            default:
                loopvar_index = lenv->loopvar_queue.front();
                loopvar_limit = lenv->loopvar_queue.front() + 1;
                loopseq = lenv->loopvar_queue.front() + 2;
            }
            lenv->loopvar_queue.pop();
        }
        
        vector<int> break_addrs;

//         switch (loop_type){
//         case 1:
//             cerr << "not implemented @ " << __FILE__ << ":" << __LINE__ << endl; exit(1);
//             break;
//         case 2:
//             cerr << "not implemented @ " << __FILE__ << ":" << __LINE__ << endl; exit(1);
//             break;
//         case 3:
//             cerr << "not implemented @ " << __FILE__ << ":" << __LINE__ << endl; exit(1);
//             break;
//         default:
//             break;
//         }

        // **** initialize ****
        switch (loop_type){
        case 1:
            vmasm.IMMVAL_NUM(vm_operand::pyval(mkint(0)), stm.pos);
            if (lenv->is_toplevel || lenv->is_global(stm.u.f->x)){
                vmasm.GSET(vm_operand::name(stm.u.f->x), stm.pos);
            } else {
                vmasm.LSET(vm_operand::integer(loopvar), stm.pos);
            }
            this->insns_val(*stm.u.f->e->u.call->args[0], vmasm, lenv);
            vmasm.LSET(vm_operand::integer(loopvar_limit), stm.pos);
            break;
        case 2:
            this->insns_val(*stm.u.f->e->u.call->args[0], vmasm, lenv);
            if (lenv->is_toplevel || lenv->is_global(stm.u.f->x)){
                vmasm.GSET(vm_operand::name(stm.u.f->x), stm.pos);
            } else {
                vmasm.LSET(vm_operand::integer(loopvar), stm.pos);
            }
            this->insns_val(*stm.u.f->e->u.call->args[1], vmasm, lenv);
            vmasm.LSET(vm_operand::integer(loopvar_limit), stm.pos);
            break;
        case 3:
            cerr << "not implemented @ " << __FILE__ << ":" << __LINE__ << endl; exit(1);
            break;
        default:
            vmasm.IMMVAL_NUM(vm_operand::pyval(Py_val::mk_int(0)), stm.pos);
            vmasm.LSET(vm_operand::integer(loopvar_index), stm.pos); // init loopvar index
            this->insns_val(*stm.u.f->e, vmasm, lenv);
            vmasm.LSET(vm_operand::integer(loopseq), stm.pos); // set loop seqence
            vmasm.LEN(stm.pos);
            vmasm.LSET(vm_operand::integer(loopvar_limit), stm.pos); // set loop limit
        }
        // **** initialize ****

        
        if (loop_type == 0){
            vmasm.LREF_PUSH(vm_operand::integer(loopvar_index), stm.pos);
        } else {
            if (lenv->is_toplevel || lenv->is_global(stm.u.f->x)){
                vmasm.GREF_PUSH(vm_operand::name(stm.u.f->x), stm.pos);
            } else {
                vmasm.LREF_PUSH(vm_operand::integer(loopvar), stm.pos);
            }
        }

        int first_addr = vmasm.insns.size(); // skip loopvar increment if firsttime
        vmasm.GOTO(vm_operand::integer(-1), stm.pos);

        
        // **** increment step ****
        int start_addr = vmasm.insns.size();
        switch (loop_type){
        case 1:
            if (lenv->is_toplevel || lenv->is_global(stm.u.f->x)){
                vmasm.GINC(vm_operand::name(stm.u.f->x), stm.pos);
                vmasm.GREF_PUSH(vm_operand::name(stm.u.f->x), stm.pos);
            } else {
                vmasm.LINC(vm_operand::integer(loopvar), stm.pos);
                vmasm.LREF_PUSH(vm_operand::integer(loopvar), stm.pos);
            }
            break;
        case 2:
            if (lenv->is_toplevel || lenv->is_global(stm.u.f->x)){
                vmasm.GINC(vm_operand::name(stm.u.f->x), stm.pos);
                vmasm.GREF_PUSH(vm_operand::name(stm.u.f->x), stm.pos);
            } else {
                vmasm.LINC(vm_operand::integer(loopvar), stm.pos);
                vmasm.LREF_PUSH(vm_operand::integer(loopvar), stm.pos);
            }
            break;
        case 3:
            cerr << "not implemented @ " << __FILE__ << ":" << __LINE__ << endl; exit(1);
            break;
        default:
            vmasm.LINC(vm_operand::integer(loopvar_index), stm.pos);
            vmasm.LREF_PUSH(vm_operand::integer(loopvar_index), stm.pos);

        }
        
        vmasm.insns[first_addr].operand = (void*)vmasm.insns.size();
        // condition check
        vmasm.LREF(vm_operand::integer(loopvar_limit), stm.pos);
        vmasm.NUMLT2(stm.pos);
        // **** increment step ****


            
        // goto end if loopvar_index >= loopvar_limit
        break_addrs.push_back(vmasm.insns.size());
        vmasm.GOTOIFNOT(vm_operand::integer(-1), stm.pos);


            
        // **** load loop sequence ****
        switch (loop_type){
        case 1:
            break;
        case 2:
            break;
        case 3:
            cerr << "not implemented @ " << __FILE__ << ":" << __LINE__ << endl; exit(1);
            break;
        default:
            vmasm.LREF_PUSH(vm_operand::integer(loopseq), stm.pos);
            vmasm.LREF(vm_operand::integer(loopvar_index), stm.pos);
            vmasm.GETITEM(stm.pos);
            if (lenv->is_toplevel || lenv->is_global(stm.u.f->x)){
                vmasm.GSET(vm_operand::name(stm.u.f->x), stm.pos);
            } else {
                vmasm.LSET(vm_operand::integer(loopvar), stm.pos);
            }
        }
        // **** load loop sequence ****

        this->insns_loop_stms(stm.u.f->b, vmasm, start_addr, break_addrs, lenv);
        vmasm.GOTO(vm_operand::integer(start_addr), stm.pos);

        int endfor_addr = vmasm.insns.size();
        for (int i = 0;i < (int)break_addrs.size();i++){
            vmasm.insns[break_addrs[i]].operand = (void*)endfor_addr;
        }

        if (lenv->is_toplevel){
            lenv->pop_var(); // loop var index
            vmasm.POP(stm.pos);

            lenv->pop_var(); // loop var index limit
            vmasm.POP(stm.pos);

            lenv->pop_var(); // loop sequence
            vmasm.POP(stm.pos);
        }
    }
        break;
    case stm_kind_fundef:

        LocalEnv * prev_lenv = lenv;
        lenv = new LocalEnv(stm.u.d->f);

        // register arguments as local variable
        for(vector<symbol_t>::iterator itr = stm.u.d->ps.begin();
            itr != stm.u.d->ps.end(); itr++){
            lenv->add_var(*itr);
        }
        // allocate stack for local vars
        this->scan_defun_local_var(stm.u.d->b, lenv);

        py_val_t self;
        if (prev_lenv->is_toplevel){
            vmasm.GDEFUN(vm_operand::name(stm.u.d->f), stm.pos);
            vmasm.GOTO(vm_operand::integer(-1), stm.pos);
            vmasm.fundef(stm.u.d->f, stm.u.d->ps.size()
                         , lenv->size() - stm.u.d->ps.size(), *genv, stm.pos);
            self = genv->lookup_sym(stm.u.d->f);
        } else {
            vmasm.LDEFUN(
                vm_operand::pyval(
                    self = Py_val::mk_vm_ifun(stm.u.d->f
                                              , stm.u.d->ps.size()
                                              , lenv->size() - stm.u.d->ps.size()
                                              , vmasm.insns.size() + 2
                                              , stm.pos))
                , stm.pos);
            vmasm.GOTO(vm_operand::integer(-1), stm.pos);
            prev_lenv->add_var(stm.u.d->f);
        }

        lenv->self = self;
        
        int goto_index = vmasm.label_index - 1;

        for(vector<Stm*>::iterator itr = stm.u.d->b.begin()
                ; itr != stm.u.d->b.end(); itr++){
            this->insns_stm(**itr, vmasm, lenv);
        }
        if (stm.u.d->b.back()->kind != stm_kind_return){
            vmasm.RET(stm.u.d->b.back()->pos);
        }
        vmasm.insns[goto_index].operand = (void*)vmasm.label_index;

        delete(lenv);
        lenv = prev_lenv;
        break;
    }
}

void Translator::compile_file(const vector<Stm*> & stms
                              , vm_assembler & vmasm, Env & genv){
    this->genv = &genv;

    LocalEnv toplevel(Symbol::get("*toplevel*"));
    toplevel.is_toplevel = true;

    this->scan_defun_local_var(stms, &toplevel);
    
    for(int i = 0; i < (int)stms.size();i++){
        this->insns_stm(*stms[i], vmasm, &toplevel);
    }
}

LocalEnv::LocalEnv(const symbol_t n): name(n){
    // FP points to the static link
    /*
              ...
              arg2
              arg1
           -----------
              info
    FP ->  static link
           -----------
    */
    this->offset = 0;
    this->is_toplevel = false;
}

void LocalEnv::add_var(const symbol_t var){
    if (! this->exists(var)){
        this->table[var] = this->offset++;
    }
}

// for loop temporaries allocation
void LocalEnv::add_var(){
    this->offset++;
}

void LocalEnv::pop_var(){
    this->offset--;
}

int LocalEnv::look_var(const symbol_t var){
    if (this->table.find(var) != this->table.end()) {
        int ret = this->table[var]; 
        if (ret >= offset){
            perror("compile error : LocalEnv::look_var");
            exit(1);
        }
        return ret;
    } else {
        return -1;
    }
}

int LocalEnv::size(){
    return this->offset;
}

void LocalEnv::mark_global(const symbol_t var){
    this->globals[var] = 1;
}

bool LocalEnv::is_global(const symbol_t var){
    return this->globals.find(var) != this->globals.end();
}

int LocalEnv::current_offset(){
    return this->offset;
}

bool LocalEnv::exists(const symbol_t var){
    return this->table.find(var) != this->table.end();
}

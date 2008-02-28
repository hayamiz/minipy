#include "eval.hpp"
//　演算子と添字を呼び出すとき、大域環境しか探してないけどいいかな？

py_val_t Eval::expr(Expr & e, Env & lenv, Env & genv,stack<Stack_trace_entry> & bt){
    
    switch(e.kind){
    case expr_kind_var:		/* 変数 */
    {
        py_val_t py =lenv.lookup_sym(e.u.var);
        if(py == py_val_not_found || py == py_val_global){
            py = genv.lookup_sym(e.u.var);
            if(py == py_val_not_found){
                runtime_error(bt,e.pos," no function name : " + *(e.u.var));
            }
        }
        return py;
    }
    break;
    case expr_kind_literal_int:	/* 整数リテラル */
        return  Py_val::mk_int(e.u.lit_i);
        break;
    case expr_kind_literal_float:/* 浮動小数点リテラル */
        return  Py_val::mk_float(e.u.lit_f);
        break;
    case expr_kind_literal_string:/* 文字列リテラル */
        return  Py_val::mk_string(*e.u.lit_s);
        break;
    case expr_kind_none:	/* None */
        return (py_val_t)py_val_none;
        break;
    case expr_kind_display_tuple:/* タプルを作る ( a, b, c,... ) */
    {
        Py_tuple * newtuple = new Py_tuple((unsigned)e.u.disp->size());
        vector<Expr>::iterator it;
        for (it=(*e.u.disp).begin(); it< (*e.u.disp).end(); it++){            newtuple->push_back(this->expr(*it,lenv,genv,bt));
    }
    return mknewtuple(newtuple);
/*
vector<py_val_t> py_vec;
        vector<Expr>::iterator it;
        for (it=(*e.u.disp).begin(); it< (*e.u.disp).end(); it++){
            py_vec.push_back(this->expr(*it,lenv,genv,bt));
        }
        //return  mknewtuple(py_vec);
        return  Py_val::mk_tuple(py_vec);
*/
    }
    break;
    case expr_kind_display_list:/* リストを作る [ a, b, c,...] */
    {
        vector<py_val_t> py_vec;
        vector<Expr>::iterator it;
        for (it=(*e.u.disp).begin(); it< (*e.u.disp).end(); it++){
            py_vec.push_back(this->expr(*it,lenv,genv,bt));
        }
        return  Py_val::mk_list(py_vec);
    }
    break;
    case expr_kind_display_dict:/* 辞書を作る   { a : x, b : y } */
        // see parser.cpp line 276
    {
        py_val_t newdict = mknewdict();
        vector<Expr>::iterator it;
        for (it=(*e.u.disp).begin(); it< (*e.u.disp).end(); it++){
            newdict->u.nd->set(
                this->expr(it->u.op->args[0], lenv,genv,bt)
                ,this->expr(it->u.op->args[1], lenv,genv,bt)
                ,bt,e.pos);
        }
        return newdict;
    }
    break;
    
    case expr_kind_paren:	/* 式を括弧でくくったもの ( e ) */
        return this->expr(*e.u.paren,lenv,genv,bt);
        break;
        
    case expr_kind_operator:	/* あらゆる演算子(+, -, ...)*/
        switch(e.u.op->kind){
        case TOK_KW_AND:
        {
            py_val_t a =expr(e.u.op->args[0],lenv,genv,bt);
            if(Py_val::is_false(a)){
                return a;
            }else{
                return expr(e.u.op->args[1],lenv,genv,bt);
            }
        }
        break;
        case TOK_KW_OR:
        {
            py_val_t a =expr(e.u.op->args[0],lenv,genv,bt);
            if(!(Py_val::is_false(a))){
                return a;
            }else{
                return expr(e.u.op->args[1],lenv,genv,bt);
            }
        }
        break;
        default:
        {
            py_val_t f = genv.lookup_sym(operator_fun_name(e.u.op->kind,e.u.op->fix,bt,e.pos));
            py_val_t args[(e.u.op->args).size()+1];
            vector<Expr>::iterator it;

            if (f == py_val_not_found){
                runtime_error(bt, e.pos, "function '" + *operator_fun_name(e.u.op->kind,e.u.op->fix,bt,e.pos) + "' is not found.");
            }

            if (f->type == py_type_ifun){
                // push bt
                bt.push(Stack_trace_entry(*Py_val::get_ifun_name(f,bt,e.pos),e.pos ));
            } else if(f->type == py_type_nfun) {
                bt.push(Stack_trace_entry(*Py_val::get_nfun_name(f,bt,e.pos),e.pos));
            } else {
                runtime_error(bt, e.pos, "type error (function name) ");
            }

            for (int i=0; i<(int)(e.u.op->args).size(); i++){
                args[i] = expr((e.u.op->args)[i],lenv,genv,bt);
            }
            args[(int)(e.u.op->args).size()]= NULL;
            py_val_t ret = this->expr_function_call(f,args,e,lenv,genv,bt);
            bt.pop();
            
            return ret;
        }
        break;
        }
        
        break;
    
    case expr_kind_attref:	/* e.f => attrepy_vec_dicf(e,"f")?? */
        runtime_error(bt, e.pos,
                      "SyntaxError: invalid syntax(attrepy) ");
        break;
    case expr_kind_subscript:/* e[e] => subscript(e,e) */
    {
        //大域環境を呼ぶ。
        py_val_t f =genv.lookup_sym(Symbol::get("getitem"));
        py_val_t args[3];
        args[0] = expr((*e.u.sub)[0], lenv,genv,bt); //添字
        args[1] = expr((*e.u.sub)[1], lenv,genv,bt); //引数
        args[2] = NULL;
        return this->expr_function_call(f,args,e,lenv,genv,bt);
    }
    break;
    case expr_kind_call:/* e(e,e,..) */
    {
        // 関数名の決定
        py_val_t f;  // 関数の名前
        py_val_t args_attref = py_val_not_found;  
        if(e.u.call->f.kind == expr_kind_attref){
            //see text(6) P15
            py_val_t py =lenv.lookup_sym((e.u.call->f).u.atr->f);
            if(py == py_val_not_found || py == py_val_global){
                py = genv.lookup_sym((e.u.call->f).u.atr->f);
                if(py == py_val_not_found){
                    runtime_error(bt,e.pos," no function name : "
                                  + *(e.u.call->f).u.atr->f);
                }
            }
            args_attref =expr(e.u.call->f.u.atr->a, lenv,genv,bt);
            f = py;
        }else{
            f = expr(e.u.call->f, lenv,genv,bt);
        }

        if (f->type == py_type_ifun){
            // push bt
            bt.push(Stack_trace_entry(*Py_val::get_ifun_name(f,bt,e.pos),e.pos ));
        } else if(f->type == py_type_nfun) {
            bt.push(Stack_trace_entry(*Py_val::get_nfun_name(f,bt,e.pos),e.pos));
        } else {
            runtime_error(bt, e.pos, "type error (function name) ");
        }

        // 引数を登録
        int num = 0;
        if(args_attref != py_val_not_found){
            num =1;
        }
        py_val_t args[(e.u.call->args).size()+num+1];  // 引数のリスト
        if(num == 1){
            args[0] =args_attref;
        }
        for(int i=0; i< (int)e.u.call->args.size(); i++){
            args[i+num] =expr((e.u.call->args)[i],lenv,genv,bt);
        }
        args[(e.u.call->args).size()+num] = NULL;
        // 関数呼び出し。
        py_val_t ret = this->expr_function_call(f,args,e,lenv,genv,bt);
        bt.pop();// pop bt
        return ret;
    }
    break;
    case expr_kind_list:
    {
        Py_tuple* newtuple = new Py_tuple((unsigned)e.u.disp->size());
        vector<Expr>::iterator it;
        for (it=(*e.u.disp).begin(); it< (*e.u.disp).end(); it++){
            newtuple->push_back(this->expr(*it,lenv,genv,bt));
        }
        return mknewtuple(newtuple);
//         vector<py_val_t> py_vec;
//         vector<Expr>::iterator it;
//         for (it=(*e.u.list).begin(); it< (*e.u.list).end(); it++){
//             py_vec.push_back(this->expr(*it,lenv,genv,bt));
//         }
//         return  Py_val::mk_list(py_vec);
    }
    break;
    default:
        runtime_error(bt, e.pos, "type error (expression)");
        break;
    }
    return 0;
}

py_val_t Eval::stmt(Stm & s, Env & lenv, Env & genv,stack<Stack_trace_entry> & bt){
    py_val_t ret = (py_val_t)py_val_none; //警告がでるので
    
    switch(s.kind){
    case stm_kind_expression:/* 例：  f(x) */
        this->expr(*(s.u.e),lenv,genv,bt);
        ret = py_val_next;
        break;
    case stm_kind_assignment:/* 代入.  a = x, a[x] = b, etc. */
        switch(s.u.a->target.kind){ // 左辺が変数かその他か
        case  expr_kind_var:
        {
            py_val_t v = this->expr(s.u.a->val,lenv,genv,bt);
            symbol_t x = s.u.a->target.u.var;
            if(genv.is_global(x)){
                genv.set_sym(x,v);
            }else {
                lenv.set_sym(x,v);
            }
            ret =  py_val_next;
        }
        break;
        case expr_kind_subscript:
        {
            //大域環境を呼ぶ。
            py_val_t f = genv.lookup_sym(Symbol::get("setitem"));
            py_val_t args[4];
            py_val_t container = expr((*s.u.a->target.u.sub)[0],lenv,genv,bt);
            if (container == py_val_not_found){
                runtime_error(bt, s.pos, "variable not found : "
                              + *(*s.u.a->target.u.sub)[0].u.var);
            }
            args[0] = container; //コンテナ
            args[1] =expr((*s.u.a->target.u.sub)[1],lenv,genv,bt); //添字
            args[2] = expr(s.u.a->val, lenv,genv,bt); //右辺
            args[3] =NULL;
            this->expr_function_call(f,args,s.u.a->val,lenv,genv,bt);
            ret =  py_val_next;
        }
        break;
        default:
            runtime_error(bt, s.pos, "syntax error: assignment");
            break;
        }
        break;
        
    case stm_kind_del:/* del.   del a[x] */
    {
        //大域環境を呼ぶ。
        py_val_t f =genv.lookup_sym(Symbol::get("delitem"));
        py_val_t args[3];
        args[0] = expr((*(s.u.e)->u.sub)[0], lenv,genv,bt); //添字
        args[1] = expr((*(s.u.e)->u.sub)[1], lenv,genv,bt); //引数
        args[2]=NULL;
        this->expr_function_call(f,args,*s.u.e,lenv,genv,bt);
        ret = py_val_next;
    }
    break;

    break;
    case stm_kind_pass:/* pass */
        ret = py_val_next;
        break;
    case stm_kind_return:/* return */
        ret = this->expr(*(s.u.e),lenv,genv,bt);
        break;
    case stm_kind_break:/* break */
        ret = py_val_break;
        break;
    case stm_kind_continue:/* continue */
        ret = py_val_continue;
        break;
    case stm_kind_print:/* print */
    {
        //大域環境を呼ぶ。
        py_val_t f = genv.lookup_sym(Symbol::get("print_"));
        py_val_t args[2];
        args[0] =expr(*s.u.e, lenv,genv,bt); //引数
        args[1]=NULL;
        this->expr_function_call(f,args,*s.u.e,lenv,genv,bt);
        ret = py_val_next;
    }
    break;
    case stm_kind_global:/* global */
    {
        symbol_t str =s.u.a->target.u.var;
        if(genv.is_global(str)){
            genv.set_global(str);
        }else{
            runtime_error(bt, s.pos, "Name error: '"+ *str+"' is already localized");
        }
        ret =  py_val_next;
    }
    break;
    case stm_kind_if:/* if文 */
    {
        ret = py_val_next;
        vector<StmIfBranch>::iterator it;
        for (it=(*s.u.i).begin(); it< (*s.u.i).end(); it++){
            if(it->elsep || !(Py_val::is_false(expr(it->e,lenv,genv,bt) ))){
                ret = this->stmt_vec(it->s,lenv,genv,bt);
                break;
            }
        }
    }
    break;
    case stm_kind_while:/* while文 */
    {
        ret = py_val_next;
        bool flag = true;
        while(!(Py_val::is_false(expr(s.u.w->e,lenv,genv,bt)))
              && flag ){
            py_val_t tmp = this->stmt_vec(s.u.w->b,lenv,genv,bt);
            switch((int)tmp){
            case py_val_continue_:
            case py_val_next_:
                break;
            case py_val_break_:
                ret = py_val_next;
                flag = false;
                break;
            default:
                ret = tmp;
                flag = false;
                break;
            }
        }
    }
    break;
    case stm_kind_for:/* for文 */
    {
        ret= py_val_next;
        
        if(s.u.f->e.kind == expr_kind_call
           && s.u.f->e.u.call->f.u.var == Symbol::get("range")){
            int start =0;
            int limit = 0;
            int inc = 1;
            switch(s.u.f->e.u.call->args.size()){
            case 1:
                limit = getint(
                    this->expr(s.u.f->e.u.call->args[0],lenv,genv,bt));
                break;
            case 2:
                start = getint(this->expr(s.u.f->e.u.call->args[0],lenv,genv,bt));
                limit = getint(this->expr(s.u.f->e.u.call->args[1],lenv,genv,bt));
                break;
            case 3:
                start = getint(this->expr(s.u.f->e.u.call->args[0],lenv,genv,bt));
                limit = getint(this->expr(s.u.f->e.u.call->args[1],lenv,genv,bt));
                inc = getint(this->expr(s.u.f->e.u.call->args[2],lenv,genv,bt));
                break;
            default:
                runtime_error(bt, s.pos, "SyntaxError: invalid syntax");
                break;
            }
            for(int i = start; i < limit; i = i+ inc){
                lenv.set_sym(s.u.f->x,mkint(i));
                py_val_t tmp_tl
                    = this->stmt_vec(s.u.f->b,lenv,genv,bt);
                switch((int)tmp_tl){
                case py_val_continue_:
                case py_val_next_:
                    break;
                case py_val_break_:
                    ret = py_val_next;
                    i = limit;
                    break;
                default:
                    ret = tmp_tl;
                    i = limit;
                    break;
                }
            }
        }else{
            py_val_t tmp = this->expr(s.u.f->e,lenv,genv,bt);
            switch (tmp->type){
            case py_type_string:
            {
                for(int i = 0;i < (int)(*tmp->u.s).size(); i++){
                    lenv.set_sym(s.u.f->x,
                                 Py_val::mk_char((*tmp->u.s)[i]));
                    py_val_t tmp_str
                        =this->stmt_vec(s.u.f->b,lenv,genv,bt);
                    switch(tmp_str->type){
                    case py_val_continue_:
                    case py_val_next_:
                        break;
                    case py_val_break_:
                        ret = py_val_next;
                        i = (int)(*tmp->u.s).size();
                        break;
                    default:
                        ret = tmp_str;
                        i =(int)(*tmp->u.s).size();
                        break;
                    }
                }
            }
        
            break;
            case py_type_tuple:
            case py_type_list:
            {
                vector<py_val_t>::iterator it;
                for(it=(*tmp->u.l).begin();it<(*tmp->u.l).end(); it++){
                    lenv.set_sym(s.u.f->x,*it);
                    py_val_t tmp_tl
                        = this->stmt_vec(s.u.f->b,lenv,genv,bt);
                    switch((int)tmp_tl){
                    case py_val_continue_:
                    case py_val_next_:
                        break;
                    case py_val_break_:
                        ret = py_val_next;
                        it = (*tmp->u.l).end();
                        break;
                    default:
                        ret = tmp_tl;
                        it = (*tmp->u.l).end();
                        break;
                    }
                }
            }
            break;
            default:
                runtime_error(bt, s.pos, "SyntaxError: invalid syntax");
                break;
        }
        }
    }

    break;
    case stm_kind_fundef:/* def文．これも実は文の一種 */
        lenv.set_sym(s.u.d->f,
                 Py_val::mk_ifun(s.u.d->f,s.u.d->ps,s.u.d->b));
        ret = py_val_next;
        break;
    default:
        break;
    }

    return ret;
}


py_val_t Eval::file_input(vector<Stm> & u, Env & genv,stack<Stack_trace_entry> & bt){
    vector<Stm>::iterator it;
    py_val_t tmp = (py_val_t)py_val_none; // ?
    for (it=u.begin(); it< u.end(); it++){
        tmp =this->stmt(*it,genv,genv,bt);
        if( tmp == py_val_break){
            runtime_error(bt, it->pos, "SyntaxError: 'continue' not properly in loop");
        }else if(tmp == py_val_continue ){
            runtime_error(bt, it->pos, "SyntaxError: 'break' not properly in loop");
        }else if(tmp != py_val_next){
            runtime_error(bt, it->pos, "SyntaxError: 'return' not properly in loop");
        } 
    }
    return tmp;
}



// 関数名にたいして環境から関数を探して引数をもとに評価
py_val_t Eval::expr_function_call(py_val_t f, py_val_t * args, Expr & e, Env & lenv, Env & genv,stack<Stack_trace_entry> & bt){

    int args_length = 0;
    while( args[args_length] != NULL ){
        args_length++; 
    }
    py_val_t tmp=(py_val_t)py_val_none;
    if(f->type == py_type_ifun){ // Python function
        if((int)f->u.i->ps.size() !=  args_length){
            genv.debug_print_all();
            runtime_error(bt, e.pos, "no matching function for call:" + *f->u.s);
        }
        Env lEnv;
        for(int i = 0; i < (int)f->u.i->ps.size(); i++){
            lEnv.set_sym(f->u.i->ps[i],args[i]);
        }
        
        tmp = Eval::stmt_vec(f->u.i->b, lEnv, genv, bt);
        if(tmp == py_val_next){
            tmp = (py_val_t)py_val_none;
        }else if(tmp == py_val_continue){
            runtime_error(bt, e.pos, "out of loop : continue");
        }else if(tmp == py_val_break){
            runtime_error(bt, e.pos, "out of loop : break");
        }
    }else if(f->type == py_type_nfun){ // Native function
        if(f->u.n->arity >= 0 && f->u.n->arity !=  args_length){
            runtime_error(bt, e.pos, "no matching function for call : " + *f->u.s);
        } // arity(引数の数)が負の場合, 任意個の引数をとれることにする.
        // c function call
        tmp = f->u.n->f(bt,e.pos,args);
    }
    return tmp;
    
}

symbol_t Eval::operator_fun_name(token_kind_t k, fix_kind_t fix,stack<Stack_trace_entry> & bt, SrcPos & p){
    string ret = "";
    switch(k){
    case TOK_KW_NOT:
        ret = "not_";
        break;
    case TOK_KW_IN:
        ret = "contains";
        break;
    case TOK_KW_NOT_IN:
        ret = "not_contains";
        break;
    case TOK_KW_IS:
        ret = "ntv_is";
        break;
    case TOK_KW_IS_NOT:
        ret = "ntv_is_not";
        break;
    case TOK_EQ_EQ: //==
        ret = "eq";
        break;
    case TOK_NEQ: // !=
        ret = "ne";
        break;
    case TOK_GT:    // >
        ret = "gt";
        break;
    case TOK_GEQ:   // >
        ret = "ge";
        break;
    case  TOK_LT:    // <
        ret = "lt";
        break;
    case  TOK_LEQ:   // <=
        ret = "le";
        break;
    case TOK_PLUS: // +(infix/prefix)
        if(fix ==  fix_kind_infix){
            ret = "add";
        }else if(fix == fix_kind_prefix){
            ret = "prefix_add";
        }else{
            runtime_error(bt, p, "without fixtype (+)");
        }
        break;
    case TOK_MINUS: // -(infix/prefix)
        if(fix ==  fix_kind_infix){
            ret = "sub";
        }else if(fix == fix_kind_prefix){
            ret = "prefix_sub";
        }else{
            runtime_error(bt, p, "without fixtype (-)");
        }
        break;
    case TOK_MUL: // *
        ret = "mul";
        break;
    case TOK_DIV: // /
        ret = "div";
        break;
    case TOK_MOD: // %
        ret = "mod";
        break;
    case TOK_TILDE: // ~
        ret = "invert";
        break;
    case TOK_LSHIFT: // <<
        ret = "lshift";
        break;
    case TOK_RSHIFT: // >>
        ret = "rshift";
        break;
    case TOK_AMP: // &
        ret = "and_";
        break;
    case TOK_BAR: // |
        ret = "or_";
        break;
    case TOK_XOR: // ^
        ret = "xor_";
        break;
    default:
        runtime_error(bt, p,
                      "SyntaxError: invalid syntax(operator) ");
        break;
    }
    
    return Symbol::get(ret);
}

py_val_t Eval::stmt_vec(vector<Stm> s, Env & lenv, Env & genv, stack<Stack_trace_entry> & bt){
    vector<Stm>::iterator it;
    for (it=s.begin(); it< s.end(); it++){
        py_val_t tmp =this->stmt(*it,lenv,genv,bt);
        if(tmp != py_val_next){
            return tmp;
        }
    }
    return  py_val_next;
}

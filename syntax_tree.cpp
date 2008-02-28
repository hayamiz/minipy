
#include "syntax_tree.hpp"

// Constructors for Expression

Expr Expr::make_var(string var, SrcPos p){
    Expr expr(expr_kind_var, p);
    expr.u.var = Symbol::get(var);
    return expr;
}
Expr Expr::make_literal_int(int literal_int, SrcPos p){
    Expr expr(expr_kind_literal_int, p);
    expr.u.lit_i = literal_int;
    return expr;
}
Expr Expr::make_literal_float(double literal_float, SrcPos p){
    Expr expr(expr_kind_literal_float, p);
    expr.u.lit_f = literal_float;
    return expr;
}
Expr Expr::make_literal_string(string literal_string, SrcPos p){
    Expr expr(expr_kind_literal_string, p);
    expr.u.lit_s = Symbol::get(literal_string);
    return expr;
}
Expr Expr::make_none(SrcPos p){
    Expr expr(expr_kind_none, p);
    return expr;
}
Expr Expr::make_display_tuple(vector<Expr> display_tuple, SrcPos p){
    Expr expr(expr_kind_display_tuple, p);
    expr.u.disp = new vector<Expr>(display_tuple);
    return expr;
}
Expr Expr::make_display_list(vector<Expr> display_list, SrcPos p){
    Expr expr(expr_kind_display_list, p);
    expr.u.disp = new vector<Expr>(display_list);
    return expr;
}
Expr Expr::make_display_dict(vector<Expr> display_dict, SrcPos p){
    Expr expr(expr_kind_display_dict, p);
    expr.u.disp = new vector<Expr>(display_dict);
    return expr;
}
Expr Expr::make_list(vector<Expr> list, SrcPos p){
    Expr expr(expr_kind_list, p);
    expr.u.list = new vector<Expr>(list);
    return expr;
}
Expr Expr::make_paren(Expr paren, SrcPos p){
    Expr expr(expr_kind_paren, p);
    expr.u.paren = new Expr(paren);
    return expr;
}
Expr Expr::make_infix_operator(token_kind_t op, Expr a, Expr b, SrcPos p){
    Expr expr(expr_kind_operator, p);
    vector<Expr> ls; ls.push_back(a); ls.push_back(b);
    expr.u.op = new ExprOperator(op, fix_kind_infix, ls);
    return expr;
}
Expr Expr::make_prefix_operator(token_kind_t op, Expr a, SrcPos p){
    Expr expr(expr_kind_operator, p);
    vector<Expr> ls; ls.push_back(a);
    expr.u.op = new ExprOperator(op, fix_kind_prefix, ls);
    return expr;
}
Expr Expr::make_attref(Expr a, string f, SrcPos p){
    Expr expr(expr_kind_attref, p);
    expr.u.atr = new ExprAttref(a, Symbol::get(f));
    return expr;
}
Expr Expr::make_subscript(Expr a, Expr i, SrcPos p){
    Expr expr(expr_kind_subscript, p);
    vector<Expr> ls; ls.push_back(a); ls.push_back(i);
    expr.u.sub = new vector<Expr>(ls);
    return expr;
}
Expr Expr::make_call(Expr f, vector<Expr> args, SrcPos p){
    Expr expr(expr_kind_call, p);
    expr.u.call = new ExprCall(f, args);
    return expr;
}


// Constructors for Statement

Stm Stm::make_expression(Expr e, SrcPos pos){
    Stm stm(stm_kind_expression, pos);
    stm.u.e = new Expr(e);
    return stm;
}
Stm Stm::make_assignment(Expr t, Expr v, SrcPos pos){
    Stm stm(stm_kind_assignment, pos);
    stm.u.a = new StmAssign(t, v);
    return stm;
}
Stm Stm::make_pass(SrcPos pos){
    Stm stm(stm_kind_pass, pos);
    return stm;
}
Stm Stm::make_return(Expr e, SrcPos pos){
    Stm stm(stm_kind_return, pos);
    stm.u.e = new Expr(e);
    return stm;
}
Stm Stm::make_break(SrcPos pos){
    Stm stm(stm_kind_break, pos);
    return stm;
}
Stm Stm::make_continue(SrcPos pos){
    Stm stm(stm_kind_continue, pos);
    return stm;
}
Stm Stm::make_print(Expr e, SrcPos pos){
    Stm stm(stm_kind_print, pos);
    stm.u.e = new Expr(e);
    return stm;
}
Stm Stm::make_del(Expr e, SrcPos pos){
    Stm stm(stm_kind_del, pos);
    stm.u.e = new Expr(e);
    return stm;
}
Stm Stm::make_global(string g, SrcPos pos){
    Stm stm(stm_kind_global, pos);
    stm.u.g = Symbol::get(g);
    return stm;
}
Stm Stm::make_if(vector<StmIfBranch> brs, SrcPos pos){
    Stm stm(stm_kind_if, pos);
    stm.u.i = new vector<StmIfBranch>(brs);
    return stm;
}
Stm Stm::make_while(Expr e, vector<Stm> b, SrcPos pos){
    Stm stm(stm_kind_while, pos);
    stm.u.w = new StmWhile(e, b);
    return stm;
}
Stm Stm::make_for(string x, Expr e, vector<Stm> b, SrcPos pos){
    Stm stm(stm_kind_for, pos);
    stm.u.f = new StmFor(Symbol::get(x), e, b);
    return stm;
}
Stm Stm::make_fundef(string f, vector<string> p, vector<Stm> b, SrcPos pos){
    Stm stm(stm_kind_fundef, pos);
    vector<string>::iterator it;
    vector<symbol_t> sym;
    for(it=p.begin();it<p.end(); it++){
        sym.push_back(Symbol::get(*it));
    }
    stm.u.d = new StmFunDef(Symbol::get(f), sym, b);
    return stm;
}


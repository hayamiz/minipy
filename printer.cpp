
#include "printer.hpp"

bool Printer::initp = false;
map<token_kind_t, string> Printer::operator_map;

void Printer::init(){
    if (! Printer::initp){

        // generated by generator.el
        // (minipy-insert-operator-string-map)
        operator_map[TOK_EQ] = "=";
        operator_map[TOK_EQ_EQ] = "==";
        operator_map[TOK_NEQ] = "!=";
        operator_map[TOK_GT] = ">";
        operator_map[TOK_GEQ] = ">=";
        operator_map[TOK_LT] = "<";
        operator_map[TOK_LEQ] = "<=";
        operator_map[TOK_PLUS] = "+";
        operator_map[TOK_MINUS] = "-";
        operator_map[TOK_MUL] = "*";
        operator_map[TOK_DIV] = "/";
        operator_map[TOK_MOD] = "%";
        operator_map[TOK_TILDE] = "~";
        operator_map[TOK_LSHIFT] = "<<";
        operator_map[TOK_RSHIFT] = ">>";
        operator_map[TOK_XOR] = "^";
        operator_map[TOK_AMP] = "&";
        operator_map[TOK_BAR] = "|";
        operator_map[TOK_KW_OR] = "or";
        operator_map[TOK_KW_AND] = "and";
        operator_map[TOK_KW_NOT] = "not";
        operator_map[TOK_KW_IS] = "is";
        operator_map[TOK_KW_IN] = "in";
        operator_map[TOK_KW_IS_NOT] = "is not";
        operator_map[TOK_KW_NOT_IN] = "not in";
        operator_map[TOK_COLON] = ":";
// ---- operator string map ----
        
        Printer::initp = false;
    }
}

void Printer::print_expr_list_with_comma(vector<Expr*> & expr_list){
    if (expr_list.size() >= 1){
        Printer::print_expr(*expr_list[0]);

        vector<Expr*>::iterator itr = expr_list.begin();
        itr++;
        while(itr != expr_list.end()){
            cout << ", ";
            Printer::print_expr(**itr);
            itr++;
        }
    }
}

void Printer::print_if_branches(vector<StmIfBranch*> & branches, int indent_level){
    vector<StmIfBranch*>::iterator branchitr
        = branches.begin();
    vector<Stm*>::iterator itr = (**branchitr).s.begin();

    cout << "if ";
    Printer::print_expr(*(**branchitr).e);
    cout << ":" << endl;
    while (itr != (**branchitr).s.end()){
        Printer::print_stm(**itr, indent_level + 1);
        itr++;
    }
    branchitr++;
        
    while(branchitr != branches.end()){
        int i = indent_level;
        while(i > 0){
            cout << "    ";
            i--;
        }
                
        if((**branchitr).elsep)
            cout << "else";
        else{
            cout << "elif ";
            Printer::print_expr(*(**branchitr).e);
        }
        cout << ":" << endl;

        itr = (**branchitr).s.begin();
        while (itr != (**branchitr).s.end()){
            Printer::print_stm(**itr, indent_level + 1);
            itr++;
        }
        branchitr++;
    }
}

void Printer::print_expr(Expr & expr){
    Printer::init();
    
    switch(expr.kind){
    case expr_kind_var:
        cout << *expr.u.var;
        break;
    case expr_kind_literal_int:
        cout << expr.u.lit_i;
        break;
    case expr_kind_literal_float:
        printf("%lf", expr.u.lit_f);
        break;
    case expr_kind_literal_string:
        cout << '"';
        for(string::iterator itr = (*expr.u.lit_s).begin();
            itr != (*expr.u.lit_s).end();
            itr++){
            if (*itr == '"'){
                cout << "\\\"";
            } else {
                cout << *itr;
            }
        }
        cout << '"';
        break;
    case expr_kind_none:
        cout << "None";
        break;
    case expr_kind_display_tuple:
        cout << "(";
        Printer::print_expr_list_with_comma(*expr.u.disp);

        if ((*expr.u.disp).size() == 1){
            cout << ",";
        }

        cout << ")";
        break;
    case expr_kind_list:
        Printer::print_expr_list_with_comma(*expr.u.list);
        break;
    case expr_kind_display_list:
        cout << "[";
        Printer::print_expr_list_with_comma(*expr.u.disp);
        cout << "]";
        break;
    case expr_kind_display_dict:
        cout << "{";
        Printer::print_expr_list_with_comma(*expr.u.disp);
        cout << "}";
        break;
    case expr_kind_paren:
        cout << "(";
        Printer::print_expr(*expr.u.paren);
        cout << ")";
        break;
    case expr_kind_operator:
        switch((*expr.u.op).fix){
        case fix_kind_infix:
            Printer::print_expr(*(*expr.u.op).args[0]);
            cout << " " << Printer::operator_map[(*expr.u.op).kind] << " ";
            Printer::print_expr(*(*expr.u.op).args[1]);
            break;
        case fix_kind_prefix:
            cout << Printer::operator_map[(*expr.u.op).kind] << " ";
            Printer::print_expr(*(*expr.u.op).args[0]);
            break;
        }
        break;
    case expr_kind_attref:
        Printer::print_expr(*(*expr.u.atr).a);
        cout << "." << *(*expr.u.atr).f;
        break;
    case expr_kind_subscript:
        Printer::print_expr(*(*expr.u.sub)[0]);
        cout << "[";
        Printer::print_expr(*(*expr.u.sub)[1]);
        cout << "]";
        break;
    case expr_kind_call:
        Printer::print_expr(*(*expr.u.call).f);
        cout << "(";
        Printer::print_expr_list_with_comma((*expr.u.call).args);
        cout << ")";
        break;
    }
}

void Printer::print_stm(Stm & stm, int indent_level){
    Printer::init();

    int i = indent_level;
    while(i > 0){
        cout << "    ";
        i--;
    }

    switch(stm.kind){
    case stm_kind_expression:
        Printer::print_expr(*stm.u.e);
        cout << endl;
        break;
    case stm_kind_assignment:
        Printer::print_expr(*(*stm.u.a).target);
        cout << " = ";
        Printer::print_expr(*(*stm.u.a).val);
        cout << endl;
        break;
    case stm_kind_del:
        cout << "del ";
        Printer::print_expr(*stm.u.e);
        cout << endl;
        break;
    case stm_kind_pass:
        cout << "pass" << endl;
        break;
    case stm_kind_return:
        cout << "return ";
        Printer::print_expr(*stm.u.e);
        cout << endl;
        break;
    case stm_kind_break:
        cout << "break";
        cout << endl;
        break;
    case stm_kind_continue:
        cout << "continue";
        cout << endl;
        break;
    case stm_kind_print:
        cout << "print ";
        Printer::print_expr(*stm.u.e);
        cout << endl;
        break;
    case stm_kind_global:
        cout << "global " << *stm.u.g;
        cout << endl;
        break;
    case stm_kind_if:
        Printer::print_if_branches(*stm.u.i, indent_level);
        break;
    case stm_kind_while:
        cout << "while ";
        Printer::print_expr(*(*stm.u.w).e);
        cout << ":" << endl;
        for(vector<Stm*>::iterator itr = (*stm.u.w).b.begin();
            itr != (*stm.u.w).b.end();
            itr++){
            Printer::print_stm(**itr, indent_level + 1);
        }
        break;
    case stm_kind_for:
        cout << "for " << *(*stm.u.f).x << " in ";
        Printer::print_expr(*(*stm.u.f).e);
        cout << ":" << endl;
        for(vector<Stm*>::iterator itr = (*stm.u.f).b.begin();
            itr != (*stm.u.f).b.end();
            itr++){
            Printer::print_stm(**itr, indent_level + 1);
        }
        break;
    case stm_kind_fundef:
        cout << "def " << *(*stm.u.d).f << "(";

        if ((*stm.u.d).ps.size() >= 1){
            cout << *(*stm.u.d).ps[0];
            vector<symbol_t>::iterator itr = (*stm.u.d).ps.begin();
            itr++;
            while(itr != (*stm.u.d).ps.end()){
                cout << **itr;
                itr++;
            }
        }
        cout << "):" << endl;
        
        vector<Stm*>::iterator itr = (*stm.u.d).b.begin();
        while(itr != (*stm.u.d).b.end()){
            Printer::print_stm(**itr, indent_level + 1);
            itr++;
        }
        break;
    }
}



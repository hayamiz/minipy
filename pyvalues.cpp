// -*- coding: utf-8 -*-

#include "pyvalues.hpp"

Stack_trace_entry::Stack_trace_entry(const string & n, const SrcPos & pos)
    : name(n), call_site(pos){
}

void print_stack_trace(ConsStack<Stack_trace_entry*> * bt){
    string filename, name;
    int line_no;
    //int column_no;
    while(bt != NULL){
        filename = bt->car()->call_site.filename;
        line_no = bt->car()->call_site.line_no;
        // column_no = bt.top().call_site.column_no;
        name = bt->car()->name;

        cout << filename << ":"
             << line_no << ": "
             << name << endl;
        bt = bt->pop();
    }
}

void runtime_error(ConsStack<Stack_trace_entry*> * bt, const SrcPos & pos, const string & msg){
    cout << pos.filename << ":"
         << pos.line_no  << " "
         << "runtime error :::"<< endl;
    cout << "   "<<  msg << endl;

    if(bt != NULL){
        cout << "----stack trace----" << endl;
        print_stack_trace(bt);
    }

    exit(1);
}


bool Py_val::is_false(py_val_t pyval){
    /* Pythonで偽とみなされる値ならば1:
       後にif文の条件部を評 価する際に使う
       None
       整数の0
       空文字列("")
       空タプル(())
       空リスト([])
       空辞書({})
    */
    SrcPos pos;   // 偽pos

    if (Py_val::is_int(pyval)){
        return (getint(pyval) == 0 ? true : false);
    } else if (Py_val::is_char(pyval)){
        return false;
    } else if (Py_val::is_none(pyval)){
        return true;
    }

    switch(pyval->type){
    case py_type_string:
        if((pyval->u.s)->size() == 0){
            return true;
        }else{
            return false;
        }
        break;
        
    case py_type_tuple:
    case py_type_list:
        if((pyval->u.l)->size() == 0){
            return true;
        }else{
            return false;
        }
        break;

    case py_type_dict:
        if((pyval->u.d)->size() == 0){
            return true;
        }else{
            return false;
        }
        break;

    default:
        return false;
    }

    return false;
}

bool Py_val::is_true(py_val_t pyval){
    return !Py_val::is_false(pyval);
}

int Py_val::compare(py_val_t val1, py_val_t val2){
    if (!is_boxed(val1)){
        if (val1 == py_val_none){
            return (val1 == py_val_none ? 0 : -1);
        } else if (is_int(val1)){
            if (is_int(val2)){
                return (int)val1 - (int)val2;
            } else if(is_float(val2)){
                return ((double)getint(val1) < getfloat(val2) ? -1
                        : ((double)getint(val1) == getfloat(val2) ? 0 : 1));
            }
            return -1;
        } else if (is_char(val1)){
            if (is_number(val2)) {
                return 1;
            } else if ( is_char(val2)){
                return (int)val1 - (int)val2;
            } else if ( is_string(val2)) {
                switch((getstring(val2)->size() == 0)){
                case 0:
                    return 1;
                    break;
                case 1:
                    return getchar(val1) - getstring(val2)->at(0);
                    break;
                default:
                    if (getchar(val1) - getstring(val2)->at(0) > 0){
                        return 1;
                    } else {
                        return -1;
                    }
                }
            } else if ( is_list(val2)) {
                return 1;
            }
            return -1;
        }
        cerr << "never reaches here at " << __FILE__ << ":" << __LINE__; exit(1);
    }

    switch (val1->type){
    case py_type_float:
        if (is_none(val2)){
            return 1;
        } else if (is_int(val2)){
            return (getfloat(val1) < (double)getint(val2) ? -1
                    : (getfloat(val1) == (double)getint(val2) ? 0 : 1));
        } else if (is_float(val2)) {
            return (getfloat(val1) < getfloat(val2) ? -1
                    : (getfloat(val1) == getfloat(val2) ? 0 : 1));
        } else {
            return -1;
        }
        break;
    case py_type_ifun:
        if(is_number(val2)){
            return 1;
        } else if(is_ifun(val2)){
            return getifun(val1)->name->compare(*getifun(val2)->name);
        } else {
            return -1;
        }
        break;
    case py_type_vm_ifun:
        if(is_number(val2)||is_ifun(val2)){
            return 1;
        } else if(is_vfun(val2)){
            return getvfun(val1)->name->compare(*getvfun(val2)->name);
        } else {
            return -1;
        }
        break;
    case py_type_nfun:
        if(is_number(val2) || is_ifun(val2) || is_vfun(val2)){
            return 1;
        } else if(is_nfun(val2)){
            return getnfun(val1)->name->compare(*getnfun(val2)->name);
        } else {
            return -1;
        }
        break;
    case py_type_list:
        if(is_number(val2) || is_ifun(val2) || is_vfun(val2) || is_nfun(val2)){
            return 1;
        } else if(is_list(val2)){
            return compare_list(val1, val2);
        } else {
            return -1;
        }
        break;
    case py_type_newdict:
        if(is_newtuple(val2) || is_tuple(val2) || is_newdict(val2) || is_string(val2)){
            return -1;
        } else if(is_newdict(val2)){
            return compare_newdict(val1, val2);
        } else {
            return 1;
        }
        break;
    case py_type_dict:
        cerr << "obsolete program path at " << __FILE__ << ":" << __LINE__; exit(1);
        break;
    case py_type_string:
        if(is_tuple(val2) || is_newdict(val2)){
            return -1;
        } else if(is_char(val2)){
            switch((getstring(val1)->size() == 0)){
            case 0:
                return -1;
                break;
            case 1:
                return getstring(val1)->at(0) - getchar(val2);
                break;
            default:
                if (getstring(val1)->at(0) - getchar(val2) < 0){
                    return -1;
                } else {
                    return 1;
                }
            }
        } else if (is_string(val2)) {
            return getstring(val1)->compare(*getstring(val2));
        } else {
            return 1;
        }
        break;
    case py_type_tuple:
        if (is_newtuple(val2)){
            return -1;
        } else if (is_tuple(val2)){
            return compare_tuple(val1, val2);
        } else {
            return 1;
        }
        break;
    case py_type_newtuple:
        if (is_newtuple(val2)){
            return compare_newtuple(val1, val2);
        } else {
            return 1;
        }
        break;
    default:
        cerr << "never reaches here at " << __FILE__ << ":" << __LINE__; exit(1);
    }

    cerr << "never reaches here at " << __FILE__ << ":" << __LINE__; exit(1);
    return 0;
}

int Py_val::compare_list(py_val_t val1, py_val_t val2){
    return compare_tuple(val1, val2);
}

int Py_val::compare_newdict(py_val_t val1, py_val_t val2){
    cerr << "not impletemted at " << __FILE__ << ":" << __LINE__; exit(1);
    return 0;
}

int Py_val::compare_tuple(py_val_t val1, py_val_t val2){
    vector<py_val_t> * tup1, * tup2;

    if (val1 == val2) return 0;
    
    tup1 = val1->u.l; tup2 = val2->u.l;
    int tup1size = tup1->size(); int tup2size = tup2->size();
    int limit = tup1size < tup2size ? tup1size : tup2size;

    for(int i = 0;i < limit;i++){
        int ret = compare((*tup1)[i], (*tup2)[i]);
        if (ret != 0) return ret;
    }

    if (tup1size < tup2size){
        return -1;
    } else if (tup1size > tup2size){
        return 1;
    } 

    return 0;
}

int Py_val::compare_newtuple(py_val_t val1, py_val_t val2){
    if (val1 == val2) return 0;
    
    Py_tuple * tup1 = val1->u.nl;
    Py_tuple * tup2 = val2->u.nl;
    int tup1size = tup1->size(); int tup2size = tup2->size();
    int limit = tup1size < tup2size ? tup1size : tup2size;

    for(int i = 0;i < limit;i++){
        int ret = compare(tup1->get(i), tup2->get(i));
        if (ret != 0) return ret;
    }

    if (tup1size < tup2size){
        return -1;
    } else if (tup1size > tup2size){
        return 1;
    } 

    return 0;
}


void Py_val::to_str(ostream & ss, py_val_t pyval, ConsStack<Stack_trace_entry*> * bt, const SrcPos & p){
    
    if (Py_val::is_int(pyval)) {
        ss << getint(pyval);
        return;
    } else if (Py_val::is_char(pyval)) {
        ss.put(getchar(pyval));
        return;
    } else if (Py_val::is_none(pyval)){
        ss << "None";
        return;
    }

    switch(pyval->type){
    case py_type_float:
        ss << getfloat(pyval);
        break;
    case py_type_newtuple:
    {
        Py_tuple * tuple = getnewtuple(pyval);
        uint tupsize = tuple->size();
        ss << "(";

        if (tupsize == 0){
            ss << ")";
        } else if (tupsize == 1){
            to_str(ss, tuple->get(0), bt, p);
            ss << ",)";
        } else {
            to_str(ss, tuple->get(0), bt, p);
            
            for(uint i = 1;i < tupsize;i++){
                ss << ", ";
                to_str(ss, tuple->get(i), bt, p);
            }
            ss << ")";
        }
    }
    break;
    case py_type_tuple:
    {
        vector<py_val_t> * tuple = gettuple(pyval);
        int tupsize = tuple->size();
        ss << "(";

        if (tupsize == 0){
            ss << ")";
        } else if (tupsize == 1){
            to_str(ss, (*tuple)[0], bt, p);
            ss << ",)";
        } else {
            to_str(ss, (*tuple)[0], bt, p);
            
            for(vector<py_val_t>::iterator itr = tuple->begin() + 1
                    ; itr != tuple->end(); itr++){
                ss << ", ";
                to_str(ss, *itr, bt, p);
            }
            ss << ")";
        }
    }
    break;
    case py_type_list:
    {
        vector<py_val_t> * list = getlist(pyval);
        ss << "[";

        if (list->size() > 0){
            to_str(ss, (*list)[0], bt, p);

            for(vector<py_val_t>::iterator itr = list->begin() + 1
                    ; itr != list->end(); itr++){
                ss << ", ";
                to_str(ss, *itr, bt, p);
            }
        }
        ss << "]";
    }
    break;
    case py_type_dict:
    {
        ss << "{";
        vector<Py_dict_entry*> * dict_list = getdict(pyval);
        if (dict_list->size() > 0){
            to_str(ss, (*dict_list)[0]->key, bt, p);
            ss << " : ";
            to_str(ss, (*dict_list)[0]->val, bt, p);

            for(vector<Py_dict_entry*>::iterator itr = dict_list->begin() + 1
                    ; itr != dict_list->end(); itr++){
                ss << ", ";
                to_str(ss, (*itr)->key, bt, p);
                ss << " : ";
                to_str(ss, (*itr)->val, bt, p);
            }
        }
        ss << "}";
    }
    break;
    case py_type_newdict:
    {
        ss << "{";
        py_val_t key;
        vector<py_val_t> * keys = getlist(getnewdict(pyval)->get_keys(bt, p));
        if (keys->size() > 0){
            to_str(ss, key = (*keys)[0], bt, p);
            ss << " : ";
            to_str(ss, getnewdict(pyval)->get(key, bt, p), bt, p);

            for(vector<py_val_t>::iterator itr = keys->begin() + 1
                    ; itr != keys->end(); itr++){
                ss << ", ";
                to_str(ss, key = *itr, bt, p);
                ss << " : ";
                to_str(ss, getnewdict(pyval)->get(key, bt, p), bt, p);
            }
        }
        ss << "}";
    }
    break;
    case py_type_string:
        ss << "'" << *getstring(pyval) << "'";
        break;
    case py_type_ifun:
        ss << "<function " << pyval->u.i->name << " at "
           << pyval->u.i->b[0]->pos.filename << ":"
           << pyval->u.i->b[0]->pos.line_no << ">";
        break;
    case py_type_vm_ifun:
        ss << "<function " << pyval->u.vm_i->name << " at "
           << pyval->u.vm_i->pos.filename << ":"
           << pyval->u.vm_i->pos.line_no << ">";
        break;
    case py_type_nfun:
        ss << "<function " << pyval->u.n->name << " at "
           << pyval->u.n->f << ">";
        break;
    default:
        cerr << "never reaches here at " << __FILE__ << ":" << __LINE__; exit(1);
    }
}

void Py_val::print(py_val_t pyval, ConsStack<Stack_trace_entry*> * bt, const SrcPos & p){

    if (!is_boxed(pyval)){
        if (Py_val::is_int(pyval)) {
            printf("%d", getint(pyval));
            return;
        } else if (Py_val::is_char(pyval)) {
            printf("%c", getchar(pyval));
            return;
        } else if (Py_val::is_none(pyval)){
            printf("None");
            return;
        }
    }

    switch(pyval->type){
    case py_type_float:
        printf("%f", getfloat(pyval));
        break;
    case py_type_newtuple:
    {
        Py_tuple * tuple = getnewtuple(pyval);
        uint tupsize = tuple->size();
        if (tupsize == 0){
            printf("()");
        } else if (tupsize == 1){
            printf("(");
            Py_val::print(tuple->get(0), bt, p);
            printf(",)");
        } else {
            printf("(");
            Py_val::print(tuple->get(0), bt, p);
            
            for(uint i = 1;i < tupsize;i++){
                printf(", ");
                Py_val::print(tuple->get(i), bt, p);
            }
            printf(")");
        }
    }
    break;
    case py_type_tuple:
    {
        vector<py_val_t> * tuple = gettuple(pyval);
        int tupsize = tuple->size();

        if (tupsize == 0){
            printf("()");
        } else if (tupsize == 1){
            printf("(");
            Py_val::print((*tuple)[0], bt, p);
            printf(",)");
        } else {
            printf("(");
            Py_val::print((*tuple)[0], bt, p);
            
            for(vector<py_val_t>::iterator itr = tuple->begin() + 1
                    ; itr != tuple->end(); itr++){
                printf(", ");
                Py_val::print(*itr, bt, p);
            }
            printf(")");
        }
    }
    break;
    case py_type_list:
    {
        vector<py_val_t> * list = getlist(pyval);
        printf("[");

        if (list->size() > 0){
            Py_val::print((*list)[0], bt, p);

            for(vector<py_val_t>::iterator itr = list->begin() + 1
                    ; itr != list->end(); itr++){
                printf(", ");
                Py_val::print(*itr, bt, p);
            }
        }
        printf("]");
    }
    break;
    case py_type_dict:
    {
        printf("{");
        vector<Py_dict_entry*> * dict_list = getdict(pyval);
        if (dict_list->size() > 0){
            Py_val::print((*dict_list)[0]->key, bt, p);
            printf(" : ");
            Py_val::print((*dict_list)[0]->val, bt, p);

            for(vector<Py_dict_entry*>::iterator itr = dict_list->begin() + 1
                    ; itr != dict_list->end(); itr++){
                printf(", ");
                Py_val::print((*itr)->key, bt, p);
                printf(" : ");
                Py_val::print((*itr)->val, bt, p);
            }
        }
        printf("}");
    }
    break;
    case py_type_newdict:
    {
        printf("{");
        py_val_t key;
        vector<py_val_t> * keys = getlist(getnewdict(pyval)->get_keys(bt, p));
        if (keys->size() > 0){
            Py_val::print(key = (*keys)[0], bt, p);
            printf(" : ");
            Py_val::print(getnewdict(pyval)->get(key, bt, p), bt, p);

            for(vector<py_val_t>::iterator itr = keys->begin() + 1
                    ; itr != keys->end(); itr++){
                printf(", ");
                Py_val::print(key = *itr, bt, p);
                printf(" : ");
                Py_val::print(getnewdict(pyval)->get(key, bt, p), bt, p);
            }
        }
        printf("}");
    }
    break;
    case py_type_string:
        printf("'%s'", getstring(pyval)->c_str());
        break;
    case py_type_ifun:
        printf("<function %s at %s:%d>", pyval->u.i->name->c_str()
               , pyval->u.i->b[0]->pos.filename.c_str()
               , pyval->u.i->b[0]->pos.line_no);
        break;
    case py_type_vm_ifun:
        printf("<function %s at %s:%d>", pyval->u.vm_i->name->c_str()
               , pyval->u.vm_i->pos.filename.c_str()
               , pyval->u.vm_i->pos.line_no);
        break;
    case py_type_nfun:
        printf("<function %s at 0x%x>"
               , pyval->u.n->name->c_str()
               , (uint)pyval->u.n->f);
        break;
    case py_type_thread:
        printf("<thread>");
        break;
    default:
        cerr << "never reaches here at " << __FILE__ << ":" << __LINE__; exit(1);
    }
}

/* 型チェック関数 */
bool Py_val::is_int(py_val_t pyval){
    return (bool)(((int)pyval) & 1);
}

bool Py_val::is_char(py_val_t pyval){
    return (bool)(!(((int)pyval) & 5) && (((int)pyval) & 2));
}

bool Py_val::is_none(py_val_t pyval){
    return pyval == py_val_none;
}

bool Py_val::is_float(py_val_t pyval){
    return is_boxed(pyval) && pyval->type == py_type_float;
}

bool Py_val::is_tuple(py_val_t pyval){
     return is_boxed(pyval) && pyval->type == py_type_tuple;
}

bool Py_val::is_list(py_val_t pyval){
    return is_boxed(pyval) && pyval->type == py_type_list;
}

bool Py_val::is_newtuple(py_val_t pyval){
    return is_boxed(pyval) && pyval->type == py_type_newtuple;
}

bool Py_val::is_dict(py_val_t pyval){
    return is_boxed(pyval) && pyval->type == py_type_dict;
}

bool Py_val::is_newdict(py_val_t pyval){
    return is_boxed(pyval) && pyval->type == py_type_newdict;
}

bool Py_val::is_string(py_val_t pyval){
    return is_boxed(pyval) && pyval->type == py_type_string;
}

bool Py_val::is_nfun(py_val_t pyval){
    return is_boxed(pyval) && pyval->type == py_type_nfun;
}

bool Py_val::is_ifun(py_val_t pyval){
    return is_boxed(pyval) && pyval->type == py_type_ifun;
}

bool Py_val::is_vfun(py_val_t pyval){
    return is_boxed(pyval) && pyval->type == py_type_vm_ifun;
}

bool Py_val::is_number(py_val_t pyval){
    return Py_val::is_int(pyval) || Py_val::is_float(pyval);
}

bool Py_val::is_thread(py_val_t pyval){
    return is_boxed(pyval) && pyval->type == py_type_thread;
}

bool Py_val::is_mutex(py_val_t pyval){
    return is_boxed(pyval) && pyval->type == py_type_mutex;
}

bool Py_val::is_cond(py_val_t pyval){
    return is_boxed(pyval) && pyval->type == py_type_cond;
}

bool Py_val::is_sockaddr(py_val_t pyval){
    return is_boxed(pyval) && pyval->type == py_type_sockaddr;
}


/* Py_va_tから値を取り出す関数 */
int Py_val::get_int(py_val_t pyval, ConsStack<Stack_trace_entry*> * bt, const SrcPos & pos){
    if (!Py_val::is_int(pyval)) {
        runtime_error(bt, pos, "type error (int)");
    }
    return ((int)pyval) >> 1;
}

char Py_val::get_char(py_val_t pyval, ConsStack<Stack_trace_entry*> * bt, const SrcPos & pos){
    if(!Py_val::is_char(pyval)){
        runtime_error(bt, pos, "type error (char)");
    }
    return (char)(((int)pyval) >> 3);
}

double Py_val::get_float(py_val_t pyval, ConsStack<Stack_trace_entry*> * bt, const SrcPos & pos){
    if (!Py_val::is_float(pyval)) {
        runtime_error(bt, pos, "type error (float)");
    }
    return pyval->u.f;
}

string * Py_val::get_string(py_val_t pyval, ConsStack<Stack_trace_entry*> * bt, const SrcPos & pos){
    if (!Py_val::is_string(pyval)) {
        runtime_error(bt, pos, "type error (string)");
    }

    return pyval->u.s; // 保留
}

vector<py_val_t> * Py_val::get_tuple(py_val_t pyval, ConsStack<Stack_trace_entry*> * bt, const SrcPos & pos){
    if (!Py_val::is_tuple(pyval)) {
        runtime_error(bt, pos, "type error (tuple)");
    }
    return pyval->u.l;
}

vector<py_val_t> * Py_val::get_list(py_val_t pyval, ConsStack<Stack_trace_entry*> * bt, const SrcPos & pos){
    if (!Py_val::is_list(pyval)) {
        runtime_error(bt, pos, "type error (list)");
    }
    return pyval->u.l;
}

Py_tuple * Py_val::get_newtuple(py_val_t pyval, ConsStack<Stack_trace_entry*> * bt, const SrcPos & pos){
    if (!Py_val::is_newtuple(pyval)) {
        runtime_error(bt, pos, "type error (newtuple)");
    }
    return pyval->u.nl;
}

vector<Py_dict_entry*> * Py_val::get_dict(py_val_t pyval, ConsStack<Stack_trace_entry*> * bt, const SrcPos & pos){
    if (!Py_val::is_dict(pyval)) {
        runtime_error(bt, pos, "type error (dictionary)");
    }

    return pyval->u.d;
}

symbol_t Py_val::get_ifun_name(py_val_t pyval, ConsStack<Stack_trace_entry*> * bt, const SrcPos & pos){
    if (!Py_val::is_ifun(pyval)) {
        runtime_error(bt, pos, "type error (identify function)");
    }
    return pyval->u.i->name;
}
symbol_t Py_val::get_nfun_name(py_val_t pyval, ConsStack<Stack_trace_entry*> * bt, const SrcPos & pos){
    if (!Py_val::is_nfun(pyval)) {
        runtime_error(bt, pos, "type error (native function)");
    }
    return pyval->u.n->name;
}

uint Py_val::hash(py_val_t pyval){
    if (Py_val::is_int(pyval)){
        return (uint)pyval;
    } else if (Py_val::is_char(pyval)){
        return (uint)pyval;
    } else if( Py_val::is_none(pyval)){
        return 0;
    }

    uint ret;
    vector<py_val_t> * l;
    Py_tuple * nl;
    
    switch(pyval->type){
    case py_type_float:
        ret = 11 * (uint) pyval->u.f;
        break;
    case py_type_string:
        ret = 17 * (uint)Symbol::get(*pyval->u.s);
        break;
    case py_type_list:
        l = pyval->u.l;
        switch(l->size()){
        case 0:
            ret = 53;
            break;
        case 1:
            ret = 53 * Py_val::hash((*l)[0]);
            break;
        case 2:
            ret = 53 * Py_val::hash((*l)[0]) + 101 * Py_val::hash((*l)[1]);
            break;
        default:
            ret = 53 * Py_val::hash((*l)[0]) + 101 * Py_val::hash((*l)[1])
                + 151 * Py_val::hash((*l)[2]);
        }
        break;
    case py_type_tuple:
        l = pyval->u.l;
        switch(l->size()){
        case 0:
            ret = 59;
            break;
        case 1:
            ret = 59 * Py_val::hash((*l)[0]);
            break;
        case 2:
            ret = 59 * Py_val::hash((*l)[0]) + 103 * Py_val::hash((*l)[1]);
            break;
        default:
            ret = 59 * Py_val::hash((*l)[0]) + 103 * Py_val::hash((*l)[1])
                + 157 * Py_val::hash((*l)[2]);
        }
        break;
    case py_type_newtuple:
        nl = pyval->u.nl;
        switch(nl->size()){
        case 0:
            ret = 59;
            break;
        case 1:
            ret = 59 * Py_val::hash(nl->get(0));
            break;
        case 2:
            ret = 59 * Py_val::hash(nl->get(0)) + 103 * Py_val::hash(nl->get(1));
            break;
        default:
            ret = 59 * Py_val::hash(nl->get(0)) + 103 * Py_val::hash(nl->get(1))
                + 157 * Py_val::hash(nl->get(2));
        }
        break;
    case py_type_dict:
        cerr << "obsolete program path at " << __FILE__ << ":" << __LINE__; exit(1);
        break;
    case py_type_newdict:
    {
        cerr << "not impletemted at " << __FILE__ << ":" << __LINE__; exit(1);
    }
        break;
    case py_type_ifun:
        ret = 19 * (uint)&(pyval->u.i->b);
        break;
    case py_type_nfun:
        ret = 23 * (uint) pyval->u.n->f;
        break;
    case py_type_vm_ifun:
        ret = 29 * (uint) pyval->u.vm_i->addr;
        break;
    default:
        cerr << "never reaches here at " << __FILE__ << ":" << __LINE__; exit(1);
        break;
    }

    return ret;
}

/* py_val構造体を割り当てる．型はtypeで示される */
py_val_t Py_val::alloc_boxed(py_type_t type){
    py_val_t py = new Py_val();
    py->type = type;
    return py;
}

//bool Py_val::is_boxed(py_val_t v) {
//}

/* Python値生成関数群 */
py_val_t Py_val::mk_int(int i){
    return (py_val_t)((i << 1) + 1);
}

py_val_t Py_val::mk_char(int c){
    return (py_val_t)((c << 3) + 2);
}

py_val_t Py_val::mk_none(){
    return py_val_none;
}

py_val_t Py_val::mk_float(double f){
    py_val_t r = alloc_boxed(py_type_float);
    r->u.f = f;
    return r;
}

py_val_t Py_val::mk_tuple(const vector<Py_val*> & l){
    py_val_t ret = Py_val::alloc_boxed(py_type_tuple);
    ret->u.l = new  vector<py_val_t>(l);
    return ret;
}

py_val_t Py_val::mk_list(const vector<Py_val*> & l){
    py_val_t ret = Py_val::alloc_boxed(py_type_list);
    ret->u.l = new vector<Py_val*>(l);
    return ret;
}

py_val_t Py_val::mk_newtuple(Py_tuple * l){
    py_val_t ret = Py_val::alloc_boxed(py_type_newtuple);
    ret->u.nl = l;
    return ret;
}

py_val_t Py_val::mk_tuple_byptr(vector<Py_val*> * l){
    py_val_t ret = Py_val::alloc_boxed(py_type_tuple);
    ret->u.l = l;
    return ret;
}

py_val_t Py_val::mk_list_byptr(vector<Py_val*> * l){
    py_val_t ret = Py_val::alloc_boxed(py_type_list);
    ret->u.l = l;
    return ret;
}

/* 辞書を作るための補助. 1エントリ(key : vl)を作る */

Py_dict_entry::Py_dict_entry(py_val_t key, py_val_t val){
    this->key = key;
    this->val = val;
}

/* 辞書を作る (dには格納されるべきキー : 値がが入っている) */
py_val_t Py_val::mk_dict(vector<Py_dict_entry*> & d){
    py_val_t ret = Py_val::alloc_boxed(py_type_dict);

    ret->u.d = new vector<Py_dict_entry*>(d);
    return ret;
}

py_val_t Py_val::mk_newdict(){
    py_val_t ret = Py_val::alloc_boxed(py_type_newdict);
    ret->u.nd = new Py_dict();
    return ret;
}

py_val_t Py_val::mk_newdict(vector<Py_dict_entry*> & d){
    py_val_t ret = Py_val::alloc_boxed(py_type_newdict);
    ret->u.nd = new Py_dict();
    ConsStack<Stack_trace_entry*> * bt; SrcPos p; // dummy
    for(vector<Py_dict_entry*>::iterator itr = d.begin();
        itr != d.end();itr++){
        ret->u.nd->set_entry(*itr, bt, p);
    }
    return ret;
}

/* 文字列を作る.
   0文字, 1文字の場合に注意 */
py_val_t Py_val::mk_string(const string& s){
    py_val_t ret;
    if (s.length() == 1){
        ret = Py_val::mk_char(s.at(0));
    } else {
        ret = Py_val::alloc_boxed(py_type_string);
        ret->u.s = new string(s);
    }
    return ret;
}

/* Python関数を作る */
py_val_t Py_val::mk_ifun(const symbol_t name, vector<symbol_t> ps, vector<Stm*> & b){
    py_val_t ret = Py_val::alloc_boxed(py_type_ifun);
    Py_ifun * ifun = new Py_ifun();
    ifun->name = name;
    ifun->ps = ps;
    ifun->b = b;
    
    ret->u.i = ifun;
    return ret;
}

/* Native関数を作る */
py_val_t Py_val::mk_nfun(const symbol_t name, int arity, C_fun_t f){
    Py_val * ret = Py_val::alloc_boxed(py_type_nfun);
    Py_nfun * nfun = new Py_nfun();
    nfun->name = name;
    nfun->arity = arity;
    nfun->f = f;

    ret->u.n = nfun;
    return ret;
}

py_val_t Py_val::mk_vm_ifun(const symbol_t name, int arity, int locals, int addr, SrcPos pos){
    py_val_t ret = Py_val::alloc_boxed(py_type_vm_ifun);
    Py_vm_ifun * vfun = new Py_vm_ifun();
    vfun->name = name;
    vfun->arity = arity;
    vfun->addr = addr;
    vfun->locals = locals;
    vfun->pos = pos;

    ret->u.vm_i = vfun;

    return ret;
}

py_val_t Py_val::mk_thread(py_val_t func){
    py_val_t ret;
    
    if (Py_val::is_ifun(func)
        || Py_val::is_vfun(func)
        || Py_val::is_nfun(func)){

        ret = Py_val::alloc_boxed(py_type_thread);
        Py_thread * th = new Py_thread(func);
        ret->u.th = th;

    } else {
        cerr << "never reaches here at " << __FILE__ << ":" << __LINE__; exit(1);
    }
    return ret;
}


py_val_t Py_val::mk_mutex(){
    py_val_t ret;

    ret = Py_val::alloc_boxed(py_type_mutex);
    Py_mutex * mutex = new Py_mutex();
    ret->u.mutex = mutex;

    return ret;
}


py_val_t Py_val::mk_cond(py_val_t mutex){
    py_val_t ret;

    ret = Py_val::alloc_boxed(py_type_cond);
    Py_cond * cond = new Py_cond(mutex);
    ret->u.cond = cond;
    
    return ret;
}

py_val_t Py_val::mk_sockaddr(py_val_t port){
    py_val_t ret;

    ret = Py_val::alloc_boxed(py_type_sockaddr);
    ret->u.addr = new sockaddr_in();
    ret->u.addr->sin_family = AF_INET;
    ret->u.addr->sin_addr.s_addr = htonl(INADDR_ANY);
    ret->u.addr->sin_port = htons(getint(port));
    
    return ret;
}


/*
 * 辞書式!
 */

Py_dict::Py_dict(){
    this->pykeys = Py_val::alloc_boxed(py_type_list);
    this->pyvalues = Py_val::alloc_boxed(py_type_list);
    
    this->pykeys->u.l = &this->keys;
    this->pyvalues->u.l = &this->values;

    this->size_ = 0;
}

Py_dict_entry * Py_dict::get_entry(py_val_t key){
    int index = Py_val::hash(key) % PY_DICT_TABLE_SIZE;

    if (this->dict[index] == NULL){
        return NULL;
    }

    vector<Py_dict_entry *> * chain = this->dict[index];

    for(vector<Py_dict_entry *>::iterator itr = chain->begin(); itr != chain->end(); itr++){
        if (Py_val::compare(key, (*itr)->key) == 0){
            return *itr;
        }
    }

    return NULL;
}

void Py_dict::add_key(py_val_t key){
    // ヒープを使ってソートされた状態で格納できると嬉しいね
    this->size_++;
    this->keys.push_back(key);
}

void Py_dict::add_value(py_val_t value){
    // ヒープを使ってソートされた状態で格納できると嬉しいね
    this->values.push_back(value);
}

void Py_dict::del_key(py_val_t key){
    vector<py_val_t>::iterator itr;
    this->size_--;
    for(itr = this->keys.begin();itr != this->keys.end();itr++){
        if (Py_val::compare(key, *itr) == 0) {
            this->keys.erase(itr);
            return;
        }
    }
    cerr << "never reaches here at " << __FILE__ << ":" << __LINE__; exit(1);
}

void Py_dict::del_value(py_val_t value){
    vector<py_val_t>::iterator itr;
    for(itr = this->values.begin();itr != this->values.end();itr++){
        if (Py_val::compare(value, *itr) == 0) {
            this->values.erase(itr);
            return;
        }
    }
    cerr << "never reaches here at " << __FILE__ << ":" << __LINE__; exit(1);
}

py_val_t Py_dict::set(py_val_t key, py_val_t val, ConsStack<Stack_trace_entry*> * bt, const SrcPos & p){
    uint hash = Py_val::hash(key);
    vector<Py_dict_entry *> * chain;
    if (this->dict[hash % PY_DICT_TABLE_SIZE] == NULL){ // chain doesn't exist
        chain = this->dict[hash % PY_DICT_TABLE_SIZE] = new vector<Py_dict_entry *>();
    } else {
        chain = this->dict[hash % PY_DICT_TABLE_SIZE];
        // cerr << "collision" << endl;
    }

    for(vector<Py_dict_entry *>::iterator itr = chain->begin(); itr != chain->end(); itr++){
        if (Py_val::compare(key, (*itr)->key) == 0){
            this->del_value((*itr)->val);
            this->add_value(val);
            (*itr)->val = val;
            return mkint(0);
        }
    }
    
    chain->push_back(new Py_dict_entry(key, val));
    this->add_key(key);
    this->add_value(val);
    
    return mkint(1);
}

py_val_t Py_dict::set_entry(Py_dict_entry * entry, ConsStack<Stack_trace_entry*> * bt, const SrcPos & p){
    int hash = Py_val::hash(entry->key);
    vector<Py_dict_entry *> * chain;
    if (this->dict[hash % PY_DICT_TABLE_SIZE] == NULL){ // chain doesn't exist
        chain = this->dict[hash % PY_DICT_TABLE_SIZE] = new vector<Py_dict_entry *>();
    } else {
        // cerr << "collision" << endl;
        chain = this->dict[hash % PY_DICT_TABLE_SIZE];
    }

    for(vector<Py_dict_entry *>::iterator itr = chain->begin(); itr != chain->end(); itr++){
        if (Py_val::compare(entry->key, (*itr)->key) == 0){
            this->del_value((*itr)->val);
            this->add_value(entry->val);
            (*itr)->val = entry->val;
            return mkint(0);
        }
    }
    
    chain->push_back(entry);
    this->add_key(entry->key);
    this->add_value(entry->val);
    
    return mkint(1);
}

py_val_t Py_dict::get(py_val_t key, ConsStack<Stack_trace_entry*> * bt, const SrcPos & p){
    Py_dict_entry * entry;
    if ((entry = this->get_entry(key)) == NULL){
        stringstream ss;
        Py_val::to_str(ss, key, bt, p);
        runtime_error(bt, p, "getitem : key doesn't exist: " + ss.str());
    } else {
        return entry->val;
    }
    
    return mkint(0);
}

py_val_t Py_dict::del(py_val_t key, ConsStack<Stack_trace_entry*> * bt, const SrcPos & p){
    int hash = Py_val::hash(key);
    
    vector<Py_dict_entry *> * chain;
    if (this->dict[hash % PY_DICT_TABLE_SIZE] == NULL){ // chain doesn't exist
        stringstream ss;
        Py_val::to_str(ss, key, bt, p);
        runtime_error(bt, p, "delitem : key doesn't exist: " + ss.str());
    }

    chain = this->dict[hash % PY_DICT_TABLE_SIZE];

    for(vector<Py_dict_entry *>::iterator itr = chain->begin(); itr != chain->end(); itr++){
        if (Py_val::compare(key, (*itr)->key) == 0){
            this->del_key(key);
            this->del_value((*itr)->val);
            chain->erase(itr);
            return mkint(1);
        }
    }

    cerr << "never reaches here at " << __FILE__ << ":" << __LINE__; exit(1);
    return mkint(0);
}

py_val_t Py_dict::get_values(ConsStack<Stack_trace_entry*> * bt, const SrcPos & p){
    return this->pyvalues;
}

py_val_t Py_dict::get_keys(ConsStack<Stack_trace_entry*> * bt, const SrcPos & p){
    return this->pykeys;
}

py_val_t Py_dict::has_key(py_val_t key, ConsStack<Stack_trace_entry*> * bt, const SrcPos & p){
    py_val_t ret;
    if (this->get_entry(key) != NULL){
        ret = mkint(1);
    } else {
        ret = mkint(0);
    }
    
    return ret;
}

int Py_dict::size(){
    return this->size_;
}



Py_tuple::Py_tuple(uint capacity_){
    this->capacity = capacity_;
    this->size_ = 0;
    this->vec = new py_val_t[this->capacity];
}

Py_tuple::~Py_tuple(){
    delete[](this->vec);
}

void Py_tuple::push_back(py_val_t val){
    if (this->size_ == this->capacity){
        cerr << "Py_tuple : tuple overflow" << endl;
    }

    this->vec[this->size_] = val;
    this->size_++;
}

void Py_tuple::append(Py_tuple * tup){
    for(uint i = 0; i < tup->size();i++){
        this->vec[this->size_++] = tup->get(i);
    }
}

py_val_t Py_tuple::get(unsigned int index){
    if (index >= this->size_){
        cerr << "Py_tuple : index out of bounds" << endl;
        exit(1);
    }
    return this->vec[index];
}

unsigned int Py_tuple::size(){
    return this->size_;
}

Py_thread::Py_thread(py_val_t func){
    this->func = func;
    this->joined = false;

    // pthread_mutex_t mt = PTHREAD_MUTEX_INITIALIZER;
//     this->join_mutex = mt;
//     pthread_cond_t cnd = PTHREAD_COND_INITIALIZER;
//     this->join_cond = cnd;

    PTH_ASSERT(pthread_mutex_init(&this->join_mutex, NULL));
    PTH_ASSERT(pthread_cond_init(&this->join_cond, NULL));
}

Py_thread::~Py_thread(){
    
}

Py_mutex::Py_mutex(){
    pthread_mutex_init(&this->mutex, NULL);
}

Py_cond::Py_cond(py_val_t mutex){
    this->py_mutex = mutex;
    pthread_cond_init(&this->cond, NULL);
}

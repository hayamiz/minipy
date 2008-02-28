#include "native.hpp"
// dictのキー：値リストって何返せばいいんだろう。

py_val_t native::is_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                        py_val_t * a){
    return Py_val::mk_int(Py_val::is_int(a[0]));
}

py_val_t native::is_float(stack<Stack_trace_entry> & bt,const SrcPos & p,
                         py_val_t * a){
    return Py_val::mk_int(Py_val::is_float(a[0]));
}

py_val_t native::is_string(stack<Stack_trace_entry> & bt,const SrcPos & p,
                          py_val_t * a){
    return Py_val::mk_int(Py_val::is_char(a[0]) || Py_val::is_string(a[0]));
}

py_val_t native::is_dict(stack<Stack_trace_entry> & bt,const SrcPos & p,
                         py_val_t * a){
    return Py_val::mk_int(Py_val::is_dict(a[0]));
}

py_val_t native::is_list(stack<Stack_trace_entry> & bt,const SrcPos & p,
                         py_val_t * a){
    return Py_val::mk_int(Py_val::is_list(a[0]));
}

py_val_t native::is_tuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                          py_val_t * a){
    return Py_val::mk_int(Py_val::is_tuple(a[0]));
}

py_val_t native::is_ifun(stack<Stack_trace_entry> & bt,const SrcPos & p,
                         py_val_t * a){
    return Py_val::mk_int(Py_val::is_ifun(a[0]));
}

py_val_t native::is_nfun(stack<Stack_trace_entry> & bt,const SrcPos & p,
                         py_val_t * a){
    return Py_val::mk_int(Py_val::is_nfun(a[0]));
}


/* 整数同士の演算に限定した各種演算子 */
py_val_t native::add_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                        py_val_t * a){     // +
    return Py_val::mk_int(getint(a[0]) + getint(a[1]));
}

py_val_t native::sub_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                        py_val_t * a){     // -
    return Py_val::mk_int(getint(a[0]) - getint(a[1]));
}
    
py_val_t native::mul_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                        py_val_t * a){     // *
    return Py_val::mk_int(getint(a[0]) * getint(a[1]));
}

py_val_t native::div_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                         py_val_t * a){     // /
    if(getint(a[1]) == 0){
        runtime_error(bt, p,
                      "ZeroDivisionError: integer modulo by zero");
    }
    return Py_val::mk_int((int)(getint(a[0]) / getint(a[1])));
}

py_val_t native::mod_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                        py_val_t * a){     // %
    if(Py_val::get_int(a[1],bt,p) == 0){
        runtime_error(bt, p,
                      "ZeroDivisionError: integer division by zero");
    }
    return Py_val::mk_int(Py_val::get_int(a[0],bt,p)
                          % Py_val::get_int(a[1],bt,p));
}

py_val_t native::invert_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                            py_val_t * a){  // ~
    return Py_val::mk_int((-1 * Py_val::get_int(a[0],bt,p))-1);
}

py_val_t native::and_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                         py_val_t * a){     // &
    return Py_val::mk_int(Py_val::get_int(a[0],bt,p)
                          &Py_val::get_int(a[1],bt,p));
}

py_val_t native::or_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                       py_val_t * a){      // |
    return Py_val::mk_int(Py_val::get_int(a[0],bt,p)
                          | Py_val::get_int(a[1],bt,p));
}

py_val_t native::xor_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                        py_val_t * a){     // ^
    return Py_val::mk_int((int)(Py_val::get_int(a[0],bt,p)
                                ^ Py_val::get_int(a[1],bt,p)));
}

py_val_t native::lshift_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                           py_val_t * a){  // <<
    return Py_val::mk_int((int)(Py_val::get_int(a[0],bt,p)
                                << Py_val::get_int(a[1],bt,p)));
}

py_val_t native::rshift_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                           py_val_t * a){  // >>
    return Py_val::mk_int((int)(Py_val::get_int(a[0],bt,p)
                                >> Py_val::get_int(a[1],bt,p)));
}


py_val_t native::prefix_sub(stack<Stack_trace_entry> & bt,const SrcPos & p,
                            py_val_t * a){  // +や-のprefix処理
    if (Py_val::is_int(a[0])){
        return Py_val::mk_int(getint(a[0]) * -1);
    } else if (Py_val::is_float(a[0])){
        return Py_val::mk_float(getfloat(a[0]) * -1);
    }

    runtime_error(bt, p , "type error : integer or float required.");
    return NULL;
}


/* 浮動小数点数同士の演算に限定した各種演算 */
py_val_t native::add_float(stack<Stack_trace_entry> & bt,const SrcPos & p,
                          py_val_t * a){     // +
    return Py_val::mk_float(getfloat(a[0]) + getfloat(a[1]));
}

py_val_t native::sub_float(stack<Stack_trace_entry> & bt,const SrcPos & p,
                          py_val_t * a){     // -
    return Py_val::mk_float(getfloat(a[0]) - getfloat(a[1]));
}

py_val_t native::mul_float(stack<Stack_trace_entry> & bt,const SrcPos & p,
                          py_val_t * a){     // *
    return Py_val::mk_float(getfloat(a[0]) * getfloat(a[1]));
}

py_val_t native::div_float(stack<Stack_trace_entry> & bt,const SrcPos & p,
                          py_val_t * a){     // /
    if(getfloat(a[1]) == 0.0){
        //error:div0
        runtime_error(bt, p, "ZeroDivisionError: float division");
    }
    return Py_val::mk_float(getfloat(a[0]) / getfloat(a[1]));
}

/* 文字列の演算子 */
py_val_t native::len_string(stack<Stack_trace_entry> & bt,const SrcPos & p,
                            py_val_t * a){ // 文字列の長さ
    if (Py_val::is_char(a[0])){
        return Py_val::mk_int(1);
    } else if (Py_val::is_string(a[0])){
        return Py_val::mk_int(getstring(a[0])->length());
    }
    runtime_error(bt, p, "type error");
    return NULL;
}

py_val_t native::getitem_string(stack<Stack_trace_entry> & bt,const SrcPos & p,
                                py_val_t * a){ // sのi番めの要素
    // string はstring[0]から始まる.iは-size〜size-1とする
    // Python氏は-1とかいうインデックスを後ろから１番めのものを返すぽい

    string * tmp_str = NULL;
    int num = Py_val::get_int(a[1],bt,p);
    int size;

    if (Py_val::is_char(a[0])){
        size = 1;
    } else {
        tmp_str = Py_val::get_string(a[0],bt,p);
        size = tmp_str->size();
    }
    
    if(size <= num || size+num <0){
        //error:
        runtime_error(bt, p, "IndexError: string index out of range");
    }

    py_val_t ret;

    if (tmp_str == NULL) { // if char
        ret = a[0];
    } else {
        if(num >= 0){ // 0 to size-1
            ret = Py_val::mk_char((*tmp_str)[num]);
        }else{ // -1 to -(size)
            ret = Py_val::mk_char((*tmp_str)[size+num]);
        }
    }

    return ret;
}

py_val_t native::add_string(stack<Stack_trace_entry> & bt,const SrcPos & p,
                            py_val_t * a){ //sとtの結合
    stringstream ss;
    if (Py_val::is_char(a[0])){
        ss.put(Py_val::get_char(a[0],bt,p));
    } else if (Py_val::is_string(a[0])) {
        ss << *Py_val::get_string(a[0],bt,p);
    }

    if (Py_val::is_char(a[1])){
        ss.put(Py_val::get_char(a[1],bt,p));
    } else if (Py_val::is_string(a[1])) {
        ss << *Py_val::get_string(a[1],bt,p);
    }
    
    return  Py_val::mk_string(ss.str());
}

py_val_t native::mul_string(stack<Stack_trace_entry> & bt,const SrcPos & p,
                            py_val_t * a){ //sとtの結合
    stringstream ss;
    if (!( ( (Py_val::is_string(a[0]) || Py_val::is_char(a[0]))
             && Py_val::is_int(a[1]))
           || ( (Py_val::is_string(a[0]) || Py_val::is_char(a[0]))
                && Py_val::is_int(a[0])))){
        runtime_error(bt, p, "type error");
    }

    int times;
    string * str;
    if (Py_val::is_string(a[0])){
        str = Py_val::get_string(a[0], bt, p);
        times = getint(a[1]);
    } else if (Py_val::is_char(a[0])){
        return Py_val::mk_string(string(getint(a[1])
                                        , Py_val::get_char(a[0], bt, p)));
    } else {
        times = getint(a[0]);
        str = Py_val::get_string(a[1], bt, p);
    }

    for(int i = 0;i < times; i++){
        ss << *str;
    }

    return Py_val::mk_string(ss.str());
}

py_val_t native::eq_string(stack<Stack_trace_entry> & bt,const SrcPos & p,
                   py_val_t * a){
    if (Py_val::is_char(a[0])){
        if (Py_val::is_char(a[1])){
            return Py_val::mk_int(a[0] == a[1]);
        } else if (Py_val::is_string(a[1])){
            return Py_val::mk_int(Py_val::get_char(a[0], bt, p)
                                 == (*Py_val::get_string(a[1], bt, p))[0]);
        } else {
            runtime_error(bt, p, "type error");
        }
    } else if (Py_val::is_string(a[0])){
        if (Py_val::is_char(a[1])){
            return Py_val::mk_int(Py_val::get_char(a[1], bt, p)
                                 == (*Py_val::get_string(a[0], bt, p))[0]);
        } else if (Py_val::is_string(a[1])){
            return Py_val::mk_int(
                Py_val::get_string(a[0], bt, p)->compare(
                    (*Py_val::get_string(a[1], bt, p))) == 0);
        } else {
            runtime_error(bt, p, "type error");
        }
    }
    runtime_error(bt, p, "type error");
    return NULL;
}
    
/* タプルの演算子 */
py_val_t native::len_tuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                           py_val_t * a){ // タプルの長さ
    return  Py_val::mk_int(Py_val::get_tuple(a[0],bt,p)->size());
}

py_val_t native::getitem_tuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                               py_val_t * a){ // tのi番めの要素
    // tupleもstringに同じ
    vector<py_val_t> * tmp_vec = Py_val::get_tuple(a[0],bt,p);
    int num = Py_val::get_int(a[1],bt,p);
    int size = tmp_vec->size();
    if(size <= num || size+num <0){
        //error:
        runtime_error(bt, p, "IndexError: tuple index out of range");
    }

    py_val_t ret;
    if(num >= 0){ // 0 to size-1
        ret = (*tmp_vec)[num];
    }else{ // -1 to -(size)
        ret = (*tmp_vec)[size+num];
    }

    return ret;
}

py_val_t native::add_tuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                           py_val_t * a){ //tとuの結合
    vector<py_val_t> * tmp_t = Py_val::get_tuple(a[0],bt,p);
    vector<py_val_t> * tmp_u = Py_val::get_tuple(a[1],bt,p);
    vector<py_val_t> * newtuple = new vector<py_val_t>();
    newtuple->insert(newtuple->end(), tmp_t->begin(), tmp_t->end());
    newtuple->insert(newtuple->end(), tmp_u->begin(), tmp_u->end());
    return Py_val::mk_tuple_byptr(newtuple);
}

py_val_t native::mul_tuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                           py_val_t * a){
    vector<py_val_t> * tup1 = Py_val::get_tuple(a[0],bt,p);
    vector<py_val_t> * tup_ret = new vector<py_val_t>;
    int mul_times = Py_val::get_int(a[1],bt,p);
    
    for(int i = 0; i < mul_times; i++){
        tup_ret->insert(tup_ret->end(), tup1->begin(), tup1->end());
    }
    return  Py_val::mk_tuple_byptr(tup_ret);
}



py_val_t native::hasitem_tuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                       py_val_t * a){ //kがtにあるか？
    vector<py_val_t>::iterator it;
    vector<py_val_t> * tmp_t = Py_val::get_tuple(a[0],bt,p);

    it = tmp_t->begin();
    while (it < tmp_t->end()){
        if(*it == a[1] ){
            return  Py_val::mk_int(1);
        }else{
            it++;
        }
    }
    return Py_val::mk_int(0);
}


py_val_t native::len_newtuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                              py_val_t * a){ // タプルの長さ
    return mkint(a[0]->u.nl->size());
}
py_val_t native::getitem_newtuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                                  py_val_t * a){ // tのi番めの要素
    // tupleもstringに同じ
    Py_tuple * tup = getnewtuple(a[0]);
    int num = getint(a[1]);
    int size = tup->size();
    if(size <= num || size+num <0){
        //error:
        runtime_error(bt, p, "IndexError: newtuple index out of range");
    }
    
    py_val_t ret;
    if(num >= 0){ // 0 to size-1
        ret = tup->get(num);
    }else{ // -1 to -(size)
        ret = tup->get(size+num);
    }

    return ret;
}
py_val_t native::add_newtuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                              py_val_t * a){ //tとuの結合
    Py_tuple * tup1 = getnewtuple(a[0]);
    Py_tuple * tup2 = getnewtuple(a[1]);
    Py_tuple * tup = new Py_tuple(tup1->size() + tup2->size());

    tup->append(tup1); tup->append(tup2);
    return Py_val::mk_newtuple(tup);
}
py_val_t native::mul_newtuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a); //t*number

py_val_t native::hasitem_newtuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                                  py_val_t * a){ //kがtにあるか？a=(t,k)
    Py_tuple * tup = getnewtuple(a[0]);

    for (uint i = 0;i < tup->size();i++){
        if (Py_val::compare(tup->get(i), a[1]) == 0){
            return mkint(1);
        }
    }

    return Py_val::mk_int(0);
}


/* リストの演算子 */
py_val_t native::len_list(stack<Stack_trace_entry> & bt,const SrcPos & p,
                          py_val_t * a){ // リストの長さ
    return  Py_val::mk_int(Py_val::get_list(a[0],bt,p)->size());
}

py_val_t native::getitem_list(stack<Stack_trace_entry> & bt,const SrcPos & p,
                              py_val_t * a){ // lのi番めの要素
    // listもstringに同じ
    vector<py_val_t> * tmp_vec = Py_val::get_list(a[0],bt,p);
    int num = Py_val::get_int(a[1],bt,p);
    int size = tmp_vec->size();
    if(size <= num || size+num < 0){
        //error:
        runtime_error(bt, p, "IndexError: list index out of range");
    }

    py_val_t ret;
    if(num >= 0){ // 0 to size-1
        ret = (*tmp_vec)[num];
    }else{ // -1 to -(size)
        ret = (*tmp_vec)[size+num];
    }

    return ret;
}

py_val_t native::setitem_list(stack<Stack_trace_entry> & bt,const SrcPos & p,
                              py_val_t * a){ //lのi番目をxにする
// listもstringに同じ // l/i/xの順
    vector<py_val_t> * tmp_l = Py_val::get_list(a[0],bt,p);
    int num = Py_val::get_int(a[1],bt,p);
    int size = tmp_l->size();
     
    if(size <= num || size+num <0){
        //error:
        runtime_error(bt, p, "IndexError: list index out of range");
    }

    if(num >= 0){ // 0 to size-1
        tmp_l->erase ( tmp_l->begin() + num);
        tmp_l->insert( tmp_l->begin() + num ,a[2]);

    }else{ // -1 to -(size)
        tmp_l->erase ( tmp_l->begin() + num +size);
        tmp_l->insert( tmp_l->begin() + num +size  ,a[2]);
    }

    return Py_val::mk_list_byptr(tmp_l);
}

py_val_t native::delitem_list(stack<Stack_trace_entry> & bt,const SrcPos & p,
                              py_val_t * a){ //lのi番目を削除
    // listもstringに同じ
    vector<py_val_t> * tmp_l = Py_val::get_list(a[0],bt,p);
    int num = Py_val::get_int(a[1],bt,p);
    int size = tmp_l->size();
    if(size <= num || size+num <0){
        //error:
        runtime_error(bt, p, "IndexError: list index out of range");
    }

    if(num >= 0){ // 0 to size-1
        tmp_l->erase ( tmp_l->begin() + num);
    }else{ // -1 to -(size)
        tmp_l->erase ( tmp_l->begin() + num +size);
    }

    return Py_val::mk_list_byptr(tmp_l);
}

py_val_t native::pop_list(stack<Stack_trace_entry> & bt,const SrcPos & p,
                          py_val_t * a){ //lのi番目を削除＆値を返す
    // listもstringに同じ
    vector<py_val_t> * tmp_l = Py_val::get_list(a[0],bt,p);
    int num = Py_val::get_int(a[1],bt,p);
    int size = tmp_l->size();
    if(size <= num || size+num <0){
        //error:
        runtime_error(bt, p, "IndexError: list index out of range");
    }

    py_val_t tmp_item;
    if(num >= 0){ // 0 to size-1
        tmp_item =  (*tmp_l)[num];
        tmp_l->erase ( tmp_l->begin() + num);
    }else{ // -1 to -(size)
        tmp_item =  (*tmp_l)[num+size];
        tmp_l->erase (tmp_l->begin() + num +size);
    }

    return tmp_item;
}

py_val_t native::hasitem_list(stack<Stack_trace_entry> & bt,const SrcPos & p,
                       py_val_t * a){ //kがtにあるか？
    vector<py_val_t>::iterator it;
    vector<py_val_t> * tmp_l = Py_val::get_list(a[0],bt,p);

    it = tmp_l->begin();
    while (it < tmp_l->end()){
        if(*it == a[1] ){
            return  Py_val::mk_int(1);
        }else{
            it++;
        }
    }
    return Py_val::mk_int(0);
}

py_val_t native::mul_list(stack<Stack_trace_entry> & bt,const SrcPos & p,
                       py_val_t * a){ //l*number
    vector<py_val_t> * list1 = Py_val::get_list(a[0],bt,p);
    vector<py_val_t> * list_ret = new vector<py_val_t>();
    int mul_times = Py_val::get_int(a[1],bt,p);
    
    for(int i = 0; i < mul_times; i++){
        list_ret->insert(list_ret->end(), list1->begin(), list1->end());
    }

    return  Py_val::mk_list_byptr(list_ret);
}


/* 辞書の演算子 */
py_val_t native::len_dict(stack<Stack_trace_entry> & bt,const SrcPos & p,
                          py_val_t * a){ //dの要素数
    return  Py_val::mk_int(a[0]->u.nd->size());
}

py_val_t native::has_key(stack<Stack_trace_entry> & bt,const SrcPos & p,
                         py_val_t * a){ // Keyがdにq存在するか
    if (!Py_val::is_newdict(a[0])){
        runtime_error(bt,p, "type error : has_key requires dictionary");
    }

    return getnewdict(a[0])->has_key(a[1], bt, p);
}

py_val_t native::getitem_dict(stack<Stack_trace_entry> & bt,const SrcPos & p,
                              py_val_t * a){ // Keyに対する値
    if (!Py_val::is_dict(a[0])){
        runtime_error(bt,p, "type error : getitem_dict requires dictionary");
    }
    return a[0]->u.nd->get(a[1],bt,p);

//     vector<Py_dict_entry*>::iterator it;
//     vector<Py_dict_entry*> * tmp_d = Py_val::get_dict(a[0],bt,p);

//     py_val_t args[3];

//     for(it = tmp_d->begin(); it != tmp_d->end();it++){
//         args[0] = (*it)->key;
//         args[1] = a[1];
//         args[2] = NULL;
//         if( Py_val::mk_int(1) == native::eq(bt, p, args) ){
//             return (*it)->val;
//         }
//     }

//     runtime_error(bt,p, "key error : not found");
//     return NULL;
}

py_val_t native::setitem_dict(stack<Stack_trace_entry> & bt,const SrcPos & p,
                              py_val_t * a){ 
//     // Keyに対する値をxに
     if (!Py_val::is_newdict(a[0])){
         runtime_error(bt,p, "type error : setitem_dict requires dictionary");
     }

     a[0]->u.nd->set(a[1],a[2],bt,p);
     return a[0];
     
//     vector<Py_dict_entry*>::iterator it;
//     vector<Py_dict_entry*> * tmp_d = Py_val::get_dict(a[0],bt,p);
//     py_val_t args[3];

//     for(it = tmp_d->begin(); it != tmp_d->end();it++){
//         args[0] = (*it)->key;
//         args[1] = a[1];
//         args[2] = NULL;
//         if( Py_val::mk_int(1) == native::eq(bt, p, args) ){
//             (*it)->val = a[2];
//             return a[0];
//         }
//     }

//     Py_dict_entry * new_entry = new Py_dict_entry(a[1], a[2]);
//     tmp_d->push_back(new_entry);

//     return a[0];
}

py_val_t native::delitem_dict(stack<Stack_trace_entry> & bt,const SrcPos & p,
                              py_val_t * a){ //Keyに対する値を削除
    if (!Py_val::is_newdict(a[0])){
        runtime_error(bt,p, "type error : setitem_dict requires dictionary");
    }
    return a[0]->u.nd->del(a[1],bt,p);


//     vector<Py_dict_entry*>::iterator it;
//     vector<Py_dict_entry*> * tmp_d = Py_val::get_dict(a[0],bt,p);
//     py_val_t args[3];

//     for(it = tmp_d->begin(); it != tmp_d->end();it++){
//         args[0] = (*it)->key;
//         args[1] = a[1];
//         args[2] = NULL;
//         if( Py_val::mk_int(1) == native::eq(bt, p, args) ){
//             delete(*it);
//             tmp_d->erase(it);
//             return a[0];
//         }
//     }
//     return Py_val::mk_int(0);
}

py_val_t native::keys(stack<Stack_trace_entry> & bt,const SrcPos & p,
                          py_val_t * a){ // すべてのKeyリスト
//     vector<Py_dict_entry*> * tmp_d = Py_val::get_dict(a[0],bt,p);
//     vector<py_val_t> tmp_l;

//     for(int i = 0;i < (int)tmp_d->size();i++){
//         tmp_l.push_back((*tmp_d)[i]->key);
//     }
//     return Py_val::mk_list(tmp_l);
    return getnewdict(a[0])->get_keys(bt, p);
}

py_val_t native::values(stack<Stack_trace_entry> & bt,const SrcPos & p,
                             py_val_t * a){ // すべての値リスト
//     vector<Py_dict_entry*> * tmp_d = Py_val::get_dict(a[0],bt,p);
//     vector<py_val_t> tmp_l;

//     for(int i = 0;i < (int)tmp_d->size();i++){
//         tmp_l.push_back((*tmp_d)[i]->val);
//     }
//     return Py_val::mk_list(tmp_l);
    return getnewdict(a[0])->get_values(bt, p);
}

py_val_t native::items(stack<Stack_trace_entry> & bt,const SrcPos & p,
                            py_val_t * a){ // すべてのKey:値のリスト
    vector<py_val_t>::iterator it;
    vector<py_val_t> * keys = getlist(getnewdict(a[0])->get_keys(bt, p));
    vector<py_val_t> * tup;
    vector<py_val_t> * l = new vector<py_val_t>();
    
    for(it = keys->begin(); it < keys->end(); it++){
        tup = new vector<py_val_t>();
        tup->push_back((*it));
        tup->push_back(getnewdict(a[0])->get((*it), bt, p));
        l->push_back(Py_val::mk_tuple_byptr(tup));
    } 
    return Py_val::mk_list_byptr(l);
}


/* 各種値を文字列に変換する関数群 */
py_val_t native::repr_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                          py_val_t * a){
    char tmp[10];
    sprintf(tmp, "%d", Py_val::get_int(a[0],bt,p));
    return Py_val::mk_string(tmp);
}

py_val_t native::repr_float(stack<Stack_trace_entry> & bt,const SrcPos & p,
                            py_val_t * a){
    //静的嫌がらせを受けている気がする…
    ostringstream os;
    os << Py_val::get_float(a[0],bt,p);
    return Py_val::mk_string(os.str());
}

py_val_t native::repr_ifun(stack<Stack_trace_entry> & bt,const SrcPos & p,
                           py_val_t * a){
    return Py_val::mk_string( *Py_val::get_ifun_name(a[0],bt,p));
}

py_val_t native::repr_nfun(stack<Stack_trace_entry> & bt,const SrcPos & p,
                           py_val_t * a){
    return Py_val::mk_string( *Py_val::get_nfun_name(a[0],bt,p));
}


py_val_t native::print_(stack<Stack_trace_entry> & bt, const SrcPos & p,
                        py_val_t * a){
    stringstream ss;
    if (Py_val::is_string(a[0])){
        cout << *Py_val::get_string(a[0], bt, p) << endl;
    } else {
        Py_val::print(a[0], bt, p);
        printf("\n");
        // Py_val::to_str(cout, a[0], bt, p);
        // cout << ss.str() << endl;
    }
    return Py_val::mk_none();
}


py_val_t native::dprint(stack<Stack_trace_entry> & bt, const SrcPos & p,
                        py_val_t * a){
    stringstream ss;
    if (Py_val::is_string(a[0])){
        cerr << *Py_val::get_string(a[0], bt, p);
#ifndef RELEASE_BUILD
        cerr << "\t--" << *SrcFiles::getline(p.filename, p.line_no);
#endif
        cerr << endl;
    } else {
        Py_val::to_str(ss, a[0], bt, p);
        cerr << ss.str();
#ifndef RELEASE_BUILD
        cerr << "\t--" << *SrcFiles::getline(p.filename, p.line_no);
#endif
        cerr << endl;
    }
    return Py_val::mk_none();
}

py_val_t native::print_string(stack<Stack_trace_entry> & bt
                              , const SrcPos & p, py_val_t * a){
    if (Py_val::is_string(a[0])){
        cout << *Py_val::get_string(a[0], bt, p);
    } else if (Py_val::is_char(a[0])){
        cout.put(Py_val::get_char(a[0], bt, p));
    } else {
        runtime_error(bt, p , "type error : string required.");
    }
    return Py_val::mk_none();
}

/*-------------- 演算子 ---------------*/
py_val_t native::add(stack<Stack_trace_entry> & bt, const SrcPos & p,
                     py_val_t * a){
    if(Py_val::is_int(a[0])){
        if(Py_val::is_int(a[1])){
            return add_int(bt,p,a);
        }else if(Py_val::is_float(a[1])){
            a[0] = Py_val::mk_float((double)getint(a[0]));
            return native::add_float(bt,p,a);
        }
    }else if(Py_val::is_float(a[0])){
        if(Py_val::is_int(a[1])){
            a[1]=Py_val::mk_float((double)getint(a[1]));
            return native::add_float(bt,p,a);
        }else if(Py_val::is_float(a[1])){
            return add_float(bt,p,a);
        }
    }else if(Py_val::is_char(a[0])){
        stringstream ss0;
        ss0.put(Py_val::get_char(a[0], bt, p));
        a[0] = Py_val::mk_string(ss0.str());
        if(Py_val::is_string(a[1])){
            return native::add_string(bt,p,a);
        }else if(Py_val::is_char(a[1])){
            stringstream ss;
            ss.put(Py_val::get_char(a[1], bt, p));
            a[1] = Py_val::mk_string(ss.str());
            return native::add_string(bt,p,a);
        }
    }else if(Py_val::is_string(a[0])){
        if(Py_val::is_string(a[1])){
            return native::add_string(bt,p,a);
        }else if(Py_val::is_char(a[1])){
            stringstream ss;
            ss.put(Py_val::get_char(a[1], bt, p));
            a[1] = Py_val::mk_string(ss.str());
            return native::add_string(bt,p,a);
        }
    }else if(Py_val::is_list(a[0]) && Py_val::is_list(a[1])){
        return native::add_list(bt,p,a);
    }else if(Py_val::is_tuple(a[0]) && Py_val::is_tuple(a[1])){
        return native::add_tuple(bt,p,a);
    }else if(Py_val::is_newtuple(a[0]) && Py_val::is_newtuple(a[1])){
        return native::add_newtuple(bt,p,a);
    }
    
    runtime_error(bt, p, "type error : invalid arguments were passed to 'add'");
    return py_val_none;
}

py_val_t native::prefix_add(stack<Stack_trace_entry> & bt,const SrcPos & p,
                            py_val_t * a){
    if(!(Py_val::is_int(a[0]) || Py_val::is_float(a[0]))){
        runtime_error(bt, p, "type error : cannot prefix_add");
    }
    return a[0];
}

py_val_t native::sub(stack<Stack_trace_entry> & bt, const SrcPos & p,
                     py_val_t * a){
    if(Py_val::is_int(a[0])){
        if(Py_val::is_int(a[1])){
            return sub_int(bt, p, a);
        }else if(Py_val::is_float(a[1])){
            a[0] = Py_val::mk_float((double)getint(a[0]));
            return native::sub_float(bt, p, a);
        }
    }else if(Py_val::is_float(a[0])){
        if(Py_val::is_int(a[1])){
            a[1] = Py_val::mk_float((double)getint(a[1]));
            return native::sub_float(bt, p, a);
        }else if(Py_val::is_float(a[1])){
            return sub_float(bt, p, a);
        }
    }
    
    runtime_error(bt, p, "type error : cannot subtract");
    return NULL;
}

py_val_t native::mul(stack<Stack_trace_entry> & bt,const SrcPos & p,
             py_val_t * a){
    py_val_t ret = NULL;

    if (Py_val::is_int(a[0])){
        if (Py_val::is_int(a[1])){
            ret = Py_val::mk_int(
                getint(a[0])
                * getint(a[1]));
        } else if (Py_val::is_float(a[1])){
            ret = Py_val::mk_float(
                (double) getint(a[0])
                * Py_val::get_float(a[1], bt, p));
        } else if (Py_val::is_char(a[1])){
            ret = a[1];
            a[1] = a[0];
            a[0] = ret;
            ret = native::mul_string(bt, p, a);
        } else if (Py_val::is_string(a[1])){
            ret = a[1];
            a[1] = a[0];
            a[0] = ret;
            ret = native::mul_string(bt, p, a);
        } else if (Py_val::is_tuple(a[1])){
            ret = native::mul_tuple(bt, p, a);
            //cerr << "not implemented @ " << __FILE__ << ":" << __LINE__ << endl; exit(1);
        } else if (Py_val::is_list(a[1])){
            py_val_t tmp = a[0];
            a[0] = a[1];
            a[1] = tmp;
            ret = native::mul_list(bt, p, a);
            //cerr << "not implemented @ " << __FILE__ << ":" << __LINE__ << endl; exit(1);
        } else {
            runtime_error(bt, p, "type error");
        }
    } else if (Py_val::is_float(a[0])){
        if (Py_val::is_int(a[1])){
            ret = Py_val::mk_float(
                Py_val::get_float(a[0], bt, p)
                * (double)getint(a[1]));
        } else if (Py_val::is_float(a[1])){
            ret = Py_val::mk_float(
                Py_val::get_float(a[0], bt, p)
                * Py_val::get_float(a[1], bt, p));
        } else {
            runtime_error(bt, p, "type error");
        }
    } else if (Py_val::is_char(a[0])){
        if (Py_val::is_int(a[1])){
            ret = native::mul_string(bt, p, a);
        } else {
            runtime_error(bt, p, "type error");
        } 
    } else if (Py_val::is_string(a[0])){
        if (Py_val::is_int(a[1])){
            ret = native::mul_string(bt, p, a);
        } else {
            runtime_error(bt, p, "type error");
        } 
    } else if (Py_val::is_list(a[0])){
        if (Py_val::is_int(a[1])){
            ret = native::mul_list(bt, p, a);
            // runtime_error(bt, p, "not implemented");
            // cerr << "not implemented @ " << __FILE__ << ":" << __LINE__ << endl; exit(1);
            // ret = native::mul_list(bt, p, a);
        } else {
            runtime_error(bt, p, "type error");
        } 
    } else if (Py_val::is_tuple(a[0])){
        if (Py_val::is_int(a[1])){
            cerr << "not implemented @ " << __FILE__ << ":" << __LINE__ << endl; exit(1);
            // ret = native::mul_tuple(bt, p, a);
        } else {
            runtime_error(bt, p, "type error");
        } 
    }  else {
        runtime_error(bt, p, "type error");
    }

    return ret;
}

py_val_t native::div(stack<Stack_trace_entry> & bt,const SrcPos & p,
             py_val_t * a){
    
    if (Py_val::is_int(a[0])){
        if (Py_val::is_int(a[1])){
            return native::div_int(bt, p, a);
        } else if(Py_val::is_float(a[1])){
            a[0] = Py_val::mk_float((double)getint(a[0]));
            return native::div_float(bt, p, a);
        } else {
            runtime_error(bt, p, "type error");
        }
    } else if(Py_val::is_float(a[0])){
        if (Py_val::is_int(a[1])){
            a[1] = Py_val::mk_float((double)getint(a[1]));
            return native::div_float(bt, p, a);
        } else if(Py_val::is_float(a[1])){
            return native::div_float(bt, p, a);
        } else {
            runtime_error(bt, p, "type error");
        }
    } else {
        runtime_error(bt, p, "type error");
    }

    runtime_error(bt, p, "type error");
    return NULL;
}
    
py_val_t native::mod(stack<Stack_trace_entry> & bt,const SrcPos & p,
                     py_val_t * a){
    if (Py_val::is_int(a[0]) && Py_val::is_int(a[1])){
        return native::mod_int(bt, p, a);
    } else if (Py_val::is_string(a[0])){
        py_val_t args[3];
        args[2] = NULL;
        args[0] = a[0];
        
        if(Py_val::is_int(a[1]) || Py_val::is_float(a[1])){
            stringstream ss;
            Py_val::to_str(ss, a[1], bt, p);
            args[1] = Py_val::mk_string(ss.str());
        } else if(Py_val::is_string(a[1]) || Py_val::is_newtuple(a[1])
                  || Py_val::is_tuple(a[1])){
            args[1] = a[1];
        } else {
            runtime_error(bt, p, "type error");
        }

        return native::replace_string(bt, p, args);
    }

    runtime_error(bt, p, "type error");
    return NULL;
}

py_val_t native::invert(stack<Stack_trace_entry> & bt,const SrcPos & p,
                        py_val_t * a){
    if(!(Py_val::is_int(a[0]))){
        runtime_error(bt, p, "type error: ~ required int");
    }
    return native::invert_int(bt,p,a);
}

py_val_t native::lshift(stack<Stack_trace_entry> & bt,const SrcPos & p,
                        py_val_t * a){
    if(!(Py_val::is_int(a[0]) || Py_val::is_int(a[1]))){
        runtime_error(bt, p, "type error: << required int");
    }
    return native::lshift_int(bt,p,a);
}

py_val_t native::rshift(stack<Stack_trace_entry> & bt,const SrcPos & p,
                        py_val_t * a){
    if(!(Py_val::is_int(a[0]) || Py_val::is_int(a[1]))){
        runtime_error(bt, p, "type error: << required int");
    }
    return native::rshift_int(bt,p,a);
}

py_val_t native::and_(stack<Stack_trace_entry> & bt,const SrcPos & p,
                        py_val_t * a){
    if (!(Py_val::is_int(a[0]) || Py_val::is_int(a[1]))){
        runtime_error(bt, p, "type error: & required int");
    }
    return native::and_int(bt,p,a);
}

py_val_t native::or_(stack<Stack_trace_entry> & bt,const SrcPos & p,
                        py_val_t * a){
    if (!(Py_val::is_int(a[0]) || Py_val::is_int(a[1]))){
        runtime_error(bt, p, "type error: | required int");
    }
    return native::or_int(bt,p,a);;
}

py_val_t native::xor_(stack<Stack_trace_entry> & bt,const SrcPos & p,
                        py_val_t * a){
    if(!(Py_val::is_int(a[0]) || Py_val::is_int(a[1]))){
        runtime_error(bt, p, "type error: ^ required int");
    }
    return  native::xor_int(bt,p,a);
}

py_val_t native::contains(stack<Stack_trace_entry> & bt,const SrcPos & p,
                         py_val_t * a){
    py_val_t ret;

    ret = a[0];
    a[0] = a[1];
    a[1] = ret;

    if (Py_val::is_tuple(a[0])){
        ret = native::hasitem_tuple(bt, p, a);
    } else if (Py_val::is_list(a[0])){
        ret = native::hasitem_list(bt, p, a);
    } else if (Py_val::is_newtuple(a[0])){
        ret = native::hasitem_newtuple(bt, p, a);
    } else {
        runtime_error(bt, p , "type error : 'getitem' requires a sequence");
    }
    
    return ret;
}

py_val_t native::not_contains(stack<Stack_trace_entry> & bt,const SrcPos & p,
                         py_val_t * a){
    if(Py_val::is_false( native::contains(bt, p, a))){
        return mkint(1);
    } 
    return mkint(0);
}




py_val_t native::not_(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a){
    py_val_t ret;
    if (Py_val::is_false(a[0])) {
        ret = Py_val::mk_int(1);
    } else {
        ret = Py_val::mk_int(0);
    }
    return ret;
}

py_val_t native::ntv_is(stack<Stack_trace_entry> & bt,const SrcPos & p,
                        py_val_t * a){
     py_val_t ret;
     
     if(Py_val::is_int(a[0]) && Py_val::is_int(a[1])){
         ret = mkint(Py_val::compare(a[0], a[1]) == 0);
     }else if(Py_val::is_float(a[0]) && Py_val::is_float(a[1])){
         ret = mkint(Py_val::compare(a[0], a[1]) == 0);
     }else if(Py_val::is_string(a[0]) && Py_val::is_string(a[1])){
         ret = native::eq_string(bt,p,a);
     }else if((Py_val::is_tuple(a[0]) && Py_val::is_tuple(a[1]))
              || (Py_val::is_list(a[0]) && Py_val::is_list(a[1]))
              || (Py_val::is_dict(a[0]) && Py_val::is_dict(a[1]))
              || (Py_val::is_newdict(a[0]) && Py_val::is_newdict(a[1]))
              || (Py_val::is_newtuple(a[0]) && Py_val::is_newtuple(a[1]))
         ){
         ret = native::eq_pointer(bt,p,a);
     }else{
         ret = Py_val::mk_int(0);
     }
     return ret;
}

py_val_t native::ntv_is_not(stack<Stack_trace_entry> & bt,const SrcPos & p,
                            py_val_t * a){
     py_val_t ret;
     if (Py_val::is_false(native::ntv_is(bt,p,a))){
        ret = Py_val::mk_int(1);
    } else {
        ret = Py_val::mk_int(0);
    }
    return ret;
}

py_val_t native::cmp(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a){
    int ret = Py_val::compare(a[0], a[1]);
    if (ret > 0){
        return mkint(1);
    } else if (ret < 0){
        return mkint(-1);
    }
    return mkint(0);
}

py_val_t native::eq(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a){
    if (Py_val::compare(a[0], a[1]) == 0){
        return mkint(1);
    }
    return mkint(0);
}

py_val_t native::ne(stack<Stack_trace_entry> & bt,const SrcPos & p,
                    py_val_t * a){
    if (Py_val::compare(a[0], a[1]) == 0){
        return mkint(0);
    }
    return mkint(1);
}

py_val_t native::gt(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a){
    if (Py_val::compare(a[0], a[1]) > 0){
        return mkint(1);
    }
    return mkint(0);
}

py_val_t native::lt(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a){
    if (Py_val::compare(a[0], a[1]) < 0){
        return mkint(1);
    }
    return mkint(0);
}

py_val_t native::ge(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a){
    if (Py_val::compare(a[0], a[1]) >= 0){
        return mkint(1);
    }
    return mkint(0);
}

py_val_t native::le(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a){
    if (Py_val::compare(a[0], a[1]) <= 0){
        return mkint(1);
    }
    return mkint(0);
}


py_val_t native::eq_pointer(stack<Stack_trace_entry> & bt,const SrcPos & p,
                            py_val_t * a){ //a is b _?
    return Py_val::mk_int(a[0] == a[1]);
}


py_val_t native::getitem(stack<Stack_trace_entry> & bt,const SrcPos & p,
                         py_val_t * a){
    py_val_t ret = NULL;
    
    if (Py_val::is_int(a[1])){
        if((Py_val::is_char(a[0]) || Py_val::is_string(a[0]))){
            ret = native::getitem_string(bt, p, a);
        } else if (Py_val::is_tuple(a[0])){
            ret = native::getitem_tuple(bt, p, a);
        } else if (Py_val::is_list(a[0])){
            ret = native::getitem_list(bt, p, a);
        } else if (Py_val::is_newtuple(a[0])){
            ret = native::getitem_newtuple(bt, p, a);
        } else if (Py_val::is_newdict(a[0])){
            ret = getnewdict(a[0])->get(a[1], bt, p);
        } else if (Py_val::is_dict(a[0])){
            ret = native::getitem_dict(bt, p, a);
        } else {
            runtime_error(bt, p , "type error : 'getitem' requires a sequence");
        }
    } else if (Py_val::is_newdict(a[0])){
        ret = getnewdict(a[0])->get(a[1], bt, p);
    } else if (Py_val::is_dict(a[0])){
        ret = native::getitem_dict(bt, p, a);
    } else {
        runtime_error(bt, p , "TypeError: list indices must be integers");
    }
    return ret;
}

py_val_t native::setitem(stack<Stack_trace_entry> & bt,const SrcPos & p,
                         py_val_t * a){
    py_val_t ret = NULL;

    if (Py_val::is_list(a[0]) && Py_val::is_int(a[1])){
        ret = native::setitem_list(bt, p, a);
    } else if (Py_val::is_newdict(a[0])){
        ret = getnewdict(a[0])->set(a[1], a[2], bt, p);
    } else if (Py_val::is_dict(a[0])){
        ret = native::setitem_dict(bt, p, a);
    } else {
        runtime_error(bt, p , "type error : 'setitem' requires a sequence");
    }
    
    return ret;
}

py_val_t native::delitem(stack<Stack_trace_entry> & bt,const SrcPos & p,
                         py_val_t * a){
    py_val_t ret = NULL;

    if (Py_val::is_list(a[0]) && Py_val::is_int(a[1])){
        ret = native::delitem_list(bt, p, a);
    } else if (Py_val::is_newdict(a[0])){
        ret = getnewdict(a[0])->del(a[1], bt, p);
    } else if (Py_val::is_dict(a[0])){
        ret = native::delitem_dict(bt, p, a);
    } else {
        runtime_error(bt, p , "type error : 'getitem' requires a sequence");
    }
    
    return ret;
}


py_val_t native::itof(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a){
    py_val_t ret;
    if (!Py_val::is_int(a[0])){
        runtime_error(bt, p, "type error : integer required.");
    }
    ret = Py_val::mk_float((double)getint(a[0]));
    return ret;
}


py_val_t native::str(stack<Stack_trace_entry> & bt, const SrcPos & p,
                     py_val_t * a){
    stringstream ss;
    if (Py_val::is_string(a[0])){
        return a[0];
    } else {
        Py_val::to_str(ss, a[0], bt, p);
        return Py_val::mk_string(ss.str());
    }
}

py_val_t native::replace_string(stack<Stack_trace_entry> & bt, const SrcPos & p,
                     py_val_t * a){

    string tmp_s = *Py_val::get_string(a[0], bt, p);

    if (Py_val::is_char(a[1])){
        string::size_type n, nb = 0;
        bool flag = false;
        string tmp_r = string(1, Py_val::get_char(a[1], bt, p));
        while ((n = tmp_s.find("%s",nb)) != string::npos){
            if(flag){ // %sが複数ある
                runtime_error(bt, p, "TypeError: not enough arguments for format string");
            }
            tmp_s.replace(n,2,tmp_r);
            flag = true;
        }
        if(!flag){// %sが足りない
            runtime_error(bt, p, "TypeError: not all arguments converted during string formatting");
        }
    } else if (Py_val::is_string(a[1])){
        string::size_type n, nb = 0;
        bool flag = false;
        string tmp_r = *Py_val::get_string(a[1], bt, p);
        while ((n = tmp_s.find("%s",nb)) != string::npos){
            if(flag){ // %sが複数ある
                runtime_error(bt, p, "TypeError: not enough arguments for format string");
            }
            tmp_s.replace(n,2,tmp_r);
            flag = true;
        }
        if(!flag){// %sが足りない
            runtime_error(bt, p, "TypeError: not all arguments converted during string formatting");
        }
    } else if (Py_val::is_tuple(a[1])){
        string::size_type n, nb = 0;
        vector<py_val_t>* tmp_t = Py_val::get_tuple(a[1], bt, p);
        vector<py_val_t>::iterator it;

        for(it=tmp_t->begin(); it < tmp_t->end(); it++){
            if((n = tmp_s.find("%s",nb)) == string::npos){ // %sが足りない
                runtime_error(bt, p, "TypeError: not all arguments converted during string formatting");
            }
            stringstream ss;
            if (is_boxed(*it) && (*it)->type == py_type_string) {
                tmp_s.replace(n, 2, *Py_val::get_string(*it, bt, p));
            } else {
                Py_val::to_str(ss, *it, bt, p);
                tmp_s.replace(n, 2, ss.str());
            }
        }
        
        if(tmp_s.find("%s",nb) != string::npos){ // %sが多すぎる
            runtime_error(bt, p, "TypeError: not enough arguments for format string");
        }
    } else if (Py_val::is_newtuple(a[1])){
        string::size_type n, nb = 0;
        Py_tuple * tmp_t = getnewtuple(a[1]);
        
        for(uint i = 0;i < tmp_t->size();i++){
            if((n = tmp_s.find("%s",nb)) == string::npos){ // %sが足りない
                runtime_error(bt, p, "TypeError: not all arguments converted during string formatting");
            }
            stringstream ss;
            if (is_boxed(tmp_t->get(i)) && tmp_t->get(i)->type == py_type_string) {
                tmp_s.replace(n, 2, *Py_val::get_string(tmp_t->get(i), bt, p));
            } else {
                Py_val::to_str(ss, tmp_t->get(i), bt, p);
                tmp_s.replace(n, 2, ss.str());
            }
        }
        
        if(tmp_s.find("%s",nb) != string::npos){ // %sが多すぎる
            runtime_error(bt, p, "TypeError: not enough arguments for format string");
        }
    }else{
        runtime_error(bt, p, "type error : string or tuple required.");
    }
    return Py_val::mk_string(tmp_s);
}



/* 文字列・リスト・辞書・タプルに共通関数 */
py_val_t native::len(stack<Stack_trace_entry> & bt,const SrcPos & p,
                     py_val_t * a){
    
    if(Py_val::is_char(a[0])){
        return Py_val::mk_int(1);
    }

    if (is_boxed(a[0])){
        switch(a[0]->type){
        case py_type_string:
            return len_string(bt,p,a);
            break;
        case py_type_tuple:
            return len_tuple(bt,p,a);
            break;
        case py_type_newtuple:
            return len_newtuple(bt,p,a);
            break;
        case py_type_list:
            return len_list(bt,p,a);
            break;
        case py_type_dict:
            return len_dict(bt,p,a);
            break;
        case py_type_newdict:
            return mkint(getnewdict(a[0])->size());
            break;
        default:
            break;
        }
    }
    
    runtime_error(bt, p, "type error : 'len' requires string,list,tuple or dict.");
    return Py_val::mk_int(-1);
}


/* リスト用ユーティリティ関数 */
py_val_t native::append(stack<Stack_trace_entry> & bt,const SrcPos & p,
                py_val_t * a){
    if (Py_val::is_list(a[0])){
        a[0]->u.l->push_back(a[1]);
        return a[1];
    }

    runtime_error(bt, p, "type error : list required.");
    return py_val_none;
}

py_val_t native::add_list(stack<Stack_trace_entry> & bt,const SrcPos & p,
                          py_val_t * a){
    if (!Py_val::is_list(a[0]) || !Py_val::is_list(a[1])){
        runtime_error(bt, p, "type error: list required.");
    }

    vector<py_val_t> * newlst = new vector<py_val_t>(*a[0]->u.l);
    vector<py_val_t> * lst2 = Py_val::get_list(a[1], bt, p);
    newlst->insert(newlst->end(), lst2->begin(), lst2->end());

    return Py_val::mk_list_byptr(newlst);
}

py_val_t native::range(stack<Stack_trace_entry> & bt,const SrcPos & p,
                       py_val_t * a){
    vector<py_val_t> * ls = new vector<py_val_t>();
    int tmp1, tmp2;

    int size = 0;

    for(int i = 0; a[i] != NULL; i++){
        if (!Py_val::is_int(a[i])){
            runtime_error(bt, p, "type error : integer required.");
        }
        size = i + 1;
    }
    
    switch(size){
    case 1:
    {
        tmp1 = getint(a[0]);
        for(int i = 0;i < tmp1;i++){
            ls->push_back(Py_val::mk_int(i));
        }

        return Py_val::mk_list_byptr(ls);
    }
        break;
    case 2:
    {
        tmp1 = getint(a[1]);
        for(int i = getint(a[0]);i < tmp1;i++){
            ls->push_back(Py_val::mk_int(i));
        }

        return Py_val::mk_list_byptr(ls);
    }
        break;
    case 3:
    {
        tmp1 = getint(a[1]); // end condition
        tmp2 = getint(a[2]); // step

        if (tmp2 > 0){
            for(int i = getint(a[0])
                    ;i < tmp1;i += tmp2){
                ls->push_back(Py_val::mk_int(i));
            }
        } else if (tmp2 < 0) {
            for(int i = getint(a[0])
                    ;i > tmp1;i += tmp2){
                ls->push_back(Py_val::mk_int(i));
            }
        } else {
            runtime_error(bt, p, "range() step must not be zero.");
        }
        
        return Py_val::mk_list_byptr(ls);
    }
        break;
    default:
    {
        stringstream ss;
        ss << "range requires 1,2 or 3 arguments, but got " << size;
        runtime_error(bt, p, ss.str());
    }
    break;
    }

    return NULL;
}

py_val_t native::pop(stack<Stack_trace_entry> & bt,const SrcPos & p,
                     py_val_t * a){
    py_val_t ret = NULL;
    vector<py_val_t> * vec;
    vector<Py_dict_entry*> * dict_vec;
    
    int size = 0;
    for(int i = 0;a[i] != NULL;i++){ size = i + 1; }

    if (Py_val::is_list(a[0])){
        vec = Py_val::get_list(a[0], bt, p);
        if (size == 1){
            ret = vec->back();
            vec->pop_back();
        } else if(size == 2){
            if (!Py_val::is_int(a[1])){
                runtime_error(bt, p, "type error : index of list must be an integer.");
            }
            int index = getint(a[1]);

            if (index < 0 || (int)vec->size() <= index){
                runtime_error(bt, p, "List index out of bounds.");
            }

            ret = (*vec)[index];
            vec->erase(vec->begin() + index);
        } else {
            runtime_error(bt, p, "invalid argument number.");
        }
    } else if(Py_val::is_dict(a[0])){
        if (size != 2){
            runtime_error(bt, p, "invalid argument number.");
        }
        dict_vec = Py_val::get_dict(a[0], bt, p);

        bool found = false;
        vector<py_val_t> cmp_args;
        cmp_args.push_back(a[1]);
        for(vector<Py_dict_entry*>::iterator itr = dict_vec->begin()
                ; itr != dict_vec->end(); itr++){
            cmp_args[1] = (*itr)->key;
            // if(native::equal(bt, p, cmp_args){
            if (false) {
                dict_vec->erase(itr);
                found = true;
            }
        }
        if (!found){
            runtime_error(bt, p, "key error.");
        }
    } else {
        runtime_error(bt, p, "type error : pop() requires dict or list.");
    }

    return ret;
}

py_val_t native::ntv_random(stack<Stack_trace_entry> & bt,const SrcPos & p,
                            py_val_t * a){
    py_val_t ret = NULL;

    ret = Py_val::mk_int(rand() % (1 << 30));

    return ret;
}

py_val_t native::sys_sleep(stack<Stack_trace_entry> & bt,const SrcPos & p,
                            py_val_t * a){
    py_val_t ret = NULL;

    sleep(Py_val::get_int(a[0], bt, p));

    return ret;
}

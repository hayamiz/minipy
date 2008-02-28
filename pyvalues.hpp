// -*- coding: utf-8 -*-

#ifndef PYVALUES_HPP
#define PYVALUES_HPP

#define DO_IT_YOURSELF 

#include <vector>
#include <stack>
#include <string>
#include <map>
#include <assert.h>
#include "syntax_tree.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"
#include "symbol.hpp"

class Py_val;
class Py_ifun;
class Py_nfun;
class Py_vm_ifun;
class Stack_trace_entry;
class Py_dict_entry;
class Py_dict;
class Env;
class Py_tuple;

/* stack trace */
class Stack_trace_entry{
public:
    string name;
    const SrcPos & call_site;
    Stack_trace_entry(const string & name, const SrcPos & call_site);
};

/* error reporting */
void print_stack_trace(stack<Stack_trace_entry> & bt );
static stack<Stack_trace_entry> stack_trace;
void runtime_error(stack<Stack_trace_entry> & bt, const SrcPos & pos, const string & msg);

/* すべては以下のpy_val_t型のデータ(32bit)として表現する */
typedef Py_val*  py_val_t;
typedef map<symbol_t, py_val_t> GlobalEnv;

typedef py_val_t (*C_fun_t)(stack<Stack_trace_entry> &, const SrcPos &,py_val_t *);	/* C関数 */

/* Python値の種類を表す定数 */
typedef enum{
//    py_type_none,
    py_type_int,		/* 整数 */
    py_type_char,		/* 文字(長さ1の文字列) */
    py_type_float,		/* 浮動小数点 */
    py_type_tuple,		/* タプル */
    py_type_list,		/* リスト */
    py_type_dict,		/* 辞書 */
    py_type_string,		/* 文字列 */
    py_type_ifun,		/* Python関数 */
    py_type_nfun,		/* Native関数 */
    py_type_vm_ifun,
    py_type_newdict,		/* 辞書 */
    py_type_newtuple,
} py_type_t;


/* 構造体中に置かれるPython値．
   整数，文字，Noneはpy_val_t (32bit)の中にすべて格納されることに注意 */


/*
  py_val_tに格納されるbit列の表現

  ...1 : int                    (最下位bit=1は整数)
  ...00 : pointer (None = 0)     (py_valへのポインタ．上を見よ)
  ...10 : other values           (その他．文字と，処理系内部でのみ使われる特別な値)
  ...   010 : char                   (文字)
  ...   110 : special                (処理系内部でのみ使われる特別な値)
  ... 000110 : continue　　　　　　　 ある文がcontinueを実行したことを表す値
  ... 001110 : break　　　　　　　　　ある文がbreakを実行したことを表す値
  ... 010110 : next                   ある文がcontinue/break/returnを実行しなかった
  ことを表す値
  ... 011110 : not found              変数の値を得ようとしたら見つからなかったことを示す値
  ... 100110 : var is global          変数の値を得ようとしたらglobal変数として
  見つかったことを示す値

  特別な値については今は理解できなくて良い(後に実際に評価器を作る際に説明する)．

*/

/* 上記のデータ表現を助けるための定数 */
enum {
    py_val_tag_int      = 1,	/* 整数のタグ */
    py_val_tag_other    = 2,	/* other valuesのタグ */
    py_val_tag_char     = (0 << 2) + py_val_tag_other, /* 文字のタグ */
    py_val_tag_special  = (1 << 2) + py_val_tag_other,
    py_val_continue_    = (0 << 3) + py_val_tag_special,
    py_val_break_       = (1 << 3) + py_val_tag_special,
    py_val_next_        = (2 << 3) + py_val_tag_special,
    py_val_not_found_   = (3 << 3) + py_val_tag_special,
    py_val_global_      = (4 << 3) + py_val_tag_special,
    py_val_none_        = (5 << 3) + py_val_tag_special,
};

#define py_val_continue  ((py_val_t)py_val_continue_)
#define py_val_break     ((py_val_t)py_val_break_)
#define py_val_next      ((py_val_t)py_val_next_)
#define py_val_not_found ((py_val_t)py_val_not_found_)
#define py_val_global    ((py_val_t)py_val_global_)
#define py_val_none      ((py_val_t)py_val_none_)

class Py_val{
public:
    py_type_t type;
    union {
        double f;		/* 浮動小数点数 */
        symbol_t s;		/* 文字列 */
        Py_ifun* i;		/* Python関数 */
        Py_nfun* n;		/* Native関数 */
        Py_vm_ifun* vm_i;		/* VMPython関数 */
        vector<py_val_t>* l;	/* tuple or list */
        vector<Py_dict_entry*>* d;	/* dict */
        Py_dict * nd;	/* dict */
        Py_tuple * nl;  /* tuple or list(in the future) */
    } u;

    /* それぞれpが整数，文字か，浮動小数点なら1 */
    static bool is_int(py_val_t pyval);
    static bool is_char(py_val_t pyval);
    static bool is_none(py_val_t pyval);
    static bool is_float(py_val_t pyval);
    static bool is_string(py_val_t pyval);
    static bool is_ifun(py_val_t pyval);
    static bool is_vfun(py_val_t pyval);
    static bool is_nfun(py_val_t pyval);
    static bool is_tuple(py_val_t pyval);
    static bool is_list(py_val_t pyval);
    static bool is_dict(py_val_t pyval);
    static bool is_newdict(py_val_t pyval);
    static bool is_newtuple(py_val_t pyval);
    // static bool is_boxed(py_val_t pyval);
    static bool is_number(py_val_t pyval); // is int or float

    /* 型チェックをし、Py_val_tから値を取り出す.
      runtimeエラーを出すのもここです。
     */
    static int get_int(py_val_t pyval, stack<Stack_trace_entry> & bt, const SrcPos & pos);
    static char get_char(py_val_t pyval, stack<Stack_trace_entry> & bt, const SrcPos & pos);
    static double get_float(py_val_t pyval, stack<Stack_trace_entry> & bt, const SrcPos & pos);
    static string * get_string(py_val_t pyval, stack<Stack_trace_entry> & bt, const SrcPos & pos);
    static vector<py_val_t> * get_tuple(py_val_t pyval, stack<Stack_trace_entry> & bt, const SrcPos & pos);
    static vector<py_val_t> * get_list(py_val_t pyval, stack<Stack_trace_entry> & bt, const SrcPos & pos);
    static Py_tuple * get_newtuple(py_val_t pyval, stack<Stack_trace_entry> & bt, const SrcPos & pos);
    static vector<Py_dict_entry*> * get_dict(py_val_t pyval, stack<Stack_trace_entry> & bt, const SrcPos & pos);
    static map<int, vector<Py_dict_entry*> > * get_newdict(py_val_t pyval, stack<Stack_trace_entry> & bt, const SrcPos & pos);
    static symbol_t get_ifun_name(py_val_t pyval, stack<Stack_trace_entry> & bt, const SrcPos & pos);
    static symbol_t get_nfun_name(py_val_t pyval, stack<Stack_trace_entry> & bt, const SrcPos & pos);
    static string get_vm_ifun_name(py_val_t pyval, stack<Stack_trace_entry> & bt, const SrcPos & pos);

    static uint hash(py_val_t pyval);

    static py_val_t alloc_boxed(py_type_t type);

    /* Python値を作り出す関数たち */
    static py_val_t mk_int(int i);
    static py_val_t mk_char(int c);
    static py_val_t mk_float(double f);
    static py_val_t mk_string(const string & a);
    static py_val_t mk_ifun(const symbol_t  name, vector<symbol_t>  ps, vector<Stm> & b);
    static py_val_t mk_nfun(const symbol_t name, int arity, C_fun_t f);
    static py_val_t mk_vm_ifun(const symbol_t name, int arity, int locals, int addr, const SrcPos p);
    static py_val_t mk_tuple(const vector<Py_val*> & l);
    static py_val_t mk_list(const vector<Py_val*> & l);
    
    static py_val_t mk_tuple_byptr(vector<Py_val*> * l);
    static py_val_t mk_list_byptr(vector<Py_val*> * l);
    
    static py_val_t mk_newtuple(Py_tuple * l);

    static py_val_t mk_dict(vector<Py_dict_entry*> & d);
    static py_val_t mk_newdict();
    static py_val_t mk_newdict(vector<Py_dict_entry*> & d);
    static py_val_t mk_none();

#define mkint(var) Py_val::mk_int(var)
#define mkchar(var) Py_val::mk_char(var)
#define mkfloat(var) Py_val::mk_float(var)
#define mkstring(var) Py_val::mk_string(var)
#define mkifun(v1, v2, v3) Py_val::mk_ifun(v1, v2, v3)
#define mknfun(v1, v2, v3) Py_val::mk_nfun(v1, v2, v3)
#define mkvm_ifun(v1, v2, v3, v4, v5) Py_val::mk_vm_ifun(v1, v2, v3, v4, v5)
#define mktuple(var) Py_val::mk_tuple(var)
#define mklist(var) Py_val::mk_list(var)
#define mkdict(var) Py_val::mk_dict(var)
#define mknewtuple(var) Py_val::mk_newtuple(var)
#define mknewdict(var) Py_val::mk_newdict(var)
#define mknone() Py_val::mk_none()

    // 以下のマクロは, varの型が判明しているときのみ使うこと
#define getint(var) (((int)var) >> 1)
#define getchar(var) ((char)(((int)var) >> 3))
#define getfloat(var) ((var)->u.f)
#define getstring(var) ((var)->u.s)
#define getifun(var) ((var)->u.i)
#define getvfun(var) ((var)->u.vm_i)
#define getnfun(var) ((var)->u.n)
#define getlist(var) ((var)->u.l)
#define getdict(var) ((var)->u.d)
#define gettuple(var) ((var)->u.l)
#define getnewtuple(var) ((var)->u.nl)
#define getnewdict(var) ((var)->u.nd)

    /* if 文でfalse扱いされる値なら1 */
    static bool is_false(py_val_t pyval);
    static bool is_true(py_val_t pyval);

    // return    0    if val1 == val2
    //       negative if val1 < val2
    //       positive if val1 > val2
    static int compare(py_val_t val1, py_val_t val2);
    static int compare_tuple(py_val_t val1, py_val_t val2);
    static int compare_list(py_val_t val1, py_val_t val2);
    static int compare_newtuple(py_val_t val1, py_val_t val2);
    static int compare_newdict(py_val_t val1, py_val_t val2);
    static void to_str(ostream & ss, py_val_t pyval, stack<Stack_trace_entry> & bt, const SrcPos & p);
    static void print(py_val_t pyval, stack<Stack_trace_entry> & bt, const SrcPos & p);
    
#define is_boxed(___ptr) (((int)___ptr & 3) == 0)
};

/* 辞書の1エントリ (key : value の組) */

class Py_dict_entry{
public:
    py_val_t key;
    py_val_t val;
    Py_dict_entry(py_val_t key, py_val_t val);
};


class Py_ifun{
public:
    symbol_t name;	/* defされた名前 */
    vector<symbol_t> ps;	/* 引数のベクタ(文字列のベクタ) */
    vector<Stm> b;	/* 本体(文のベクタ) */
};


// VM上で, pythonにより定義された関数
class Py_vm_ifun{
public:
    symbol_t name;	/* defされた名前 */
    int arity;
    int addr;
    int locals;
    SrcPos pos;
};

/* Native関数 */
class Py_nfun{
public:
    symbol_t name;		/* 名前 */
    int arity;			/* 引数の数 */
    C_fun_t f;			/* C関数 */
};

class Py_dict {
#define PY_DICT_TABLE_SIZE 10000000
private:
    vector<Py_dict_entry *> * dict[PY_DICT_TABLE_SIZE];
    vector<py_val_t> keys;   // ヒープを使ってソートされた状態で保持する？
    vector<py_val_t> values; // ヒープを使ってソートされた状態で保持する？

    int size_;

    py_val_t pykeys;
    py_val_t pyvalues;

    Py_dict_entry * get_entry(py_val_t key);
    void add_key(py_val_t key);
    void add_value(py_val_t value);
    void del_key(py_val_t key);
    void del_value(py_val_t value);

public:
    Py_dict();

    // 外部API
    py_val_t set(py_val_t key, py_val_t val, stack<Stack_trace_entry> & bt, const SrcPos & p);
    py_val_t set_entry(Py_dict_entry * entry, stack<Stack_trace_entry> & bt, const SrcPos & p);
    py_val_t get(py_val_t key, stack<Stack_trace_entry> & bt, const SrcPos & p);
    py_val_t del(py_val_t key, stack<Stack_trace_entry> & bt, const SrcPos & p);
    py_val_t get_values(stack<Stack_trace_entry> & bt, const SrcPos & p);
    py_val_t get_keys(stack<Stack_trace_entry> & bt, const SrcPos & p);

    py_val_t has_key(py_val_t key, stack<Stack_trace_entry> & bt, const SrcPos & p);
    int size();
};


class Py_tuple {
private:
    py_val_t * vec;
    unsigned int capacity;
    unsigned int size_;

    void realloc(unsigned int required_capacity);
public:
    Py_tuple(uint capacity_);
    virtual ~Py_tuple();
    
    void push_back(py_val_t val);
    void append(Py_tuple * tup);
    py_val_t get(unsigned int index);
    unsigned int size();
};

#endif

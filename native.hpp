
#ifndef NATIVE_HPP
#define NATIVE_HPP
#include <stack>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include "pyvalues.hpp"
#include "syntax_tree.hpp"
#include "env.hpp"
#include "tivi.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>

#define DEFINE_PY_FUNC(funcname)                                \
    py_val_t funcname(ConsStack<Stack_trace_entry*> * bt        \
                      ,const SrcPos & p,                        \
                      py_val_t * a)

namespace native{
    /* 値が特定の型であるかを判定する関数群 */
    py_val_t is_int(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                    py_val_t * a);
    py_val_t is_float(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                      py_val_t * a);
    py_val_t is_string(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                       py_val_t * a);
    py_val_t is_dict(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                     py_val_t * a);
    py_val_t is_list(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                     py_val_t * a);
    py_val_t is_tuple(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                     py_val_t * a);
    py_val_t is_ifun(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                     py_val_t * a);
    py_val_t is_nfun(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                     py_val_t * a);
    
    /* 整数同士の演算に限定した各種演算子 */
    py_val_t add_int(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                     py_val_t * a);     // +
    py_val_t sub_int(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                    py_val_t * a);     // -
    py_val_t mul_int(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                    py_val_t * a);     // *
    py_val_t div_int(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                    py_val_t * a);     // /
    py_val_t mod_int(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                    py_val_t * a);     // %
    py_val_t invert_int(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                     py_val_t * a);  // ~
    py_val_t and_int(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                    py_val_t * a);     // &
    py_val_t or_int(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                   py_val_t * a);      // |
    py_val_t xor_int(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                    py_val_t * a);     // ^
    py_val_t lshift_int(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                       py_val_t * a);  // <<
    py_val_t rshift_int(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                       py_val_t * a);  // >>

    py_val_t prefix_sub(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                       py_val_t * a);  // +や-のprefix処理

    /* 浮動小数点数同士の演算に限定した各種演算 */
    py_val_t add_float(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                      py_val_t * a);     // +
    py_val_t sub_float(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                      py_val_t * a);     // -
    py_val_t mul_float(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                      py_val_t * a);     // *
    py_val_t div_float(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                      py_val_t * a);     // /

    /* 文字列の演算子 */
    py_val_t len_string(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                        py_val_t * a); // 文字列の長さ
    py_val_t getitem_string(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                           py_val_t * a); // sのi番めの要素
    py_val_t add_string(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                        py_val_t * a); //sとtの結合
    py_val_t mul_string(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                        py_val_t * a); //sとtの結合
    py_val_t eq_string(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                        py_val_t * a); //sとtの結合
    
    /* タプルの演算子 */
    py_val_t len_tuple(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                      py_val_t * a); // タプルの長さ
    py_val_t getitem_tuple(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                           py_val_t * a); // tのi番めの要素
    py_val_t add_tuple(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                       py_val_t * a); //tとuの結合
    py_val_t mul_tuple(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                       py_val_t * a); //t*number
    py_val_t hasitem_tuple(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                       py_val_t * a); //kがtにあるか？a=(t,k)

    // new implementation of tuple
    py_val_t len_newtuple(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                      py_val_t * a); // タプルの長さ
    py_val_t getitem_newtuple(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                           py_val_t * a); // tのi番めの要素
    py_val_t add_newtuple(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                       py_val_t * a); //tとuの結合
    py_val_t mul_newtuple(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                       py_val_t * a); //t*number
    py_val_t hasitem_newtuple(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                       py_val_t * a); //kがtにあるか？a=(t,k)

    
    /* リストの演算子 */
    py_val_t len_list(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                      py_val_t * a); // リストの長さ
    py_val_t getitem_list(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                         py_val_t * a); // lのi番めの要素
    py_val_t setitem_list(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                          py_val_t * a); //xをlのi番目に追加
    py_val_t delitem_list(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                          py_val_t * a); //lのi番目を削除
    py_val_t pop_list(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                      py_val_t * a); //lのi番目を削除＆値を返す
    py_val_t hasitem_list(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                       py_val_t * a); //kがlにあるか？a=(l,k)
    py_val_t mul_list(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                      py_val_t * a); //l*number


    /* 辞書の演算子 */
    py_val_t len_dict(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                      py_val_t * a); //dの要素数
    py_val_t has_key(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                          py_val_t * a); // Keyがdに存在するか
    py_val_t getitem_dict(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                          py_val_t * a); // Keyに対する値
    py_val_t setitem_dict(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                          py_val_t * a); // Keyに対する値をxに
    py_val_t delitem_dict(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                          py_val_t * a); //Keyに対する値を削除
    py_val_t keys(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                      py_val_t * a); // すべてのKeyリスト
    py_val_t values(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                         py_val_t * a); // すべての値リスト
    py_val_t items(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                        py_val_t * a); // すべてのKey:値のリスト
    
    /* 各種値を文字列に変換する関数群 */
    py_val_t repr_int(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                      py_val_t * a);
    py_val_t repr_float(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                        py_val_t * a);
    py_val_t repr_ifun(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                       py_val_t * a);
    py_val_t repr_nfun(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                       py_val_t * a);
    
    py_val_t print_(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                    py_val_t * a);
    py_val_t print_string(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                    py_val_t * a);
    py_val_t dprint(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                    py_val_t * a);

    /* 演算子 */
    py_val_t add(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                  py_val_t * a);
     py_val_t prefix_add(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t sub(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t mul(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t div(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t mod(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                 py_val_t * a);
    py_val_t invert(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                 py_val_t * a);
    py_val_t lshift(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                 py_val_t * a);
    py_val_t rshift(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
              py_val_t * a);
    py_val_t and_(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
              py_val_t * a);
    py_val_t or_(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
              py_val_t * a);
    py_val_t xor_(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
              py_val_t * a);
    py_val_t contains(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                      py_val_t * a);
    py_val_t not_contains(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                      py_val_t * a);
    py_val_t not_(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t ntv_is(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t ntv_is_not(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t cmp(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t eq(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t ne(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t gt(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t lt(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t ge(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t le(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t eq_pointer(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                       py_val_t * a); //a is b _?(tuple/sict/list)
    py_val_t getitem(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                 py_val_t * a);
    py_val_t setitem(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                 py_val_t * a);
    py_val_t delitem(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                 py_val_t * a);


    /* 変換用の関数たち */

    py_val_t itof(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                 py_val_t * a);
    py_val_t str(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                 py_val_t * a);
    py_val_t replace_string(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                 py_val_t * a);


    py_val_t len(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                 py_val_t * a);
        /* リスト用ユーティリティ関数 */
    py_val_t append(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                    py_val_t * a);
    py_val_t add_list(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                      py_val_t * a);
    py_val_t range(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                      py_val_t * a);

    // popは,dictとlistでとる引数の数がちがうよん. 注意.
    py_val_t pop(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                 py_val_t * a);

    
    py_val_t ntv_random(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                        py_val_t * a);
    py_val_t sys_sleep(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                   py_val_t * a);

    // experimental multithread implementation
    py_val_t mutex(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                   py_val_t * a);
    py_val_t lock(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t unlock(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                    py_val_t * a);

    py_val_t condition(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t acquire(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t release(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t wait(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t notify(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                    py_val_t * a);
    py_val_t notifyAll(ConsStack<Stack_trace_entry*> * bt,const SrcPos & p,
                       py_val_t * a);
    DEFINE_PY_FUNC(get_rawaddr);

    // networking
    DEFINE_PY_FUNC(make_sockaddr);

    DEFINE_PY_FUNC(py_socket);
    DEFINE_PY_FUNC(py_bind);
    DEFINE_PY_FUNC(py_listen);
    DEFINE_PY_FUNC(py_accept);
    DEFINE_PY_FUNC(py_recv);
    DEFINE_PY_FUNC(py_send);
    DEFINE_PY_FUNC(py_close);
}

#endif

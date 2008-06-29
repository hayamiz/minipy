
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

// experimental multithread implementation
#include <pthread.h>

namespace native{
    /* 値が特定の型であるかを判定する関数群 */
    py_val_t is_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                    py_val_t * a);
    py_val_t is_float(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a);
    py_val_t is_string(stack<Stack_trace_entry> & bt,const SrcPos & p,
                       py_val_t * a);
    py_val_t is_dict(stack<Stack_trace_entry> & bt,const SrcPos & p,
                     py_val_t * a);
    py_val_t is_list(stack<Stack_trace_entry> & bt,const SrcPos & p,
                     py_val_t * a);
    py_val_t is_tuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                     py_val_t * a);
    py_val_t is_ifun(stack<Stack_trace_entry> & bt,const SrcPos & p,
                     py_val_t * a);
    py_val_t is_nfun(stack<Stack_trace_entry> & bt,const SrcPos & p,
                     py_val_t * a);
    
    /* 整数同士の演算に限定した各種演算子 */
    py_val_t add_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                     py_val_t * a);     // +
    py_val_t sub_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                    py_val_t * a);     // -
    py_val_t mul_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                    py_val_t * a);     // *
    py_val_t div_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                    py_val_t * a);     // /
    py_val_t mod_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                    py_val_t * a);     // %
    py_val_t invert_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                     py_val_t * a);  // ~
    py_val_t and_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                    py_val_t * a);     // &
    py_val_t or_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                   py_val_t * a);      // |
    py_val_t xor_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                    py_val_t * a);     // ^
    py_val_t lshift_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                       py_val_t * a);  // <<
    py_val_t rshift_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                       py_val_t * a);  // >>

    py_val_t prefix_sub(stack<Stack_trace_entry> & bt,const SrcPos & p,
                       py_val_t * a);  // +や-のprefix処理

    /* 浮動小数点数同士の演算に限定した各種演算 */
    py_val_t add_float(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a);     // +
    py_val_t sub_float(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a);     // -
    py_val_t mul_float(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a);     // *
    py_val_t div_float(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a);     // /

    /* 文字列の演算子 */
    py_val_t len_string(stack<Stack_trace_entry> & bt,const SrcPos & p,
                        py_val_t * a); // 文字列の長さ
    py_val_t getitem_string(stack<Stack_trace_entry> & bt,const SrcPos & p,
                           py_val_t * a); // sのi番めの要素
    py_val_t add_string(stack<Stack_trace_entry> & bt,const SrcPos & p,
                        py_val_t * a); //sとtの結合
    py_val_t mul_string(stack<Stack_trace_entry> & bt,const SrcPos & p,
                        py_val_t * a); //sとtの結合
    py_val_t eq_string(stack<Stack_trace_entry> & bt,const SrcPos & p,
                        py_val_t * a); //sとtの結合
    
    /* タプルの演算子 */
    py_val_t len_tuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a); // タプルの長さ
    py_val_t getitem_tuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                           py_val_t * a); // tのi番めの要素
    py_val_t add_tuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                       py_val_t * a); //tとuの結合
    py_val_t mul_tuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                       py_val_t * a); //t*number
    py_val_t hasitem_tuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                       py_val_t * a); //kがtにあるか？a=(t,k)

    // new implementation of tuple
    py_val_t len_newtuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a); // タプルの長さ
    py_val_t getitem_newtuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                           py_val_t * a); // tのi番めの要素
    py_val_t add_newtuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                       py_val_t * a); //tとuの結合
    py_val_t mul_newtuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                       py_val_t * a); //t*number
    py_val_t hasitem_newtuple(stack<Stack_trace_entry> & bt,const SrcPos & p,
                       py_val_t * a); //kがtにあるか？a=(t,k)

    
    /* リストの演算子 */
    py_val_t len_list(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a); // リストの長さ
    py_val_t getitem_list(stack<Stack_trace_entry> & bt,const SrcPos & p,
                         py_val_t * a); // lのi番めの要素
    py_val_t setitem_list(stack<Stack_trace_entry> & bt,const SrcPos & p,
                          py_val_t * a); //xをlのi番目に追加
    py_val_t delitem_list(stack<Stack_trace_entry> & bt,const SrcPos & p,
                          py_val_t * a); //lのi番目を削除
    py_val_t pop_list(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a); //lのi番目を削除＆値を返す
    py_val_t hasitem_list(stack<Stack_trace_entry> & bt,const SrcPos & p,
                       py_val_t * a); //kがlにあるか？a=(l,k)
    py_val_t mul_list(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a); //l*number


    /* 辞書の演算子 */
    py_val_t len_dict(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a); //dの要素数
    py_val_t has_key(stack<Stack_trace_entry> & bt,const SrcPos & p,
                          py_val_t * a); // Keyがdに存在するか
    py_val_t getitem_dict(stack<Stack_trace_entry> & bt,const SrcPos & p,
                          py_val_t * a); // Keyに対する値
    py_val_t setitem_dict(stack<Stack_trace_entry> & bt,const SrcPos & p,
                          py_val_t * a); // Keyに対する値をxに
    py_val_t delitem_dict(stack<Stack_trace_entry> & bt,const SrcPos & p,
                          py_val_t * a); //Keyに対する値を削除
    py_val_t keys(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a); // すべてのKeyリスト
    py_val_t values(stack<Stack_trace_entry> & bt,const SrcPos & p,
                         py_val_t * a); // すべての値リスト
    py_val_t items(stack<Stack_trace_entry> & bt,const SrcPos & p,
                        py_val_t * a); // すべてのKey:値のリスト
    
    /* 各種値を文字列に変換する関数群 */
    py_val_t repr_int(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a);
    py_val_t repr_float(stack<Stack_trace_entry> & bt,const SrcPos & p,
                        py_val_t * a);
    py_val_t repr_ifun(stack<Stack_trace_entry> & bt,const SrcPos & p,
                       py_val_t * a);
    py_val_t repr_nfun(stack<Stack_trace_entry> & bt,const SrcPos & p,
                       py_val_t * a);
    
    py_val_t print_(stack<Stack_trace_entry> & bt,const SrcPos & p,
                    py_val_t * a);
    py_val_t print_string(stack<Stack_trace_entry> & bt,const SrcPos & p,
                    py_val_t * a);
    py_val_t dprint(stack<Stack_trace_entry> & bt,const SrcPos & p,
                    py_val_t * a);

    /* 演算子 */
    py_val_t add(stack<Stack_trace_entry> & bt,const SrcPos & p,
                  py_val_t * a);
     py_val_t prefix_add(stack<Stack_trace_entry> & bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t sub(stack<Stack_trace_entry> & bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t mul(stack<Stack_trace_entry> & bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t div(stack<Stack_trace_entry> & bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t mod(stack<Stack_trace_entry> & bt,const SrcPos & p,
                 py_val_t * a);
    py_val_t invert(stack<Stack_trace_entry> & bt,const SrcPos & p,
                 py_val_t * a);
    py_val_t lshift(stack<Stack_trace_entry> & bt,const SrcPos & p,
                 py_val_t * a);
    py_val_t rshift(stack<Stack_trace_entry> & bt,const SrcPos & p,
              py_val_t * a);
    py_val_t and_(stack<Stack_trace_entry> & bt,const SrcPos & p,
              py_val_t * a);
    py_val_t or_(stack<Stack_trace_entry> & bt,const SrcPos & p,
              py_val_t * a);
    py_val_t xor_(stack<Stack_trace_entry> & bt,const SrcPos & p,
              py_val_t * a);
    py_val_t contains(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a);
    py_val_t not_contains(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a);
    py_val_t not_(stack<Stack_trace_entry> & bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t ntv_is(stack<Stack_trace_entry> & bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t ntv_is_not(stack<Stack_trace_entry> & bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t cmp(stack<Stack_trace_entry> & bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t eq(stack<Stack_trace_entry> & bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t ne(stack<Stack_trace_entry> & bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t gt(stack<Stack_trace_entry> & bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t lt(stack<Stack_trace_entry> & bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t ge(stack<Stack_trace_entry> & bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t le(stack<Stack_trace_entry> & bt,const SrcPos & p,
                  py_val_t * a);
    py_val_t eq_pointer(stack<Stack_trace_entry> & bt,const SrcPos & p,
                       py_val_t * a); //a is b _?(tuple/sict/list)
    py_val_t getitem(stack<Stack_trace_entry> & bt,const SrcPos & p,
                 py_val_t * a);
    py_val_t setitem(stack<Stack_trace_entry> & bt,const SrcPos & p,
                 py_val_t * a);
    py_val_t delitem(stack<Stack_trace_entry> & bt,const SrcPos & p,
                 py_val_t * a);


    /* 変換用の関数たち */

    py_val_t itof(stack<Stack_trace_entry> & bt,const SrcPos & p,
                 py_val_t * a);
    py_val_t str(stack<Stack_trace_entry> & bt,const SrcPos & p,
                 py_val_t * a);
    py_val_t replace_string(stack<Stack_trace_entry> & bt,const SrcPos & p,
                 py_val_t * a);


    py_val_t len(stack<Stack_trace_entry> & bt,const SrcPos & p,
                 py_val_t * a);
        /* リスト用ユーティリティ関数 */
    py_val_t append(stack<Stack_trace_entry> & bt,const SrcPos & p,
                    py_val_t * a);
    py_val_t add_list(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a);
    py_val_t range(stack<Stack_trace_entry> & bt,const SrcPos & p,
                      py_val_t * a);

    // popは,dictとlistでとる引数の数がちがうよん. 注意.
    py_val_t pop(stack<Stack_trace_entry> & bt,const SrcPos & p,
                 py_val_t * a);

    
    py_val_t ntv_random(stack<Stack_trace_entry> & bt,const SrcPos & p,
                        py_val_t * a);
    py_val_t sys_sleep(stack<Stack_trace_entry> & bt,const SrcPos & p,
                   py_val_t * a);

    // experimental multithread implementation
    py_val_t thread_start(stack<Stack_trace_entry> & bt,const SrcPos & p,
                          py_val_t * a);
    py_val_t thread_join(stack<Stack_trace_entry> & bt,const SrcPos & p,
                         py_val_t * a);
    void * thread_nfun_dispatch(void * a);
    void * thread_vfun_dispatch(void * a);
}

#endif

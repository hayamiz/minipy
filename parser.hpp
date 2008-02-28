#ifndef PARSER_HPP
#define PARSER_HPP

#include <queue>
#include <stack>

#include "tokenizer.hpp"
#include "syntax_tree.hpp"

class Parser{
public:
    Parser(string filename);
    ~Parser();
    vector<Stm> file_input();
private:
    Tokenizer tok;

    // eatは, 食べたトークン(this->cur_token)を返すようにした
    token_kind_t eat();
    token_kind_t eat(token_kind_t eat_token_kind);

    token_kind_t cur_token;
    // token_kind_t peek_token();
    token_kind_t next_token();
    SrcPos get_pos();

    void error();

    // 左結合
    // a * a / a * a ... => ((((a * a) / a) * a) ...)
    static Expr left_infix_operator_list_helper(queue<Expr> & expr_list
                                                , queue<SrcPos> & pos_list
                                                , queue<token_kind_t> & opr_list);
    Expr parse_atom();    //
    vector<Expr> parse_expression_list_with_comma(bool & tail_commap); //
    Expr parse_literal(); //
    Expr parse_list_display();   //
    Expr parse_dict_display();   //
    vector<Expr> parse_key_datum_list(); //
    Expr parse_key_datum();//
    
    Expr parse_primary();  //
    Expr parse_u_expr();   //
    Expr parse_m_expr();   //
    Expr parse_a_expr();   //
    Expr parse_shift_expr();//
    Expr parse_and_expr(); //
    Expr parse_xor_expr(); //
    Expr parse_or_expr();  //
    Expr parse_comparison();//
    token_kind_t parse_comp_operator();//
    Expr parse_not_test();//
    Expr parse_and_test();//
    Expr parse_or_test();//
    Expr parse_expression();//

    Stm parse_expression_stmt();//

    Expr parse_subscription();//
    Stm parse_pass_stmt();
    Stm parse_return_stmt();//
    Stm parse_del_stmt();//
    Stm parse_break_stmt();
    Stm parse_continue_stmt();
    Stm parse_global_stmt();//
    Stm parse_print_stmt();//
    vector<Stm> parse_suite();//
    Stm parse_statement();//
    Stm parse_if_stmt();//
    Stm parse_while_stmt();//
    Stm parse_for_stmt();//
    Stm parse_funcdef();//
    vector<string> parse_parameter_list();//
    string parse_funcname();//
    vector<Stm> parse_file_input();//
};

#endif

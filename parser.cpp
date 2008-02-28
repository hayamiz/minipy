#include "parser.hpp"

Parser::Parser(string filename)
    : tok(filename) {
    this->next_token();
}

Parser::~Parser(){
}

vector<Stm> Parser::file_input(){
    return this->parse_file_input();
}

// 私食べるのは大好きだ
token_kind_t Parser::eat(){
    token_kind_t ret = this->cur_token;

    // cerr << this->tok.get_cur_token_string();
    
    this->next_token();

    return ret;
}

// 特に甘いものが好きだ
token_kind_t Parser::eat(token_kind_t eat_token_kind){
    token_kind_t ret = this->cur_token;

    // cerr << this->tok.get_cur_token_string();

    if(eat_token_kind == this->cur_token){
        this->next_token();
    }else{
        this->error();
    }

    return ret;
}

// // 次のトークンをピークする
// token_kind_t Parser::peek_token(){
//     return this->tok.get_cur_token_kind();
// }

// 次のトークンをとってくる
token_kind_t Parser::next_token(){
    this->cur_token = this->tok.get_next();
    return this->cur_token;
}

// ポジション
SrcPos Parser::get_pos(){
    return SrcPos( this->tok.get_filename(),
                   this->tok.get_cur_line_no(),
                   this->tok.get_cur_column_no());
}

//error
void Parser::error(){
    cerr << "syntax error::: "
         << this->tok.get_filename()
         << ":" << this->tok.get_cur_line_no()
         << endl
         << "near '" << this->tok.get_cur_line_string() << "'"
         << endl;
    exit(1);
}

// or_expr := xor_expr ( '|' xor_test )* のような式の結果をExprにまとめる
Expr Parser::left_infix_operator_list_helper(queue<Expr> & expr_list
                                     , queue<SrcPos> & pos_list
                                     , queue<token_kind_t> & opr_list){
    Expr ret = expr_list.front();
    expr_list.pop();
    while(!opr_list.empty() && !expr_list.empty()){
        ret = Expr::make_infix_operator(opr_list.front()
                                        , ret
                                        , expr_list.front()
                                        , pos_list.front());
        opr_list.pop(); expr_list.pop(); pos_list.pop();
    }

    return ret;
}

// アトム
Expr Parser::parse_atom(){
    // identifier | literal 
    // | list_display | dict_display 
    // | "(" expression_list_with_comma ")"

    Expr ret;
    SrcPos pos;
    vector<Expr> expr_list;
    switch (this->cur_token){
    case TOK_ID:   // identifer
        ret = Expr::make_var(this->tok.get_cur_token_str_val(), get_pos());
        this->eat();
        break;
    case TOK_LITERAL_INT: // リテラル
    case TOK_LITERAL_FLOAT:
    case TOK_LITERAL_STRING:
        return this->parse_literal();
        break;
    case TOK_LBRACKET: //[ >>> リスト式
        return this->parse_list_display();
        break;
    case TOK_LBRACE: // { >>> dictionary
        return this->parse_dict_display();
        break;
    case TOK_LPAREN: //( >>>
// # 
// # "(" expression_list_with_comma ")" について
// # expression_list_with_comma は，
// #  式を0個以上カンマで区切りながら並べ，
// #  最後にオプションとして余分なカンマがあっても良い，
// #  というものである．
// #  ただし式の数が0のときは，最後の余分なカンマはあってはいけない
// #  (前方の定義参照)．
// #
// # 以下はどれも正しい atom の例である
// # () 
// # (1)
// # (1,)
// # (1,2)
// # (1,2,)
// # --- そしてこのうち， (1) 以外はどれもタプルである．
// # --- (1) は 1 という式を括弧にくくったものである．
// # 1要素のタプルを作るのに， (1,)という表現を使うことに注意
        this->eat();
        bool tail_commap;
        pos = get_pos();
        expr_list = this->parse_expression_list_with_comma(tail_commap);
        if (expr_list.size() == 1 && !tail_commap){
            ret = Expr::make_paren(expr_list[0], pos);
        } else{
            ret = Expr::make_display_tuple(expr_list, pos);
        }
        
        this->eat(TOK_RPAREN);
        break;
    case TOK_NONE: // NONE
        ret = Expr::make_none(this->get_pos());
        break;
    default:
        this->error();
        break;
    }

    return ret;
}

// これは何だろう <- タプルの中とかに出現する式のリスト
// tail_commapは, リストの最後がcommaで終わってたらtrue
// そうじゃければfalse
vector<Expr> Parser::parse_expression_list_with_comma(bool & tail_commap){
    vector<Expr> ret;
    // [ expression ( "," expression )* [","] ]
    while(true){
        switch (this->cur_token){
        case TOK_ID:   // identifer
        case TOK_LITERAL_INT: // リテラル
        case TOK_LITERAL_FLOAT:
        case TOK_LITERAL_STRING:
        case TOK_LBRACKET: //[ >>> リスト式
        case TOK_LBRACE: // { >>> dictionary
        case TOK_LPAREN: //( >>>
        case TOK_MINUS:   // -
        case TOK_PLUS:   // +
        case TOK_TILDE:   // ~
        case TOK_KW_NOT: // not
            tail_commap = false;
            ret.push_back(this->parse_expression());
            if(this->cur_token == TOK_COMMA){
                this->eat();// [,]
                tail_commap = true;
            }
            break;
        default:
            goto finish;
            break;
        }
    }

finish:
    return ret;
}

// リテラル
Expr Parser::parse_literal(){
    Expr expr;
    // stringliteral | integer | floatnumber
    switch (this->cur_token){
    case TOK_LITERAL_STRING:   // 文字列リテラル
        expr = Expr::make_literal_string
            (this->tok.get_cur_token_str_val()
             ,this->get_pos());
        break;
    case TOK_LITERAL_INT:   // 整数リテラル
        expr = Expr::make_literal_int
            (this->tok.get_cur_token_int_val()
             , this->get_pos());
        break;
    case TOK_LITERAL_FLOAT:   // 浮動小数点リテラル
        expr = Expr::make_literal_float
            (this->tok.get_cur_token_float_val()
             , this->get_pos());
        break;
    default:
        this->error();
    }
    this->eat();
    return expr;
}

// リスト式
Expr Parser::parse_list_display(){
    // "[" expression_list_with_comma "]"
    this->eat(TOK_LBRACKET);// [
    bool tail_commap;
    Expr ret = Expr::make_display_list
        (this->parse_expression_list_with_comma(tail_commap)
         , get_pos());
    this->eat(TOK_RBRACKET);// ]

    return ret;
}

// 辞書式
Expr Parser::parse_dict_display(){
    // "{" key_datum_list "}"
    this->eat(TOK_LBRACE);// {
    Expr ret = Expr::make_display_dict(this->parse_key_datum_list()
                                       , get_pos());
    this->eat(TOK_RBRACE);// }

    return ret;
}

// 辞書式の中身 ////　あとでやる！！！
// #
// # リスト式  [ 式, 式, ... ]
// # タプルと文法を統一するために，最後の余分な , を許す．
// # この , はあってもなくても，
// # 意味は変わらない
// #
vector<Expr> Parser::parse_key_datum_list(){
    // [ key_datum ("," key_datum)* ]
    vector<Expr> expr_list;

    while(true){
        switch (this->cur_token){
        case TOK_ID:   // identifer
        case TOK_LITERAL_INT: // リテラル
        case TOK_LITERAL_FLOAT:
        case TOK_LITERAL_STRING:
        case TOK_LBRACKET: //[ >>> リスト式
        case TOK_LBRACE: // { >>> dictionary
        case TOK_LPAREN: //( >>>
        case TOK_MINUS:   // -
        case TOK_PLUS:   // +
        case TOK_TILDE:   // ~
        case TOK_KW_NOT: // not
        case TOK_NONE: // NONE
            expr_list.push_back(this->parse_key_datum());
            if(this->cur_token == TOK_COMMA){
                this->eat();
            }
            break;
        default:
            goto finish;
            break;
        }
    }
finish:
    return expr_list;
}

// 名前と値
// infix_operatorとして表現するお
Expr Parser::parse_key_datum(){
    // expression ":" expression
    SrcPos pos = get_pos();
    Expr key = this->parse_expression();
    this->eat(TOK_COLON);
    Expr value = this->parse_expression();

    return Expr::make_infix_operator(TOK_COLON, key, value, pos);
}

// #
// # - atom.f は attref式. mini-Pythonではその直後に atom.f(x, y, z) 
// #   と続く場合のみが許されるが，
// #   このことは以下の文法には現れていない(別途チェックする)．
// #   そして，atom.f(x, y, z) は f(atom, x, y, z) と等価としている．
// #
// # - atom[expr]は 構文木ではsubscript 式に対応している．
// #   atom[x,y,z]と来た場合，expr がタプル (x,y,z)である
// #   とみなす．つまり， atom[x,y,z] = atom[(x,y,z)]
// #
// # - atom(x, y, z) は関数呼び出し．構文木では call 
// #
// 以上文法定義より
Expr Parser::parse_primary(){
    //atom ( "." identifier 
    // | "[" expression_list_with_comma "]" 
    // | "(" expression_list_with_comma ")" )*

    Expr ret = this->parse_atom();
    SrcPos pos;
    vector<Expr> expr_list;

    while(this->cur_token == TOK_PERIOD
          || this->cur_token ==TOK_LBRACKET
          || this->cur_token ==TOK_LPAREN){
        switch (this->cur_token){
        case TOK_PERIOD:   // .
            this->eat();
            ret = Expr::make_attref(ret
                                    , this->tok.get_cur_token_str_val()
                                    , get_pos());
            this->eat(TOK_ID);

            // TODO: ε(    v ゜ω゜) atom.f( ... )の形のみを通すよう
            // にチェックせねばならん
            break;
        case TOK_LBRACKET:   // [
            this->eat();
            pos = get_pos();
           bool dummy;
            expr_list = this->parse_expression_list_with_comma(dummy);
            if (expr_list.size() == 1){
                ret = Expr::make_subscript(ret, expr_list[0], pos);
            } else {
                ret = Expr::make_subscript(ret,
                                           Expr::make_list(expr_list,
                                                           pos)
                                           , pos);
            }
            this->eat(TOK_RBRACKET);
            break;
        case TOK_LPAREN:   // (
            this->eat();
            bool tail_commap;
            ret = Expr::make_call(ret
                                  , this->parse_expression_list_with_comma(tail_commap)
                                  , get_pos());
            this->eat(TOK_RPAREN);
            break;
        default:
            break;
        }
    }

    return ret;
}

// 単項演算子
Expr Parser::parse_u_expr(){
    // primary | "-" u_expr | "+" u_expr | "~" u_expr

    token_kind_t opr;
    switch (this->cur_token){
    case TOK_ID:   // identifer
    case TOK_LITERAL_INT: // リテラル
    case TOK_LITERAL_FLOAT:
    case TOK_LITERAL_STRING:
    case TOK_LBRACKET: //[ >>> リスト式
    case TOK_LBRACE: // { >>> dictionary
    case TOK_LPAREN: //( >>>
    case TOK_NONE: // NONE
        return this->parse_primary();
        break;

    case TOK_MINUS:   // -
    case TOK_PLUS:   // +
    case TOK_TILDE:   // ~
        opr = this->cur_token;
        this->eat();
        return Expr::make_prefix_operator(opr, this->parse_u_expr(), get_pos());
        break;
    default:
        this->error();
        break;
    }

    // dummy return. never reachs here;
    this->error();
    return Expr::make_none(get_pos());
}

////////// 多項演算子
//
Expr Parser::parse_m_expr(){
    queue<Expr> expr_list;
    queue<SrcPos> pos_list;
    queue<token_kind_t> opr_list;
    
    // u_expr ( ( "*" | "/" | "%" ) u_expr )*
    pos_list.push(get_pos());
    expr_list.push(this->parse_u_expr());

    while(this->cur_token == TOK_MUL
       || this->cur_token == TOK_DIV
       || this->cur_token == TOK_MOD){
        opr_list.push(this->cur_token);
        this->eat();
        pos_list.push(get_pos());
        expr_list.push(this->parse_u_expr());
    }

    return Parser::left_infix_operator_list_helper(expr_list, pos_list, opr_list);
}

// 
Expr Parser::parse_a_expr(){
    queue<Expr> expr_list;
    queue<SrcPos> pos_list;
    queue<token_kind_t> opr_list;
    
    // m_expr ( ( "+" | "-" ) m_expr )*
    pos_list.push(get_pos());
    expr_list.push(this->parse_m_expr());
    
    while(this->cur_token == TOK_PLUS
       || this->cur_token == TOK_MINUS){
        opr_list.push(this->cur_token);
        this->eat();
        pos_list.push(get_pos());
        expr_list.push(this->parse_m_expr());
    }

    return Parser::left_infix_operator_list_helper(expr_list, pos_list, opr_list);
}

// 
Expr Parser::parse_shift_expr(){
    queue<Expr> expr_list;
    queue<SrcPos> pos_list;
    queue<token_kind_t> opr_list;
    
    // a_expr ( ( "<<" | ">>" ) a_expr )*
    pos_list.push(get_pos());
    expr_list.push(this->parse_a_expr());

    while(this->cur_token == TOK_LSHIFT
       || this->cur_token == TOK_RSHIFT){
        opr_list.push(this->cur_token);
        this->eat();

        pos_list.push(get_pos());
        expr_list.push(this->parse_a_expr());
    }

    return Parser::left_infix_operator_list_helper(expr_list, pos_list, opr_list);
}

// 
Expr Parser::parse_and_expr(){
    queue<Expr> expr_list;
    queue<SrcPos> pos_list;
    queue<token_kind_t> opr_list;
    
    //shift_expr ( "&" shift_expr )*
    pos_list.push(get_pos());
    expr_list.push(this->parse_shift_expr());
    
    while(this->cur_token == TOK_AMP){
        opr_list.push(TOK_AMP);
        this->eat();

        pos_list.push(get_pos());
        expr_list.push(this->parse_shift_expr());
    }

    return Parser::left_infix_operator_list_helper(expr_list, pos_list, opr_list);
}

// 
Expr Parser::parse_xor_expr(){
    queue<Expr> expr_list;
    queue<SrcPos> pos_list;
    queue<token_kind_t> opr_list;

    // and_expr ( "^" and_expr )*
    pos_list.push(get_pos());
    expr_list.push(this->parse_and_expr());
    while(this->cur_token == TOK_XOR){
        opr_list.push(TOK_XOR);
        this->eat();
        pos_list.push(get_pos());
        expr_list.push(this->parse_and_expr());
    }

    return Parser::left_infix_operator_list_helper(expr_list, pos_list, opr_list);
}

// 
Expr Parser::parse_or_expr(){
    //xor_expr ( "|" xor_expr )*
    queue<Expr> expr_list;
    queue<SrcPos> pos_list;
    queue<token_kind_t> opr_list;
    
    pos_list.push(get_pos());
    expr_list.push(this->parse_xor_expr());
    
    while(this->cur_token == TOK_BAR){
        opr_list.push(TOK_BAR);
        this->eat();
        pos_list.push(get_pos());
        expr_list.push(this->parse_xor_expr());
    }

    return Parser::left_infix_operator_list_helper(expr_list, pos_list, opr_list);
}

// あとでやる
Expr Parser::parse_comparison(){
    //or_expr [ comp_operator or_expr ]
    Expr ret = this->parse_or_expr();
    token_kind_t opr;
    switch (this->cur_token){
    case TOK_LT:   // <
    case TOK_GT:   // >
    case TOK_EQ_EQ: // ==
    case TOK_GEQ:   // >=
    case TOK_LEQ:   // <=
    case TOK_NEQ:   // !=
    case TOK_KW_IS:   //is [not]
    case TOK_KW_NOT:   // not in
    case TOK_KW_IN:   // in
        opr = this->parse_comp_operator();
        ret = Expr::make_infix_operator(opr, ret, this->parse_or_expr(), get_pos());
        break;
    default:
        break;
    }

    return ret;
}

// 
token_kind_t Parser::parse_comp_operator(){
    //"<" | ">" | "==" | ">=" | "<=" | "!="
    // | "is" ["not"] | ["not"] "in"
    token_kind_t ret;
    
    switch (this->cur_token){
    case TOK_LT:   // <
    case TOK_GT:   // >
    case TOK_EQ_EQ: // ==
    case TOK_GEQ:   // >=
    case TOK_LEQ:   // <=
    case TOK_NEQ:   // !=
        ret = this->eat();
        break;
    case TOK_KW_IS:   //is [not]
        ret = this->eat();
        if(this->cur_token == TOK_KW_NOT){
            ret = TOK_KW_IS_NOT; //this->eat();
        }
        break;
    case TOK_KW_NOT:   // not in
        this->eat();
        this->eat(TOK_KW_IN);
        ret = TOK_KW_NOT_IN;
        break;
    case TOK_KW_IN:   // in
        ret = this->eat();
        break;
    default:
        this->error();
        break;
    }

    return ret;
}

// 
Expr Parser::parse_not_test(){
    // comparison | "not" not_test
    switch (this->cur_token){
    case TOK_ID:   // identifer
    case TOK_LITERAL_INT: // リテラル
    case TOK_LITERAL_FLOAT:
    case TOK_LITERAL_STRING:
    case TOK_LBRACKET: //[ >>> リスト式
    case TOK_LBRACE: // { >>> dictionary
    case TOK_LPAREN: //( >>>
    case TOK_MINUS:   // -
    case TOK_PLUS:   // +
    case TOK_TILDE:   // ~
    case TOK_NONE: // NONE
        return this->parse_comparison();
        break;
    case TOK_KW_NOT: // not
        this->eat();
        return Expr::make_prefix_operator(TOK_KW_NOT
                                          , this->parse_not_test()
                                          , get_pos());
        break;
    default:
        this->error();
        break;
    }

    // dummy return
    // never reaches here.
    return Expr();
}

// 
Expr Parser::parse_and_test(){
    // not_test ( "and" not_test )*
    queue<Expr> expr_list;
    queue<SrcPos> pos_list;
    queue<token_kind_t> opr_list;

    pos_list.push(get_pos());
    expr_list.push(this->parse_not_test());
    while(this->cur_token == TOK_KW_AND){
        opr_list.push(this->eat());
        pos_list.push(get_pos());
        expr_list.push(this->parse_not_test());
    }
    return Parser::left_infix_operator_list_helper(expr_list
                                           , pos_list
                                           , opr_list);
}

// 
Expr Parser::parse_or_test(){
    // and_test ( "or" and_test )*
    queue<Expr> expr_list;
    queue<SrcPos> pos_list;
    queue<token_kind_t> opr_list;

    pos_list.push(get_pos());
    expr_list.push(this->parse_and_test());
    while(this->cur_token == TOK_KW_OR){
        opr_list.push(this->eat());
        pos_list.push(get_pos());
        expr_list.push(this->parse_and_test());
    }

    return Parser::left_infix_operator_list_helper(expr_list
                                           , pos_list
                                           , opr_list);
}

// 
Expr Parser::parse_expression(){
    // or_test
    return this->parse_or_test();
}

// 
Stm Parser::parse_expression_stmt(){
    // expression
    return Stm::make_expression(this->parse_expression()
                                , this->get_pos());
}

// obsolete
Expr Parser::parse_subscription(){
    // primary "[" expression_list_with_comma "]"
    Expr expr;
    expr = this->parse_primary();
    if (expr.kind != expr_kind_subscript
        && expr.kind != expr_kind_var) {
        this->error();
    }
    return expr;
    // 上でパースしたprimaryが,subscriptionに適格かどうか
    // つまり,最後が [ .. ] で終わっているかどうかを本当はチェックしなきゃいけない
//     this->eat(TOK_LBRACKET);
//     this->parse_expression_list_with_comma();
//     this->eat(TOK_RBRACKET);
}

// 
Stm Parser::parse_return_stmt(){
    // "return" [expression_list_with_comma]
    this->eat(TOK_KW_RETURN);
    bool tail_commap;
    vector<Expr> ret;
    
    switch (this->cur_token){
    case TOK_ID:   // identifer
    case TOK_LITERAL_INT: // リテラル
    case TOK_LITERAL_FLOAT:
    case TOK_LITERAL_STRING:
    case TOK_LBRACKET: //[ >>> リスト式
    case TOK_LBRACE: // { >>> dictionary
    case TOK_LPAREN: //( >>>
    case TOK_MINUS:   // -
    case TOK_PLUS:   // +
    case TOK_TILDE:   // ~
    case TOK_KW_NOT: // not

        // parse_expression_list_with_comma は, vector<Expr> 型
        // Python では, return foo, bar, baz のように書くことができる
        // この場合, 戻り値は (foo, bar, baz) というタプルになる

        // 構文木を作る場合, カンマ区切りで複数の戻り値がある場合は,
        // Expr::make_display_tuple を使っておくのがよさそう

        ret = this->parse_expression_list_with_comma(tail_commap);
        if(ret.size() == 1){
            return Stm::make_return(ret[0], this->get_pos());
        }else{
            return Stm::make_return(
                Expr::make_list(ret, this->get_pos()),
                this->get_pos());
        }
        break;
    default:
        // return のみの場合
        return Stm::make_return(Expr::make_none(this->get_pos())
                                , this->get_pos());
        break;
    }
    // dummy return
    // never reaches here.
    return Stm();
}

// 
Stm Parser::parse_pass_stmt(){
    // "pass"
    this->eat(TOK_KW_PASS);
    return Stm::make_pass(this->get_pos());
}

// 
Stm Parser::parse_del_stmt(){
    // "del" subscription
    this->eat(TOK_KW_DEL);
    return  Stm::make_del(this->parse_subscription(), this->get_pos());
}

// 
Stm Parser::parse_break_stmt(){
    // "break"
    this->eat(TOK_KW_BREAK);
    return Stm::make_break(this->get_pos());
}

// 
Stm Parser::parse_continue_stmt(){
    // "continue"
    this->eat(TOK_KW_CONTINUE);
    return Stm::make_continue(this->get_pos());
}

// 
Stm Parser::parse_global_stmt(){
    Stm stm;
    // "global" identifier
    this->eat(TOK_KW_GLOBAL);
    stm = Stm::make_global(this->tok.get_cur_token_str_val()
                           ,this->get_pos() );
    this->eat(TOK_ID);
    return stm;
}

// 
Stm Parser::parse_print_stmt(){
    // "print" expression_list_with_comma
    bool tail_commap;
    vector<Expr> ret;
    this->eat(TOK_KW_PRINT);

    ret = this->parse_expression_list_with_comma(tail_commap);
    if(ret.size() == 1){
        return Stm::make_print(ret[0], this->get_pos());
    }else{
        return Stm::make_print(
            Expr::make_list(ret, this->get_pos()),
            this->get_pos());
    }
}

// 
vector<Stm> Parser::parse_suite(){
    // NEWLINE INDENT statement+ DEDENT
    this->eat(TOK_NEWLINE);
    this->eat(TOK_INDENT);

    vector<Stm> stm_list;
    bool loop = true;
    while(loop){
        switch (this->cur_token){
        case TOK_ID:   // identifer
        case TOK_LITERAL_INT: // リテラル
        case TOK_LITERAL_FLOAT:
        case TOK_LITERAL_STRING:
        case TOK_LBRACKET: //[ >>> リスト式
        case TOK_LBRACE: // { >>> dictionary
        case TOK_LPAREN: //( >>>
        case TOK_MINUS:   // -
        case TOK_PLUS:   // +
        case TOK_TILDE:   // ~
        case TOK_KW_NOT: // not
        case TOK_KW_PASS: //  | pass_stmt NEWLINE
        case TOK_KW_BREAK://  | break_stmt NEWLINE
        case TOK_KW_CONTINUE: //  | continue_stmt NEWLINE
        case TOK_KW_RETURN://  | return_stmt NEWLINE
        case TOK_KW_DEL: //  | del_stmt NEWLINE
        case TOK_KW_GLOBAL: //  |  global_stmt NEWLINE
        case TOK_KW_PRINT://  | print_stmt NEWLINE
        case TOK_KW_IF: // | if_stmt
        case TOK_KW_WHILE: //  | while_stmt
        case TOK_KW_FOR://  | for_stmt
        case TOK_KW_DEF://  | funcdef
            stm_list.push_back(this->parse_statement());
            break;
        default:
            loop = false;
            break;
        }
    }
    
    this->eat(TOK_DEDENT);
    return stm_list;
}

// 
Stm Parser::parse_statement(){
    // expression_stmt NEWLINE
    //  | assignment_stmt NEWLINE
    //  | pass_stmt NEWLINE
    //  | return_stmt NEWLINE
    //  | del_stmt NEWLINE
    //  | break_stmt NEWLINE
    //  | continue_stmt NEWLINE
    //  | global_stmt NEWLINE
    //  | print_stmt NEWLINE
    //  | if_stmt
    //  | while_stmt
    //  | for_stmt
    //  | funcdef

    Expr expr;
    Stm stm;
    SrcPos cur_pos = this->get_pos();
    
    switch (this->cur_token){
     // ====================ここから===================
     // expression とassignmentを見分けるためには
     // 初めのトークンの次のトークンが＝かどうかで判断する必要が
    // とりあえず熱があるから今日はこの面倒くさいのを考える余力なす
    // すまぬ。あとコンストラクタが変な以外はコンパイル通った
    case TOK_ID:   // identifer
    case TOK_LITERAL_INT: // リテラル
    case TOK_LITERAL_FLOAT:
    case TOK_LITERAL_STRING:
    case TOK_LBRACKET: //[ >>> リスト式
    case TOK_LBRACE: // { >>> dictionary
    case TOK_LPAREN: //( >>>
    case TOK_NONE: // NONE
        // この expression が, assignmentの左辺の可能性がある
        expr = this->parse_expression();
        
        switch(this->cur_token){
        case TOK_NEWLINE:
            stm =  Stm::make_expression(expr
                                        , cur_pos);
            this->eat();
            break;
        case TOK_EQ:
            if (expr.kind == expr_kind_subscript
                || expr.kind == expr_kind_var) {
                this->eat();
                stm = Stm::make_assignment(expr
                                           ,this->parse_expression()
                                           , cur_pos);
                this->eat(TOK_NEWLINE);
            } else {
                this->error();
            }
            break;
        default:
            this->error();
            break;
        }
        break;
    case TOK_MINUS:   // -
    case TOK_PLUS:   // +
    case TOK_TILDE:   // ~
    case TOK_KW_NOT: // not
        // expression_stmt NEWLINE
        stm = this->parse_expression_stmt();
        this->eat(TOK_NEWLINE);
        break;
        // ====================ここまで===================
    case TOK_KW_PASS: //  | pass_stmt NEWLINE
        stm = parse_pass_stmt();
        this->eat(TOK_NEWLINE);
        break;
    case TOK_KW_BREAK://  | break_stmt NEWLINE
         stm = parse_break_stmt();
        this->eat(TOK_NEWLINE);
        break;      
    case TOK_KW_CONTINUE: //  | continue_stmt NEWLINE
        stm = parse_continue_stmt();
        this->eat(TOK_NEWLINE);
        break;    
    case TOK_KW_RETURN://  | return_stmt NEWLINE
        stm = this->parse_return_stmt();
        this->eat(TOK_NEWLINE);
        break;
    case TOK_KW_DEL: //  | del_stmt NEWLINE
        stm = this->parse_del_stmt();
        this->eat(TOK_NEWLINE);
        break;
    case TOK_KW_GLOBAL: //  |  global_stmt NEWLINE
        stm = this->parse_global_stmt();
        this->eat(TOK_NEWLINE);
        break;
    case TOK_KW_PRINT://  | print_stmt NEWLINE
        stm = this->parse_print_stmt();
        this->eat(TOK_NEWLINE);
        break;
    case TOK_KW_IF: // | if_stmt
        stm = this->parse_if_stmt();
        break;
    case TOK_KW_WHILE: //  | while_stmt
        stm = this->parse_while_stmt();
        break;
    case TOK_KW_FOR://  | for_stmt
        stm = this->parse_for_stmt();
        break;
    case TOK_KW_DEF://  | funcdef
        stm = this->parse_funcdef();
        break;
    default:
        break;
    }
    return stm;
}

// 
Stm Parser::parse_if_stmt(){
    // "if" expression ":" suite
    //  ( "elif" expression ":" suite )*
    //  ["else" ":" suite]
    vector<StmIfBranch> ifBranch;
    
    Expr e;
    vector<Stm> s;
    SrcPos cur_pos = this->get_pos();
    
    this->eat(TOK_KW_IF);
    e = this->parse_expression();
    this->eat(TOK_COLON);
    s = this->parse_suite();
    ifBranch.push_back(StmIfBranch(e, s));
    
    while(this->cur_token == TOK_KW_ELIF){
            this->eat();
            e = this->parse_expression();
            this->eat(TOK_COLON);
            s = this->parse_suite();
            ifBranch.push_back(StmIfBranch(e, s));
    }
    if(this->cur_token == TOK_KW_ELSE){
        this->eat();
        this->eat(TOK_COLON);
        s = this->parse_suite();
        ifBranch.push_back(StmIfBranch(s));
    }
    return Stm::make_if(ifBranch, cur_pos);
}

// 
Stm Parser::parse_while_stmt(){
    // "while" expression ":" suite
    this->eat(TOK_KW_WHILE);
    SrcPos cur_pos = this->get_pos();
    Expr e = this->parse_expression();
    this->eat(TOK_COLON);
    vector<Stm> b = this->parse_suite();
    return Stm::make_while(e, b, cur_pos);
}

// 
Stm Parser::parse_for_stmt(){
    // "for" identifier "in" expression ":" suite
    this->eat(TOK_KW_FOR);
    SrcPos cur_pos = this->get_pos();
    string x = this->tok.get_cur_token_str_val();
    this->eat(TOK_ID);
    this->eat(TOK_KW_IN);
    Expr e = this->parse_expression();
    this->eat(TOK_COLON);
    vector<Stm> b = this->parse_suite();
    return  Stm::make_for(x, e, b, cur_pos);
}

// 
Stm Parser::parse_funcdef(){
    // "def" funcname "(" parameter_list ")" ":" suite
    this->eat(TOK_KW_DEF);
    SrcPos cur_pos = this->get_pos();
    string f = this->parse_funcname();
    this->eat(TOK_LPAREN);
    vector<string> p =this->parse_parameter_list();
    this->eat(TOK_RPAREN);
    this->eat(TOK_COLON);
    vector<Stm> b = this->parse_suite();
    return Stm::make_fundef(f, p, b, cur_pos);
}
// 
vector<string> Parser::parse_parameter_list(){
    //  [ identifier ("," identifier)* ]
    vector<string> parameter_list;
    if(this->cur_token == TOK_ID){
        parameter_list.push_back(this->tok.get_cur_token_str_val());
        this->eat();
        while(this->cur_token == TOK_COMMA){
            this->eat();
            parameter_list.push_back(this->tok.get_cur_token_str_val());
            this->eat(TOK_ID);
        }
    }
    return parameter_list;
}

// 
string Parser::parse_funcname(){
    // identifier
    string ret = this->tok.get_cur_token_str_val();
    this->eat(TOK_ID);
    return ret;
}

// 
vector<Stm> Parser::parse_file_input(){
    // (NEWLINE | statement)* EOF
    vector<Stm> stm_list;
    while(this->cur_token != TOK_EOF){
        switch (this->cur_token){
        case TOK_NEWLINE:
            this->eat();
            break;
        case TOK_ID:   // identifer
        case TOK_LITERAL_INT: // リテラル
        case TOK_LITERAL_FLOAT:
        case TOK_LITERAL_STRING:
        case TOK_LBRACKET: //[ >>> リスト式
        case TOK_LBRACE: // { >>> dictionary
        case TOK_LPAREN: //( >>>
        case TOK_MINUS:   // -
        case TOK_PLUS:   // +
        case TOK_TILDE:   // ~
        case TOK_KW_NOT: // not
        case TOK_KW_PASS: 
        case TOK_KW_BREAK:
        case TOK_KW_CONTINUE:
        case TOK_KW_RETURN:
        case TOK_KW_DEL: 
        case TOK_KW_GLOBAL:
        case TOK_KW_PRINT:
        case TOK_KW_IF: 
        case TOK_KW_WHILE:
        case TOK_KW_FOR:
        case TOK_KW_DEF:
        case TOK_NONE: // NONE
            stm_list.push_back(this->parse_statement());
            break;
        default:
            break;
        }
    }

    return stm_list;
}

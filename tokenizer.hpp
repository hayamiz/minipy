#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <cctype>
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <stack>
#include <queue>
#include "char_stream.hpp"

using namespace std;

typedef enum{
    TOK_UNRECOGNIZED, // tokenを読み取れなかったとき(lexical error)
    
    TOK_EQ,    // =
    TOK_EQ_EQ, // ==
    TOK_NEQ,   // !=
    TOK_GT,    // >
    TOK_GEQ,   // >=
    TOK_LT,    // <
    TOK_LEQ,   // <=
    TOK_PLUS,  // +
    TOK_MINUS, // -
    TOK_MUL,   // *
    TOK_DIV,   // /
    TOK_MOD,   // %
    TOK_TILDE, // ~
    TOK_LSHIFT,// <<
    TOK_RSHIFT,// >>
    TOK_XOR,   // ^
    TOK_AMP,   // &
    TOK_BAR,   // |

    TOK_NONE,

    TOK_KW_BREAK,
    TOK_KW_CONTINUE,
    TOK_KW_PASS,
    TOK_KW_RETURN,
    TOK_KW_DEL,
    TOK_KW_PRINT,
    TOK_KW_GLOBAL,
    TOK_KW_IF,
    TOK_KW_ELIF,
    TOK_KW_ELSE,
    TOK_KW_FOR,
    TOK_KW_WHILE,
    TOK_KW_DEF,
    TOK_KW_OR, // or
    TOK_KW_AND,// and
    TOK_KW_NOT,// not
    TOK_KW_IS, // is
    TOK_KW_IN, // in

    // 構文木を作るために, 便宜的に導入
    TOK_KW_IS_NOT,
    TOK_KW_NOT_IN,

    TOK_LPAREN,  // (
    TOK_RPAREN,  // )
    TOK_LBRACE,  // {
    TOK_RBRACE,  // }
    TOK_LBRACKET,// [
    TOK_RBRACKET,// ]
    TOK_PERIOD,  // ,
    TOK_COMMA,   // .
    TOK_COLON,   // :

    TOK_LITERAL_INT,
    TOK_LITERAL_FLOAT,
    TOK_LITERAL_STRING,
    
    TOK_ID,

    TOK_NEWLINE,
    TOK_INDENT,
    TOK_DEDENT,

    TOK_EOF,
}token_kind_t;

class Tokenizer{
private:
    CharStream charstream;
    token_kind_t cur_token_kind;//現在の字句の種類
    token_kind_t prev_token_kind; //1つ前のtokenの種類.indent/dedentの開始判定に使用
    string cur_token_str_val;//現在のリテラル/identiferの内容/名前
    int cur_token_int_val;//現在の整数リテラルの内容
    double cur_token_float_val;//現在の浮動小数リテラルの内容

    map<string, token_kind_t> keywords; // 予約語とtoken_kind_tのマップ

    stack<int> indent_stack; // インデントの深さ管理用スタック
    int cur_line_indent; // 現在行の,行頭からのスペースの数

    string cur_token_string;//現在構築中の字句（文字列）
    string cur_token_kind_string;//現在の字句の種類（文字列）

    Tokenizer& tokenize_init(); // 次のtokenを読むための初期化メソッド.get_nextから呼ぶ
    Tokenizer& tokenize_finish(); // tokenを読みおわった後処理のメソッド

    Tokenizer& tokenize_string();//
    Tokenizer& tokenize_numeric();//
    Tokenizer& tokenize_identifier();//

    Tokenizer& error(string errmsg);
public:
    Tokenizer(string filename);
    // 現在の字句の種類をかえす
    token_kind_t get_cur_token_kind();
    // 現在のリテラル/identiferの中身/名前をかえす
    string get_cur_token_str_val();
    // 現在の整数リテラル値を返す
    int get_cur_token_int_val();
    // 現在の浮動小数点リテラル値をかえす
    double get_cur_token_float_val();
    
    // 次の字句を読み込む
    token_kind_t get_next();
    
    // 現在読み込んでいるファイル名をかえす
    string get_filename();
    // 現在の行番号をかえす
    int get_cur_line_no();
    // 現在のカラム番号をかえす
    int get_cur_column_no();
    // 現在の行の先頭から読み込んでいる文字までを返す
    string get_cur_line_string();
    // 現在の字句を文字列として返す
    string get_cur_token_string();
    // 現在の字句の種類を文字列として返す
    string get_cur_token_kind_string();

    // 現在の字句を標準出力へ出力する  ex. TOK_ID(foo)
    Tokenizer& print_cur_token();
};


#endif

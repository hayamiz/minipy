#include "tokenizer.hpp"

Tokenizer::Tokenizer(string filename)
    : charstream(filename) {

    this->keywords["None"] = TOK_NONE;
    // generated by generator.el
    this->keywords["break"] = TOK_KW_BREAK;
    this->keywords["continue"] = TOK_KW_CONTINUE;
    this->keywords["pass"] = TOK_KW_PASS;
    this->keywords["return"] = TOK_KW_RETURN;
    this->keywords["del"] = TOK_KW_DEL;
    this->keywords["print"] = TOK_KW_PRINT;
    this->keywords["global"] = TOK_KW_GLOBAL;
    this->keywords["if"] = TOK_KW_IF;
    this->keywords["elif"] = TOK_KW_ELIF;
    this->keywords["else"] = TOK_KW_ELSE;
    this->keywords["for"] = TOK_KW_FOR;
    this->keywords["while"] = TOK_KW_WHILE;
    this->keywords["def"] = TOK_KW_DEF;
    this->keywords["or"] = TOK_KW_OR;
    this->keywords["and"] = TOK_KW_AND;
    this->keywords["not"] = TOK_KW_NOT;
    this->keywords["is"] = TOK_KW_IS;
    this->keywords["in"] = TOK_KW_IN;

    this->cur_line_indent = 0; // 一応初期化する
    this->indent_stack.push(0); // こちらも初期化
}

// 現在の字句の種類をかえす
token_kind_t Tokenizer::get_cur_token_kind(){
    return this->cur_token_kind;
}

// 現在のリテラル/identiferの中身/名前をかえす
string Tokenizer::get_cur_token_str_val(){
    return this->cur_token_str_val;
}

// 現在の整数リテラル値を返す
int Tokenizer::get_cur_token_int_val(){
    return this->cur_token_int_val;
}

// 現在の浮動小数点リテラル値をかえす
double Tokenizer::get_cur_token_float_val(){
    return this->cur_token_float_val;
}

Tokenizer& Tokenizer::tokenize_init(){
    this->cur_token_string = "";
    this->cur_token_str_val = "";
    this->prev_token_kind = this->cur_token_kind;
    this->cur_token_kind = TOK_UNRECOGNIZED;

    return *this;
}
Tokenizer& Tokenizer::tokenize_finish(){

    return *this;
}

// 次の字句を読み込む
token_kind_t Tokenizer::get_next(){
    this->tokenize_init();

    do {
        while(this->charstream.peek() == ' '){
            if (this->cur_line_indent == this->get_cur_column_no()){
                this->cur_line_indent++;
            }
            this->charstream.get_next_char();
        }

        int comment_length = 0;
        //comment
        if(this->charstream.peek() == '#'){

            // 改行の手前まですすむ
            while(this->charstream.peek() != '\n'
                  && this->charstream.peek() != '\r'){
                this->charstream.get_next_char();
                comment_length++;
            }
            
//             // 今の行にコメントしか無い場合
//             // その行はなかったことに
//             if (this->get_cur_column_no() == this->cur_line_indent){
//                 this->charstream.get_next_char(); // 改行読み捨て
//                 this->cur_line_indent = 0;
//                 comment_length = 0;
//             }
        }
        
        // 空の行について
        if (this->get_cur_column_no() == (this->cur_line_indent + comment_length)
            && (this->charstream.peek() == '\n'
                || this->charstream.peek() == '\r')){
            if (this->charstream.peek() == '\r'){
                this->charstream.get_next_char(); // '\r'読みすて
            }
            if (this->charstream.peek() == '\n'){
                this->charstream.get_next_char(); // '\n'読みすて
            }
            this->cur_line_indent = 0;
        }
    } while(this->charstream.peek() == ' '
            || this->charstream.peek() == '#');

    // indent/dedent
    // 現在行のカラム数と,先頭からのインデント数がおなじだったら
    if (this->get_cur_column_no() == this->cur_line_indent){
        // 前の行のインデントを取得
        int prev_line_indent = this->indent_stack.top();

        // 今の行のほうがインデントが深い場合
        if (this->cur_line_indent > prev_line_indent){
            this->indent_stack.push(this->cur_line_indent);
            return this->cur_token_kind = TOK_INDENT;
        }

        // 今の行のほうがインデントが浅い場合
        if (this->cur_line_indent < prev_line_indent){
            this->indent_stack.pop();
            
            if (this->cur_line_indent > this->indent_stack.top()){
                ostringstream str;
                str << "bad indent level";
                this->error(str.str());
            }

            return this->cur_token_kind = TOK_DEDENT;
        }
    }
    
    char c = this->charstream.get_next_char();

    this->cur_token_string += c;
    
    switch (c){
    case EOF:
        this->cur_token_kind = TOK_EOF;
        break;
    case '(':
        this->cur_token_kind = TOK_LPAREN;
        break;
    case ')':
        this->cur_token_kind = TOK_RPAREN;
        break;
    case '{':
        this->cur_token_kind = TOK_LBRACE;
        break;
    case '}':
        this->cur_token_kind = TOK_RBRACE;
        break;
    case '[':
        this->cur_token_kind = TOK_LBRACKET;
        break;
    case ']':
        this->cur_token_kind = TOK_RBRACKET;
        break;
    case '.':
        if (isdigit(this->charstream.peek())){
            this->tokenize_numeric();
        }else{
            this->cur_token_kind = TOK_PERIOD;
        }
        break;
    case ',':
        this->cur_token_kind = TOK_COMMA;
        break;
    case ':':
        this->cur_token_kind = TOK_COLON;
        break;
    case '=':
        if (this->charstream.peek() == '='){
            this->charstream.get_next_char();
            this->cur_token_kind = TOK_EQ_EQ;
            this->cur_token_string += '=';
        }else{
            this->cur_token_kind = TOK_EQ;
        }
        break;
    case '!':
        if (this->charstream.get_next_char() == '='){
            this->cur_token_kind = TOK_NEQ;
            this->cur_token_string += '=';
        }else{
            // error
        }
        break;
    case '>':
        switch(this->charstream.peek()){
        case '=':
            this->cur_token_kind = TOK_GEQ;
            this->cur_token_string += '=';
            this->charstream.get_next_char();
            break;
        case '>':
            this->cur_token_kind = TOK_RSHIFT;
            this->cur_token_string += '>';
            this->charstream.get_next_char();
            break;
        default:
            this->cur_token_kind = TOK_GT;
        }
        break;
    case '<':
        switch(this->charstream.peek()){
        case '=':
            this->cur_token_kind = TOK_LEQ;
            this->cur_token_string += '=';
            this->charstream.get_next_char();
            break;
        case '<':
            this->cur_token_kind = TOK_LSHIFT;
            this->cur_token_string += '<';
            this->charstream.get_next_char();
            break;
        default:
            this->cur_token_kind = TOK_LT;
        }
        break;
    case '*':
        this->cur_token_kind = TOK_MUL;
        break;
    case '/':
        this->cur_token_kind = TOK_DIV;
        break;
    case '%':
        this->cur_token_kind = TOK_MOD;
        break;
    case '~':
        this->cur_token_kind = TOK_TILDE;
        break;
    case '&':
        this->cur_token_kind = TOK_AMP;
        break;
    case '+':
        this->cur_token_kind = TOK_PLUS;
        break;
    case '-':
        this->cur_token_kind = TOK_MINUS;
        break;
    case '|':
        this->cur_token_kind = TOK_BAR;
        break;
    case '^':
        this->cur_token_kind = TOK_XOR;
        break;
    case '"':
        this->tokenize_string();
        break;
    case '\n':
        this->cur_token_kind = TOK_NEWLINE;
        this->cur_line_indent = 0;
        break;
    case '\r':
        if (this->charstream.peek() == '\n'){
            this->charstream.get_next_char();
        }
        this->cur_token_kind = TOK_NEWLINE;
        this->cur_line_indent = 0;
        break;
    default:
        if (isdigit(c)){
            this->tokenize_numeric();
        } else if (isalpha(c) || c == '_'){
            this->tokenize_identifier();
        }
    }

    if (this->cur_token_kind == TOK_UNRECOGNIZED){
        unsigned int ch = (unsigned int)c;
        ostringstream str;
        str << "invalid character '"
            << c
            << "' (ascii code:"
            << ch
            << ")";
        this->error(str.str());
    }

    return this->cur_token_kind;
}

// stringの最後の"まで読む
Tokenizer& Tokenizer::tokenize_string(){
    string tmp = "";
    while(this->charstream.peek() != '"'){
          if(this->charstream.peek()=='\\'){
              char c = this->charstream.get_next_char();
              if (this->charstream.peek() == 'n'){
                  
                  this->charstream.get_next_char();
                  tmp += '\n';
                  this->cur_token_string += '\n';
              } else {
                  tmp += c;
                  this->cur_token_string += this->charstream.get_cur_char();

                  tmp += this->charstream.get_next_char();
                  this->cur_token_string += this->charstream.get_cur_char();
              }
          }else{
              tmp += this->charstream.get_next_char();
              this->cur_token_string += this->charstream.get_cur_char();
          }
    }
    
    this->cur_token_string +=  this->charstream.get_next_char();
    this->cur_token_kind = TOK_LITERAL_STRING;
    this->cur_token_str_val = tmp;
    return *this;
}

// 整数か浮動小数
Tokenizer& Tokenizer::tokenize_numeric(){
    string tmp = "";
    tmp += this->charstream.get_cur_char();

    while(isdigit(this->charstream.peek())){
         tmp += this->charstream.get_next_char();
    }
    if(this->charstream.peek() == '.'){
        // 浮動小数点
        tmp += this->charstream.get_next_char();
        while(isdigit(this->charstream.peek())){
            tmp += this->charstream.get_next_char();
        }
        this->cur_token_float_val = strtod(tmp.c_str(), NULL);
        this->cur_token_kind = TOK_LITERAL_FLOAT;
    }else{
        // 初めが.の浮動小数の扱い
        if (tmp[0] == '.'){
            this->cur_token_float_val = strtod(tmp.c_str(), NULL);
            this->cur_token_kind = TOK_LITERAL_FLOAT;
        }else{
            if (tmp.length() > 1 && tmp[0] == '0'){
                this->error(string("malformed integer '") + tmp + "'");
            }
            this->cur_token_int_val = (int)(strtol(tmp.c_str(), NULL, 10) );
            this->cur_token_kind = TOK_LITERAL_INT;
        }
    }
    
    this->cur_token_string = tmp;

    return *this;
}

// identifer
Tokenizer& Tokenizer::tokenize_identifier(){
    string tmp = this->cur_token_string;
    while(isalpha(this->charstream.peek())
          || isdigit(this->charstream.peek())
          || this->charstream.peek() == '_'){
        tmp += this->charstream.get_next_char();
    }
    this->cur_token_str_val+= tmp;
    this->cur_token_string = tmp;

    if (this->keywords.find(this->cur_token_string)
        != this->keywords.end()){
        this->cur_token_kind = this->keywords[this->cur_token_string];
    }else{
        this->cur_token_kind = TOK_ID;
    }
    
    return *this;
}


// 現在読み込んでいるファイル名をかえす
string Tokenizer::get_filename(){
    return this->charstream.get_filename();
}

// 現在の行番号をかえす
int Tokenizer::get_cur_line_no(){
    return this->charstream.get_cur_line();
}

// 現在のカラム番号をかえす
int Tokenizer::get_cur_column_no(){
    return this->charstream.get_cur_column();
}

// 現在の行の先頭から読み込んでいる文字までを返
string Tokenizer:: get_cur_line_string(){
    return this->charstream.get_cur_line_string();
}

// 現在の字句を文字列として返す
string Tokenizer::get_cur_token_string(){
    return this->cur_token_string;
}

// 現在の字句の種類を文字列として返す
string Tokenizer::get_cur_token_kind_string(){
    return this->cur_token_kind_string;
}

Tokenizer& Tokenizer::print_cur_token(){
    
    // generated by generator.el
    switch(this->get_cur_token_kind()){
        // (minipy-insert-pretty-print)
case TOK_RPAREN: cout << "TOK_RPAREN" << endl; break;
case TOK_LPAREN: cout << "TOK_LPAREN" << endl; break;
case TOK_LBRACE: cout << "TOK_LBRACE" << endl; break;
case TOK_RBRACE: cout << "TOK_RBRACE" << endl; break;
case TOK_RBRACKET: cout << "TOK_RBRACKET" << endl; break;
case TOK_LBRACKET: cout << "TOK_LBRACKET" << endl; break;
case TOK_PERIOD: cout << "TOK_PERIOD" << endl; break;
case TOK_COMMA: cout << "TOK_COMMA" << endl; break;
case TOK_COLON: cout << "TOK_COLON" << endl; break;
case TOK_EQ_EQ: cout << "TOK_EQ_EQ" << endl; break;
case TOK_EQ: cout << "TOK_EQ" << endl; break;
case TOK_NEQ: cout << "TOK_NEQ" << endl; break;
case TOK_GEQ: cout << "TOK_GEQ" << endl; break;
case TOK_RSHIFT: cout << "TOK_RSHIFT" << endl; break;
case TOK_GT: cout << "TOK_GT" << endl; break;
case TOK_LEQ: cout << "TOK_LEQ" << endl; break;
case TOK_LSHIFT: cout << "TOK_LSHIFT" << endl; break;
case TOK_LT: cout << "TOK_LT" << endl; break;
case TOK_MUL: cout << "TOK_MUL" << endl; break;
case TOK_DIV: cout << "TOK_DIV" << endl; break;
case TOK_MOD: cout << "TOK_MOD" << endl; break;
case TOK_TILDE: cout << "TOK_TILDE" << endl; break;
case TOK_AMP: cout << "TOK_AMP" << endl; break;
case TOK_PLUS: cout << "TOK_PLUS" << endl; break;
case TOK_MINUS: cout << "TOK_MINUS" << endl; break;
case TOK_BAR: cout << "TOK_BAR" << endl; break;
case TOK_XOR: cout << "TOK_XOR" << endl; break;
case TOK_KW_OR: cout << "TOK_KW_OR" << endl; break;
case TOK_KW_AND: cout << "TOK_KW_AND" << endl; break;
case TOK_KW_NOT: cout << "TOK_KW_NOT" << endl; break;
case TOK_KW_IS: cout << "TOK_KW_IS" << endl; break;
case TOK_KW_IN: cout << "TOK_KW_IN" << endl; break;
case TOK_KW_BREAK: cout << "TOK_KW_BREAK" << endl; break;
case TOK_KW_CONTINUE: cout << "TOK_KW_CONTINUE" << endl; break;
case TOK_KW_PASS: cout << "TOK_KW_PASS" << endl; break;
case TOK_KW_RETURN: cout << "TOK_KW_RETURN" << endl; break;
case TOK_KW_DEL: cout << "TOK_KW_DEL" << endl; break;
case TOK_KW_PRINT: cout << "TOK_KW_PRINT" << endl; break;
case TOK_KW_GLOBAL: cout << "TOK_KW_GLOBAL" << endl; break;
case TOK_KW_IF: cout << "TOK_KW_IF" << endl; break;
case TOK_KW_ELIF: cout << "TOK_KW_ELIF" << endl; break;
case TOK_KW_ELSE: cout << "TOK_KW_ELSE" << endl; break;
case TOK_KW_FOR: cout << "TOK_KW_FOR" << endl; break;
case TOK_KW_WHILE: cout << "TOK_KW_WHILE" << endl; break;
case TOK_KW_DEF: cout << "TOK_KW_DEF" << endl; break;
case TOK_LITERAL_INT: cout << "TOK_LITERAL_INT" << " (" << this->cur_token_string << ")" << endl; break;
case TOK_LITERAL_FLOAT: cout << "TOK_LITERAL_FLOAT" << " (" << this->cur_token_string << ")" << endl; break;
case TOK_LITERAL_STRING: cout << "TOK_LITERAL_STRING" << " (" << this->cur_token_string << ")" << endl; break;
case TOK_ID: cout << "TOK_ID" << " (" << this->cur_token_string << ")" << endl; break;
case TOK_NEWLINE: cout << "TOK_NEWLINE" << endl; break;
case TOK_INDENT: cout << "TOK_INDENT" << endl; break;
case TOK_DEDENT: cout << "TOK_DEDENT" << endl; break;
case TOK_NONE: cout << "TOK_NONE" << endl; break;
case TOK_EOF: cout << "TOK_EOF" << endl; break;
case TOK_UNRECOGNIZED: cout << "TOK_UNRECOGNIZED" << " (" << this->cur_token_string << ")" << endl; break;
// ---- pretty print ----
    default:
        break;
    }

    return *this;
}


Tokenizer& Tokenizer::error(string errmsg){
    cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl
         << "!!!!!!!!!!!! Syntax Error !!!!!!!!!!!!" << endl
         << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    cerr << this->charstream.get_filename()
         << ":" << this->charstream.get_cur_line() << ": "
         << errmsg << endl;
    while(this->charstream.peek() != '\n'
          && this->charstream.peek() != EOF)
    {this->charstream.get_next_char();}
    cerr << "code aroud:" << endl
         << this->charstream.get_cur_line_string() << endl
         << "fatal error("
         << __FILE__
         << ":" << __LINE__
         << ")"
         << endl;

    // indent error
    exit(-1);
}
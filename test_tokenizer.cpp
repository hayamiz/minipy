
#include "tokenizer.hpp"
#include <iostream>

int main(int argc, char * argv[]){
    Tokenizer tok(argv[1]);
    
    while(tok.get_next() != TOK_EOF){
        tok.print_cur_token();
    }

    return 0;
}

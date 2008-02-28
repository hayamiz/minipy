
#include "test_parser.hpp"

// Syntax Treeの使い方:

int main(int argc, char * argv[]){
    // dummy source position
//     SrcPos pos("file name", 0 /* line number */, 0 /* column number */);
    
//     // 1 + 2
//     Expr one_plus_two
//         = Expr::make_infix_operator( // make expression like 'Expr + Expr'
//             TOK_PLUS // specify operator by token_kind_t
//             , Expr::make_literal_int(1, pos) // make literal integer
//             , Expr::make_literal_int(2, pos) // make literal integer
//             , pos);

//     // "hoge"
//     Expr str_hoge
//         = Expr::make_literal_string( // make expression of string literal
//             "ho\"ge"
//             , pos);

//     // tuple: (1 + 2, "hoge")
//     vector<Expr> ls;
//     ls.push_back(one_plus_two); ls.push_back(str_hoge);
//     Expr tuple
//         = Expr::make_display_tuple( // make tuple
//             ls
//             , pos);

//     Stm assign =
//         Stm::make_assignment(
//             Expr::make_var("variable_name", pos)
//             , tuple
//             , pos);

//     vector<StmIfBranch> ifbrs;
//     vector<Stm> branch1;
//     branch1.push_back(assign);
//     ifbrs.push_back(
//         StmIfBranch(
//             Expr::make_infix_operator(
//                 TOK_EQ_EQ
//                 , Expr::make_var("foo", pos)
//                 , Expr::make_none(pos)
//                 , pos)
//             , branch1));

//     Stm ifstm = Stm::make_if(ifbrs, pos);
//     Printer::print_stm(ifstm, 0);
//     cout << endl;

    if (argc < 2){
        cerr << "One more arguments required." << endl;
        exit(-1);
    }

    Parser parser(argv[1]);

    vector<Stm> input_stms =  parser.file_input();

    for(vector<Stm>::iterator itr = input_stms.begin();
        itr != input_stms.end();
        itr++){
        Printer::print_stm(*itr, 0);
    }

    return 0;
}

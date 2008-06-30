#include "minipy.hpp"

int main(int argc, char *argv[]){
    if (argc < 2){
        perror("input file required.");
        exit(1);
    }

    Parser parser(argv[1]);
    Eval eval;

    Env genv;
    ConsStack<Stack_trace_entry*> * bt = NULL;
    vector<Stm*> file = parser.file_input();

    bootstrap(eval, genv);

    eval.file_input(file, genv, bt);

    return 0;
}

void bootstrap(Eval & eval, Env & genv){
    Parser p("builtinLib.py");
    ConsStack<Stack_trace_entry*> * bt;
    vector<Stm*> builtin = p.file_input();

    eval.file_input(builtin, genv, bt);

#include "bootstrap.inc"
}

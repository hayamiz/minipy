#include "vmpy.hpp"

int main(int argc, char * argv[]){

    if (argc < 2){
        perror("input file required.");
        exit(1);
    }

    vector<string> args;
    for (int i = 1; i < argc; i++){
        args.push_back(argv[i]);
    }

    Tivi tivi;

    tivi.load_file(argv[1]);
    if (argc >= 3){
        tivi.disasm();
        exit(0);
    }
    tivi.run(0);

    return 0;
}


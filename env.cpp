#include "env.hpp"

Env::Env(){
    this->env.clear();
}

void Env::set_sym(symbol_t s, Py_val* v){
    this->env[s] = v;
}

Py_val* Env::lookup_sym(symbol_t s){
    if(this->env.find(s) != this->env.end()){
        return this->env[s];
    }else{
        return py_val_not_found;
    }
}

void Env::set_global(symbol_t s){
    this->set_sym(s,py_val_global);
}

bool Env::is_global(symbol_t s){
    return lookup_sym(s) == py_val_global;
}

void Env::debug_print_all(){
    map<symbol_t, Py_val*>::iterator itr;
    cout << "--- env symbols ---" << endl;
    for(itr = this->env.begin();itr != this->env.end();itr++){
        cout << *(*itr).first << ", "; 
    }
    cout << endl;
}

uint Env::size(){
    return this->env.size();
}

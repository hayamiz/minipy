#include "debug.hpp"

map<symbol_t, vector<symbol_t> *> SrcFiles::files;

void SrcFiles::addfile(const string & file){
    SrcFiles::files[Symbol::get(file)] = new vector<string*>();
}

void SrcFiles::addline(const string & file, const string & line){
    SrcFiles::files[Symbol::get(file)]->push_back(Symbol::get(line));
}

string* SrcFiles::getline(const string & file, int lineno){
    if(SrcFiles::files.find(Symbol::get(file)) == SrcFiles::files.end()){
        return NULL;
    }
    return SrcFiles::files[Symbol::get(file)]->at(lineno);
}

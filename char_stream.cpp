#include "char_stream.hpp"

CharStream::CharStream(string filename){
    SrcFiles::addfile(filename);
    this->filestream.open(filename.c_str(), fstream::in);
    this->cur_char = '\0';
    this->cur_line = 0;
    this->cur_column = 0;
    this->cur_line_string = "";
    this->filename = filename;
}

CharStream::~CharStream(){
    this->filestream.close();
}
// 読み込み中のファイル名をかえす
string CharStream::get_filename(){
    return this->filename; 
}

// 読んだ文字一文字を返す
char CharStream::get_cur_char(){
    return this->cur_char; 
}

// 現在の行番号を返す
int CharStream::get_cur_line(){
    return this->cur_line; 
}

// 現在の列番号を返す
int CharStream::get_cur_column(){
    return this->cur_column;
}

// 現在の行の先頭から現在の文字までを返す
string CharStream::get_cur_line_string(){
    return this->cur_line_string;
}

// 次の文字を読み込んでくる
char CharStream::get_next_char(){
    this->cur_char = this->filestream.get();

    if(this->filestream.eof()){
        return EOF;
    }

    switch(this->cur_char){
    case '\n':
#ifndef RELEASE_BUILD
        SrcFiles::addline(this->filename, this->cur_line_string);
#endif
        this->cur_line++;
        this->cur_column = 0;
        this->cur_line_string = "";
        break;
   
    default:
        this->cur_line_string +=  this->cur_char;
        this->cur_column++;
    }
    return this->cur_char;
}

// 次の文字をpeekする
char CharStream::peek(){
    return (char )this->filestream.peek();
}

#ifndef CHAR_STREAM_HPP
#define CHAR_STREAM_HPP

#include "debug.hpp"
#include <iostream>
#include <istream>
#include <fstream>
#include <string>

using namespace std;

class CharStream{
private:
    fstream filestream; 
    char cur_char;      // 読み込んだ１文字
    int cur_line;       // 現在の行番号
    int cur_column;     // 現在の列番号
    string filename;    // 現在読み込み中のファイル名
    string cur_line_string;  // 現在の行の先頭から現在の文字まで

public:
    CharStream(string filename);
    ~CharStream();
    string get_filename(); // 読んだ文字一文字を返す
    char get_cur_char();   // 現在の行番号を返す
    int get_cur_line();    // 現在の列番号を返す
    string get_cur_line_string();// 現在の行の先頭から現在の文字までを返す
    int get_cur_column();// 現在の行の先頭から現在の文字までを返す
    char get_next_char();// 次の文字を読み込んでくる
    char peek();// 次の文字をpeekする
};

#endif

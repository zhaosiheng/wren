﻿
#include <iostream>
#include "nonterminal_symbols.h"
#include <fstream>
#include <sstream>
#include <string.h>

int main()
{
    ifstream ifile("program.txt");
    //将文件读入到ostringstream对象buf中
    ostringstream buf;
    char ch;
    while (buf && ifile.get(ch))
        buf.put(ch);
    //返回与流对象buf关联的字符串
    string s = buf.str();
    cout << s;
    Program tmp(s);
     
     
    return 0;
}


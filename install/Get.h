#ifndef _W05_IGYW_GET_
#define _W05_IGYW_GET_

#include <string>
#include <fstream>
#include <iostream>

#include "defs.h"
#include "Utility.h"
#include "Tokenizer.h"

using namespace std;

void Get_Next_Token(
    Tokenizing_Machine& m,
    ifstream& file,
    string& token_text,
    int& token_kind
    );

void Get_Next_Non_Whitespace_Token(
    Tokenizing_Machine& m,
    ifstream& file,
    string& token_text,
    int& token_kind
    );

void Display_Token_Kind(
    int token_kind
    );

void Display_Buffer_State(
    Tokenizing_Machine& m
    );

#endif

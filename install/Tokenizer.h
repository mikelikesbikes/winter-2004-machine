#ifndef _W05_IGYW_TOKENIZER_
#define _W05_IGYW_TOKENIZER_

#include <string>
#include <iomanip>
#include <fstream>
#include <iostream>

#include "defs.h"
#include "Utility.h"

using namespace std;

enum TokenKind {
    ERROR,
    OP,
    SYMBOL,
    REGISTER,
    LITERAL,
    NUMBER,
    STRING,
    SEPARATOR,
    WHITE_SPACE,
    COMMENT,
    NEWLINE
};

enum BufferState {
    EMPTY_BS,
    OP_OR_SYMBOL_OR_REGISTER_BS,
    NUMBER_BS,
    LITERAL_BS,
    STRING_BS,
    WHITE_SPACE_BS,
    SEPARATOR_BS,
    COMMENT_BS,
    NEWLINE_BS,
    ERROR_BS
};

// representation of tokenizing machine
struct Tokenizing_Machine {
    string buffer;
    int bs;
    bool token_ready;
    };

void Insert(
    Tokenizing_Machine& m,
    char& ch
    );

void Dispense(
    Tokenizing_Machine& m,
    string& token_text,
    int& token_kind
    );

void Flush_A_Token(
    Tokenizing_Machine& m,
    string& token_text,
    int& token_kind
    );

bool Is_Ready_To_Dispense(
    Tokenizing_Machine& m
    );

int Size(
    Tokenizing_Machine& m
    );

bool Is_Whitespace_Char(
    char ch
    );

bool Is_Digit_Char(
    char ch
    );

bool Is_Hex_Digit_Char(
    char ch
    );

bool Is_Alphabetic_Char(
    char ch
    );

bool Is_Symbol_Char(
    char ch
    );

bool Can_Start_Token(
    char ch
    );

bool Is_Branch_Op(
    string& t
    );

int Op_Or_Symbol_Or_Register(
    string& t
    );

int Buffer_Type(
    char ch
    );

int Token_Kind(
    int bs,
    string& str
    );

void Initialize_Machine (
    Tokenizing_Machine& m
    );

#endif

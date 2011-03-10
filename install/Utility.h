#ifndef _W05_IGYW_UTILITY_
#define _W05_IGYW_UTILITY_

#include <iomanip>
#include <string>
#include <fstream>
#include <iostream>

#include "defs.h"

using namespace std;

bool Remove(
    string& str,
    char& ch,
    int pos
    );

void Print_Bin(
    unsigned short x,
    ofstream& file
    );

bool Text_To_Int(
    unsigned short& ret,
    string op,
    Tables& tab,
    int line
    );

bool Convert_Literal(
    unsigned short& ret,
    string op,
    Tables& tab,
    int line
    );

bool Convert_Symbol(
    unsigned short& ret,
    string op,
    Tables& tab,
    int line
    );

bool Convert_Hex(
    unsigned short& ret,
    string op,
    int line
    );

bool Convert_Decimal(
    unsigned short &ret,
    Tables& table,
    string op,
    int line
    );

bool Convert_Register(
    unsigned short& ret,
    string op,
    int line
    );
//find symbols in a table
bool Find(
    Tables& table,
    string sym,
    unsigned short& value,
    bool& rel
    );

//finds entry symbols
bool Find(
	  Tables& tab,
	  string sym,
	  bool& defined,
	  unsigned short& value
	  );

//finds external symbols
bool Find(
	  Tables& table,
	  string sym
	  );



//finds literals in a table
bool Find(
    Tables& table,
    unsigned short value,
    unsigned short& address,
    bool& rel
    );

void Insert(
    Tables& table,
    string str,
    unsigned short value,
    bool rel
    );

void Insert(
    Tables& table,
    unsigned short value,
    bool rel
    );

void Initialize(
    Tables& table
    );

void Initialize(
    W05& machine
    );

int Convert_Hex(
	char ch[]
	);

bool Convert_Hex(
    unsigned short& ret,
    char op[],
    int line,
    char file[]
    );

bool Defined(
    Total& table,
    char value[],
    int& counter
    );

bool Get(
    char arr[],
    int num,
    ifstream &input,
    int line,
    char file[]
    );

#endif

#ifndef _W05_IGYW_PARSER_
#define _W05_IGYW_PARSER_

#include <string>
#include <iomanip>
#include <fstream>

#include "defs.h"
#include "Utility.h"
#include "Get.h"

using namespace std;

bool Parser(
    ifstream& file,
    Tables& tables
    );

void Update_Literal_Table(
    Tables& tables,
    int& LC,
    bool rel
    );

bool Check_Entry_Table(
    Tables& tables
    );

bool Define_Entry_Point(
    Tables& tables,
    string name,
    unsigned short value
    );

void Output_Literal_Table(
    Tables& tables
    );

void Print_Op_Code(
    int op
    );

#endif    

#ifndef _W05_IGYW_DISPLAY_
#define _W05_IGYW_DISPLAY_

#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>

#include "defs.h"

using namespace std;

void Display_Mem(
    W05&,
    int& startMem
    );

void Display_Reg(
    W05&
    );

void Display_Help(
    bool done
    );

void Display_Next_Inst(
    W05&
    );

#endif

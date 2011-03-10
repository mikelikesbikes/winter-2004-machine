#ifndef _W05_IGYW_INTERPRETER_
#define _W05_IGYW_INTERPRETER_

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <iomanip>

#include "defs.h"
#include "Display.h"

using namespace std;

bool ADD_I(
    unsigned short& instruction,
    W05& machine
    );

void AND_I(
    unsigned short& instruction,
    W05& machine
    );

void BR_I(
    unsigned short& instruction,
    W05& machine
    );

void DBUG_I(
    W05& machine
    );

void LD_I(
    unsigned short& instruction,
    W05& machine
    );

void LDI_I(
    unsigned short& instruction,
    W05& machine
    );

bool LDR_I(
    unsigned short& instruction,
    W05 &machine
    );

void LEA_I(
    unsigned short& instruction,
    W05& machine
    );

void JSR_I(
    unsigned short& instruction,
    W05& machine
    );

bool JSRR_I(
    unsigned short& instruction,
    W05& machine
    );
bool TRAP_I(
    unsigned short& instruction,
    W05& machine
    );

void ST_I(
    unsigned short& instruction,
    unsigned short& addressChange,
    W05& machine
    );

void STI_I(
    unsigned short& instruction,
    unsigned short& addressChange,
    W05& machine
    );

bool STR_I(
    unsigned short& instruction,
    unsigned short& addressChange,
    W05& machine
    );

void NOT_I(
    unsigned short& instruction,
    W05& machine
    );

void RET_I(
    W05& machine
    );

bool Interpreter(
    W05& machine,
    unsigned short& addressChange
    );

#endif

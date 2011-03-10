#ifndef _W05_IGYW_SIMULATOR_
#define _W05_IGYW_SIMULATOR_

#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>

#include "defs.h"
#include "Utility.h"
#include "Display.h"
#include "Loader.h"
#include "Interpreter.h"

using namespace std;

bool Prompt(
    W05& state,
    int& x,
    bool& done
    );

bool Simulator(
    ifstream& fileStream
    );

#endif

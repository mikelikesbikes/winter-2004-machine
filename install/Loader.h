#ifndef _W05_IGYW_LOADER_
#define _W05_IGYW_LOADER_

#include "defs.h"
#include "Utility.h"

#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

bool Loader(
    W05& state,
    ifstream& file,
    int& start
    );

#endif

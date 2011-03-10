#ifndef _W05_IGYW_LINKER_
#define _W05_IGYW_LINKER_

#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>

#include "defs.h"
#include"Utility.h"

using namespace std;

bool Linker(
    ifstream inn[],
    ofstream& out,
    int length
    );

#endif 

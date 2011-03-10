#ifndef _W05_IGYW_WRITER_
#define _W05_IGYW_WRITER_

#include <string>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "defs.h"
#include "Utility.h"

using namespace std;

void Writer(
    Tables& tab,
    int index,
    ofstream& object,
    ofstream& list,
    unsigned short instruction
    );
    
#endif

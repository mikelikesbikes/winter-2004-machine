#ifndef _W05_IGYW_ASSEMBLER_
#define _W05_IGYW_ASSEMBLER_

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <map>

#include "defs.h"
#include "Utility.h"
#include "Encoder.h"
#include "Writer.h"
#include "Parser.h"

using namespace std;

bool Assembler(
    ofstream& object,
    ofstream& list,
    ifstream& source
    );

#endif

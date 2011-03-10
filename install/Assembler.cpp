#include "Assembler.h"

bool Assembler(ofstream& object, ofstream& list, ifstream& source)
{
    Tables table;
    unsigned short instruction;
    bool error;
    
    Initialize(table);
    
    cout << "\nParsing...\n";
    // parse source file, create tables
    // if error, close source stream and exit
    if(!Parser(source, table))
    {
	source.close();
	cout << "\nError(s) found during parse.\n";
	return false;
    }

    cout << "Parsing complete.\n";
    cout << "\nAssembling...\n";

    int index=1;
    // for each source record - encode instruction, write to output
    while(index <= table.src_len)
    {
	error |= !(Encoder(table, index, instruction));
	Writer(table, index, object, list, instruction);
	index++;
    }// end for loop

    // output literals
    for(int k=0;k<table.lit_len; k++)
    {
	unsigned short lit = table.lit_arr[k].value;

	// output object file
	if (object.is_open())
	{
	    object << "T" << right << hex << uppercase
		   << setw(4) << setfill('0')
		   << table.lit_arr[k].address
		   << setw(4) << lit << endl;
	}
	
	// output listing file
	if (list.is_open())
	{
	    list << "(" << right << hex << uppercase
		 << setw(4) << setfill('0')
		 << table.lit_arr[k].address << ") "
		 << setw(4) << lit << "  ";
	    Print_Bin(lit, list);
	    list << " ( lit)" << endl;
	}
    }//end for loop

    // print symbol table in listing file
    if(list.is_open())
    {
	list << "\n\n       SYMBOL TABLE"
	     << "\n\nSymbol          Value    Rel/Abs" << endl << endl;
	
	int n = 0;
	while(n < table.sym_len)
	{
	    
	    list << setw(15) << setfill(' ')
		 << left << table.sym_arr[n].name << " 0x"
		 << setw(4) << setfill('0')
		 << right << table.sym_arr[n].value << "   ";
	    if(table.sym_arr[n].rel) list << "REL\n";
	    else list << "ABS\n";
	    n++;
	}
    }


    // output start of execution address
    if(object.is_open())
    {
	// create end record
	object << "E" << right << hex << uppercase << setw(4)
	       << setfill('0') << table.exec << endl;
    }

    return !error;
}

// Loader.cpp
// Author: Jason Karns
// Modified:
//   Travis Nauman, 3/3/05
//   Jason Karns, 3/8/05
//     hex function moved to Utility, changed to Convert_Hex
// Date: January 20, 2005
// Class: CSE560

#include "Loader.h"

//**********************************************//
// The loader function loads the user program   //
// stored in "file" into memory represented by  //
// state.mem_arr[], and initializes the PC      //
// to the address specified by the user. The    //
// loader returns true, if the program is loaded//
// successfully, and false otherwise.           //
//**********************************************//

bool Loader(
    W05& state,
    ifstream& file,
    int& start
    )
{
    char ch;
    char str[4];
    bool init[0x10000];
    unsigned short address, value, length;
    int temp, footprint;
    string err;

    // the init[] array is an array of booleans which stores the state
    // (initialized or not) for each cell in memory. It must be set to
    // false at the beginning, and each cell is switched
    for(int k=0;k<0xFFFF;k++){init[k] = false;}

    try
    {
//**************************************************************//
//  Header Record                                               //
//**************************************************************//

	// get first character - expecting 'H' or h
	ch = file.get();
	
	if(file.eof())// end of file
	{
	    err = "ERROR: #LH10\n\tEnd of File Encountered.\n\t"
		"Expected: Header Record Identifier 'H'";
	    throw err;
	}
	if(toupper(ch)!='H')// Non H or h character
	{
	    err = "ERROR: #LH01\n\tInvalid Character Encountered.\n\t"
		"Expected: Header Record Identifier 'H'";
	    throw err;
	}
	
	// get 6 character program name
	for(int k=0;k<6;k++)
	{
	    ch = file.get();
	    if(file.eof())// End of File
	    {
		err = "ERROR: #LH02\n\tEnd of File Encountered.\n\t"
		    "Expected: Header Record Program Name (6 characters)";
		throw err;
	    }
	    if(ch < (' ') || ('~') < ch)// accepted range of characters
	    {
		err = "ERROR: #LH04\n\tInvalid Character Encountered.\n\t"
		    "Expected: Header Record Program Name (ASCII characters 32-127)";
		throw err;
	    }
	}
	
	// get starting footprint address as 4 characters and convert to int
	for(int k=0;k<4 && !file.eof();k++)
	{
	    ch = file.get();
	    str[k] = ch;
	}
	if(file.eof())// End of File
	{
	    err = "ERROR: #LH08\n\tEnd of File Encountered.\n\t"
		"Expected: Header Record Footprint Start Address (2b HEX Integer)";
	    throw err;
	}
	temp = Convert_Hex(str);
	if(temp > 0xFFFF)// Check flag for invalid characters in hex
			 // address
	{
	    err = "ERROR: #LH03\n\tInvalid Character Encountered.\n\t"
		"Expected: Header Record Footprint Start Address (2b HEX Integer)";
	    throw err;
	}
	start = (int)temp;// address is valid, store it

	// get footprint length as 4 characters and convert to int
	for(int k=0;k<4 && !file.eof();k++)
	{
	    ch = file.get();
	    str[k] = ch;
	}
	if(file.eof())// End of File
	{
	    err = "ERROR: #LH09\n\tEnd of File Encountered.\n\t"
		"Expected: Header Record Footprint Length (2b HEX Integer)";
	    throw err;
	}
	temp = Convert_Hex(str);
	if(temp > 0xFFFF)// Check flag for invalid characters in hex
			 // value
	{
	    err = "ERROR: #LH05\n\tInvalid Character Encountered.\n\t"
		"Expected: Header Record Footprint Length (2b HEX Integer)";
	    throw err;
	}
	length = temp;// address is valid, store it
	if(length == 0)
	{
	    err = "ERROR: #LH06\n\tInvalid Address Encountered.\n\t"
		"Violated Assertion: Footprint Length != 0x0000";
	    throw err;
	}
	footprint = start + length;// footprint size

	// footprint must exist entirely within memory bounds
	if(footprint > 0x10000)
	{
	    err = "ERROR: #LH07\n\tInvalid Address Encountered.\n\t"
		"Violated Assertion: Footprint Start Address + "
		"Footprint Length <= 0x10000";
	    throw err;
	}
	
	// get rest of line (comments) until either carriage return or linefeed
	do{ch = file.get();}while(ch != 10 && ch != 13 && !file.eof());
	if(file.eof())// End of File
	{
	    err = "ERROR: #LT05\n\tEnd of File Encountered.\n\t"
		"Expected: Text Record Identifier 'T'";
	    throw err;
	}
	
	// check for Win32/MS-DOS style newlines (carriage return/linefeed)
	if(ch == 13 && file.peek() == 10) ch = file.get();
	
//**************************************************************//
//  Text Record                                                 //
//**************************************************************//
	
	ch = file.get();// expecting Text Record Identifier 'T'
	if(file.eof())// End of File
	{
	    err = "ERROR: #LT05\n\tEnd of File Encountered.\n\t"
		"Expected: Text Record Identifier 'T'";
	    throw err;
	}
	if(toupper(ch)!='T')// input file must contain at least one text record
	{
	    err = "ERROR: #LT01\n\tInvalid Character Encountered.\n\t"
		"Expected: Text Record Identifier 'T'";
	    throw err;
	}

	// continue reading text records until end record is found
	while(toupper(ch)=='T')
	{
	    // get memory address to initialize as 4 hex characters
	    for(int k=0;k<4 && !file.eof();k++)
	    {
		ch = file.get();
		str[k] = ch;
	    }
	    if(file.eof())// End of File
	    {
		err = "ERROR: #LT06\n\tEnd of File Encountered.\n\t"
		    "Expected: Text Record Memory Address (2b HEX Integer)";
		throw err;
	    }
	    temp = Convert_Hex(str);
	    
	    if(temp > 0xFFFF)// Check flag for invalid characters in
			     // hex address
	    {
		err = "ERROR: #LT02\n\tInvalid Character Encountered.\n\t"
		    "Expected: Text Record Memory Address (2b HEX Integer)";
		throw err;
	    }
	    address = temp;// address is valid hex, store it
	    
	    // memory address must lie within footprint bounds
	    if(!(start <= address && address < footprint))
	    {
		err = "ERROR: #LT03\n\tInvalid Address Encountered.\n\t"
		    "Violated Assertion: Footprint Start Address <= "
		    "Text Memory Address < Footprint Start Address "
		    "+ Footprint Length";
		throw err;
	    }
	    
	    // get hex value for associated memory address as characters
	    for(int k=0;k<4 && !file.eof();k++)
	    {
		ch = file.get();
		str[k] = ch;
	    }
	    if(file.eof())// End of File
	    {
		err = "ERROR: #LT07\n\tEnd of File Encountered.\n\t"
		    "Expected: Text Record Memory Value (2b HEX Integer)";
		throw err;
	    }
	    temp = Convert_Hex(str);
	    if(temp > 0xFFFF)// Check flag for invalid characters in
			     // hex value
	    {
		err = "ERROR: #LT04\n\tInvalid Character Encountered.\n\t"
		    "Expected: Text Record Memory Value (2b HEX Integer)";
		throw err;
	    }
	    value = temp;// hex value is valid, store it

	    // check if memory cell has already had a value set for it
	    if(init[address] == true)// if so, display warning
	    {
		cerr << "WARNING: #LT08\n\tText Record Memory Address "
		     << hex << uppercase << showbase
		     << address << " already initialized.\n\tValue "
		     << state.mem_arr[address] << " overwritten with "
		     << value << ".\n";
	    }
	    
	    // either way, write/overwrite with new value
	    state.mem_arr[address] = value;

	    // set this cell as initialized
	    init[address] = true;
	    
	    // get rest of line (comments) until either carriage return or linefeed
	    do{ch = file.get();}while(ch != 10 && ch != 13 && !file.eof());

	    if(file.eof())// End of File
	    {
		err = "ERROR: #LE04\n\tEnd of File Encountered.\n\t"
		    "Expected: Text Record Identifier 'T' or "
		    "End Record Identifier 'E'";
		throw err;
	    }
	    
	    // check for Win32/MS-DOS style newlines (carriage return/linefeed)
	    if(ch == 13 && file.peek() == 10) ch = file.get();

	    // get next character, either E or T
	    ch = file.get();
	    if(file.eof())// End of File
	    {
		err = "ERROR: #LE04\n\tEnd of File Encountered.\n\t"
		    "Expected: Text Record Identifier 'T' or "
		    "End Record Identifier 'E'";
		throw err;
	    }
	    // continue while loop for text records while leading char is T
	}// end while
	
//**************************************************************//
//  End Record                                                  //
//**************************************************************//
	
	if(toupper(ch)!='E')// expecting Text/End Record Identifiter T
			    // or E
	{
	    err = "ERROR: #LE01\n\tInvalid Character Encountered.\n\t"
		"Expected: Text Record Identifier 'T' or "
		"End Record Identifier 'E'";
	    throw err;
	}
	
	// get PC address as 4 hex characters
	for(int k=0;k<4 && !file.eof();k++)
	{
	    ch = file.get();
	    str[k] = ch;
	}
	if(file.eof())// End of File
	{
	    err = "ERROR: #LE05\n\tEnd of File Encountered.\n\t"
		"Expected: End Record Program Execution Start Address (2b HEX Integer)";
	    throw err;
	}
	temp = Convert_Hex(str);
	if(temp > 0xFFFF)// Check flag for invalid characters in hex address
	{
	    err = "ERROR: #LE02\n\tInvalid Character Encountered.\n\t"
		"Expected: End Record Program Execution Start Address (2b HEX Integer)";
	    throw err;
	}

	state.pc = temp;// valid hex address, store it

	// PC value must lie within footprint bounds
	if(!(start <= state.pc && state.pc < footprint))
	{
	    err = "ERROR: #LE03\n\tInvalid Address Encountered.\n\t"
		"Violated Assertion: Footprint Start Address <= "
		"Program Execution Start Address < Footprint Start Address + "
		"Length";
	    throw err;
	}
	
	// display warning if the memory cell referenced by the PC has not
	// been initialized (will default to NOP, if uninitialized)
	if(init[state.pc]==false)
	{
	    cerr << "WARNING: #LE06\n\tProgram Execution Start Address "
		 << "not initialized. Instruction defaults to NOP\n";
	}
	return true;
    }// end try
    catch(string e)
    {
	cerr << e << endl;
	return false;
    }
}

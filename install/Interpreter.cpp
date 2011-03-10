// Interpreter.cpp
// Authors: Sudhir Patel, Jen Nappier
// Date: January 20, 2005
// Class: CSE560

#include "Interpreter.h"

// This function adds two values and places the result in the
// destination register dr; one of the values to be added comes from a
// source register, while the other comes from another source register
// or as a 5-bit immediate value to be sign extended.

bool ADD_I(unsigned short& instruction, W05& machine)
{
    unsigned short bit5, bit5True, sr1, sr2, dr, imm5, temp1, temp2;
    bool error=true;
    //determine the value of bit 5 from the instruction which
    //determines whether or not an immediate value will be used
    
    bit5True = 0x0020;
    bit5 = bit5True & instruction;
    
    // obtain the number of the source register
    sr1 = instruction & 0x01C0;
    sr1 = sr1 >> 6;
    
    // obtain the number of the destination register
    dr = instruction & 0x0E00;
    dr = dr >> 9;
    
    // if bit 5 equals one, use an immediate value
    if (bit5 == bit5True)
    {
	// obtain the immediate value
	imm5 = 0x001F & instruction;
	
	// determine whether imm5 requires extension with 1's if imm5
	// (interpreted as a 16 bit value) is greater than 15, then
	// bit  4 of imm5 must be a 1- in this case, sign extend with
	// ones; otherwise, leave it with leading zeros
	if (imm5 > 15)
	{
	    // add leading ones
	    imm5 = imm5 | 0xFFE0;
	}

	//store the initial value of the source register
	temp1 = machine.reg_arr[sr1];
	
	// compute sum
	machine.reg_arr[dr] = temp1 + imm5;
	
	// check for arithmetic overflow and print warning if so if
	// two nonnegatives added up result in a negative, there is
	// overflow if machine.reg_arr[sr1] or imm5, when interpreted as
	// unsigned short, aregreater than 32767, the leading bit must
	// be a 1; this represents a negative number in signed 2's
	// complement form
	try
	  {
	    if ((temp1 < 32768) && (imm5 < 32768) && (machine.reg_arr[dr] >= 32768))
	      {
		string err="WARNING #I01:\n\nArithmetic overflow has occurred in ADD instruction. Value in Register ";
		throw err;
	      }
	    // if two negatives added up result in a nonnegative, there is underflow
	    else if ((temp1 >= 32768) && (imm5 >= 32768) && (machine.reg_arr[dr] < 32768))
	      {
		string err="WARNING #I02:\n\nArithmetic underflow has occurred in ADD instruction. Value in Register ";
		throw err;
	      }
	  }
	catch(string e)
	  {
	    cerr<<e<<dr<<" may be incorrect."<<endl;
   
	  }
    }
    
    // else use two source registers
    else
    {
	// obtain number of second source register
	sr2 = instruction & 0x0007;

	// save the initial values of the two source registers
	temp1 = machine.reg_arr[sr1];
	temp2 = machine.reg_arr[sr2];

        // compute sum
	machine.reg_arr[dr] = temp1 + temp2;
	
	// check for arithmetic overflow and print warning if so if
	// two nonnegatives added up result in a negative, there is
	// overflow if machine.reg_arr[sr1] or machine.reg_arr[sr2], when interpreted
	// as unsigned short, are greater than 32767, the leading bit
	// must be a 1; this represents a negative number in signed
	// 2's complement form
	try
	  {
	    
	    if ((temp1 < 32768) && (temp2 < 32768) &&
	    (machine.reg_arr[dr] >= 32768))
	      {
		string err="WARNING #I03:\n\nArithmetic overflow has occurred in ADD instruction. Value in Register ";
		throw err;
	      }
	    
	    // if two negatives added up result in a nonnegative, there is underflow
	    else if ((temp1 >= 32768) && (temp2 >= 32768) &&
		     (machine.reg_arr[dr] < 32768))
	      {
		string err="WARNING #I04:\n\nArithmetic underflow has occurred in ADD instruction. Value in Register ";
		throw err;
	      }
	  }
	catch (string e)
	  {
	    cerr<<e<<dr<<" may be incorrect."<<endl;

	  }
	  }
    
    // set appropriate machine.ccr register
    // if machine.reg_arr[dr], when interpreted as an unsigned short, is
    // greater than 32767, the leading bit must be a 1; this
    // represents a negative number in signed 2's complement form
    if (machine.reg_arr[dr] > 32767)
    {
	// set negative bit
	machine.ccr = 4;
    }
    else if (machine.reg_arr[dr] == 0)
    {
	// set zero bit
	machine.ccr = 2;
    }
    // else machine.reg_arr[dr] is positive
    else
    {
	// set positive bit
	machine.ccr = 1;
    }
    return error;
}

// This function performs a bitwise AND on two values and places the
// result in the destination register dr; one of the values to be
// ANDed comes from a source register, while the other comes from
// another source register or as a 5-bit immediate value to be sign
// extended.
void AND_I(unsigned short& instruction, W05& machine)
{
    unsigned short bit5, bit5True, sr1, sr2, dr, imm5;
    // determine the value of bit 5 from the instruction which
    // determines whether or not an immediate value will be used
    bit5True = 0x0020;
    bit5 = bit5True & instruction;
    
    // obtain the number of the source register
    sr1 = instruction & 0x01C0;
    sr1 = sr1 >> 6;
    
    // obtain the number of the destination register
    dr = instruction & 0x0E00;
    dr = dr >> 9;
    
    // if bit 5 equals one, use an immediate value
    if (bit5 == bit5True)
    {
	// obtain the immediate value
	imm5 = 0x001F & instruction;
	
	// determine whether imm5 requires extension with 1's
	// if imm5 (interpreted as a 16 bit value) is greater than 15,
	// then bit 4 of imm5 must be a 1
	// in this case, sign extend with ones; otherwise, leave it with leading zeros
	if (imm5 > 15)
	{
	    // add leading ones
	    imm5 = imm5 | 0xFFE0;
	}
	
	// perform AND
	machine.reg_arr[dr] = machine.reg_arr[sr1] & imm5;
    }
    
    // else use two source registers
    else
    {
	// obtain number of second source register
	sr2 = instruction & 0x0007;
	
	// perform AND
	machine.reg_arr[dr] = machine.reg_arr[sr1] & machine.reg_arr[sr2];
    }
    
    // set appropriate machine.ccr register
    // if machine.reg_arr[dr], when interpreted as an unsigned short, is
    // greater than 32767, the leading bit must be a 1; this
    // represents a negative number in signed 2's complement form
    if (machine.reg_arr[dr] > 32767)
    {
	// set negative bit
	machine.ccr = 4;
    }
    else if (machine.reg_arr[dr] == 0)
    {
	// set zero bit
	machine.ccr = 2;
    }
    // else machine.reg_arr[dr] is positive
    else
    {
	// set positive bit
	machine.ccr = 1;
    }
}


// This function is a conditional branch that is taken if any of the
// specified machine.ccrs contain a 1.
void BR_I(unsigned short& instruction, W05& machine)
{
    unsigned short n, z, p, fifteenNine, pgoffset9;
    
    n = instruction & 0x0800;
    z = instruction & 0x0400;
    p = instruction & 0x0200;
    
    // determine whether to branch;  if the n bit in the instruction
    // is set and the machine.ccr is negative or the z bit is set and the machine.ccr
    // is zero or the p bit is set and the machine.ccr is positive, then
    // branch
    if (((n == 0x0800) && (machine.ccr == 4)) || ((z == 0x0400) && (machine.ccr == 2)) || 
	((p == 0x0200) && (machine.ccr == 1)))
    {
	// obtain the 7 leading bits of the machine.pc
	fifteenNine = 0xFE00 & machine.pc;
	
	// obtain the page offset from the instruction
	pgoffset9 = 0x01FF & instruction;
	
	// obtain new value for machine.pc
	machine.pc = pgoffset9 + fifteenNine;
    }
}
// This function displays the the contents of the machine.pc, machine.ccr, and general
// purpose register.
void DBUG_I(W05& machine)
{
    // display the 8 general purpose registers, machine.pc, and machine.ccr
    Display_Reg(machine);
}

// This funtion allows jumps to subroutines.  The destination address
// is formed by concatenating bits 15:9 of the machine.pc with bits 8:0 of the
// instruction.  It may also store the value of the current machine.pc.
void JSR_I(unsigned short& instruction, W05& machine)
{
    unsigned short page, pgoffset9, eL;
    
    // isolate the link bit (bit 11) of the instruction
    eL = 0x0800 & instruction;
    
    // if link bit is set, save current machine.pc to register 7
    if (eL == 0x0800)
    {
	machine.reg_arr[7] = machine.pc;
    }
    
    // obtain the 7 leading bits of the machine.pc (the page)
    page = 0xFE00 & machine.pc;
    
    // obtain the page offset from the instruction
    pgoffset9 = 0x01FF & instruction;
    
    // obtain new value for the machine.pc
    machine.pc = page + pgoffset9;
}

// This function allows jumps to subroutines.  The destination address
// is formed by adding the value in a base register to the index
// specified in the instruction.
bool JSRR_I(unsigned short& instruction, W05 & machine)
{
    unsigned short index6, baseR, address, eL;
    bool error=true;
    // obtain the base register from the instruction
    baseR = instruction >> 6;
    
    // baseR = baseR & (13 zeros # 3 ones)
    baseR = baseR & (7);
    
    // obtain the index from the instruction index6 = instruction &
    // (10 ones # 6 ones)
    index6 = instruction & (63);
    
    // calculate effective address
    address = machine.reg_arr[baseR] + index6;
    
    // check for address overflow by checking that the base address
    // was a high number and that adding the index to it resulted in
    // an overflow (a low number)
    try
      {
	if ((machine.reg_arr[baseR] > 65472) && (address < 63))
	  {
	    // print warning
	    string err="WARNING #I05:\n\nMaximum address has been exceeded.\nBaseR + index calculation will wrap back to address 0x0000.\nPC will jump to address 0x";
	    throw err;
	  }
      }
    catch(string e)
      {
	cerr<<e<<hex<<address<<"."<<endl;
      }
    // isolate the link bit (bit 11) of the instruction
    eL = 0x0800 & instruction;
    
    // if link bit is set, save current machine.pc to register 7
    if (eL == 0x0800)
    {
	machine.reg_arr[7]=machine.pc;
    }

    //obtain the new value for the machine.pc
    machine.pc = address;
    return error;

}

// This procedure loads a value from memory to a general purpose
// register using the direct addressing mode.
void LD_I (unsigned short& instruction, W05& machine)
{
    unsigned short pgoffset9, dr, page, address;
    
    // obtain the page offset from the instruction
    // pgoffset9 = (7 zeros # 9 ones) & instruction
    pgoffset9 = (511) & instruction;
    
    // obtain the page from the machine.pc
    // page = machine.pc & (7 ones # 9 zeros)
    page = machine.pc & (65024);
    // calculate effective address
    address = pgoffset9 + page;
    
    // obtain destination register from the instruction
    dr = instruction >> 9;
    // dr = dr & (13 zeros # 3 ones)
    dr = dr & (7);
    
    // load register
    machine.reg_arr[dr] = machine.mem_arr[address];
    
    // set appropriate machine.ccr register
    // if machine.reg_arr[dr], when interpreted as an unsigned short, is
    // greater than 32767, the leading bit must be a 1; this
    // represents a negative number in signed 2's complement form
    if (machine.reg_arr[dr] > 32767)
    {
	// set negative bit
	machine.ccr = 4;
    }
    else if (machine.reg_arr[dr] == 0)
    {
	// set zero bit
	machine.ccr = 2;
    }
    // else machine.reg_arr[dr] is positive
    else
    {
	// set positive bit
	machine.ccr = 1;
    }
}
// This procedure loads a value from memory to a general purpose
// register using the indirect addressing mode.  This allows the
// operand to be on a different page than the page containing the
// current instruction.
void LDI_I (unsigned short& instruction, W05& machine)
{
    unsigned short pgoffset9, dr, page, address;
    
    // obtain page offset from instruction
    // pgoffset9 = (7 zeros # 9 ones) & instruction
    pgoffset9 = (511) & instruction;
    // obtain page from machine.pc
    // page = machine.pc & (7 ones # 9 zeros)
    page = machine.pc & (65024);
    // calculate address
    address = pgoffset9 + page;
    
    // obtain destination register from the instruction
    dr = instruction >> 9;
    // dr = dr & (13 zeros # 3 ones)
    dr = dr & (7);
    
    // get effective address
    address = machine.mem_arr[address];
    // load register
    machine.reg_arr[dr] = machine.mem_arr[address];
    
    // set appropriate machine.ccr register
    // if machine.reg_arr[dr], when interpreted as an unsigned short, is
    // greater than 32767, the leading bit must be a 1; this
    // represents a negative number in signed 2's complement form
    if (machine.reg_arr[dr] > 32767)
    {
	// set negative bit
	machine.ccr = 4;
    }
    else if (machine.reg_arr[dr] == 0)
    {
	// set zero bit
	machine.ccr = 2;
    }
    // else machine.reg_arr[dr] is positive
    else
    {
	// set positive bit
	machine.ccr = 1;
    }
}

// This procedure loads a value from memory to a general purpose
// register using the register indexed addressing mode.
bool LDR_I (unsigned short& instruction, W05& machine)
{
    unsigned short dr, address, baseR, index;
    bool error=true;
    // obtain index from instruction
    // index = instruction & (10 zeros # 6 ones)
    index = instruction & (63);
    
    // obtain base register from instruction
    baseR = instruction >> 6;
    // baseR = baseR & (13 zeros # 3 ones)
    baseR = baseR & (7);
    
    // obtain destination register from instruction
    dr = instruction >> 9;
    // dr = dr & (13 zeros # 3 ones)
    dr = dr & (7);
    
    // calculate effective address
    address = machine.reg_arr[baseR] + index;
    
    // check for address overflow by checking that the base address
    // was a high number and that adding the index to it resulted in
    // an overflow (a low number)
    try
      {
	if ((machine.reg_arr[baseR] > 65472) && (address < 63))
	  {
	    // print warning
	    string err="WARNING #I06:\n\nMaximum address has been exceeded.\nBaseR + index calculation will wrap back to address 0x0000.\nValue will be loaded from address 0x";
	    throw err;
	  }
      }
    catch(string e)
      {
	cerr<<e<<hex<<address<<"."<<endl;
      }
    // load register
    machine.reg_arr[dr] = machine.mem_arr[address];
    
    // set appropriate machine.ccr register
    // if machine.reg_arr[dr], when interpreted as an unsigned short, is
    // greater than 32767, the leading bit must be a 1; this
    // represents a negative number in signed 2's complement form
    if (machine.reg_arr[dr] > 32767)
    {
	// set negative bit
	machine.ccr = 4;
    }
    else if (machine.reg_arr[dr] == 0)
    {
	// set zero bit
	machine.ccr = 2;
    }
    // else machine.reg_arr[dr] is positive
    else
    {
	// set positive bit
	machine.ccr = 1;
    }
    return error;
}

// This procedure loads a value to a general purpose register using
// the immediate addressing mode.
void LEA_I (unsigned short& instruction, W05& machine)
{
    unsigned short pgoffset9, dr, page, address;
    
    // obtain page offset from instruction
    // pgoffset9 = (7 zeros # 9 ones) & instruction
    pgoffset9 = (511) & instruction;
    // obtain page from machine.pc
    // page = machine.pc & (7 ones # 9 zeros)
    page = machine.pc & (65024);
    // calculate address
    address = pgoffset9 + page;
    
    // obtain destination register from the instruction
    dr = instruction >> 9;
    // dr = dr & (13 zeros # 3 ones)
    dr = dr & (7);
    
    // load register
    machine.reg_arr[dr] = address;
    
    // set appropriate machine.ccr register
    // if machine.reg_arr[dr], when interpreted as an unsigned short, is
    // greater than 32767, the leading bit must be a 1; this
    // represents a negative number in signed 2's complement form
    if (machine.reg_arr[dr] > 32767)
    {
	// set negative bit
	machine.ccr = 4;
    }
    else if (machine.reg_arr[dr] == 0)
    {
	// set zero bit
	machine.ccr = 2;
    }
    // else machine.reg_arr[dr] is positive
    else
    {
	// set positive bit
	machine.ccr = 1;
    }
}

// This procedure stores a value from a general purpose register to
// memory using the direct addressing mode.
void ST_I (unsigned short& instruction, unsigned short & addressChange, W05& machine)
{
    unsigned short pgoffset9, sr, page;
    
    // obtain the page offset from the instruction
    // pgoffset9 = (7 zeros # 9 ones) & instruction
    pgoffset9 = (511) & instruction;
    // obtain the page from the machine.pc
    // page = machine.pc & (7 ones # 9 zeros)
    page = machine.pc & (65024);
    // calculate effective address
    addressChange = pgoffset9 + page;
    
    // obtain source register from the instruction
    sr = instruction >> 9;
    // sr = sr & (13 zeros # 3 ones)
    sr = sr & (7);
    
    // store value to memory
    machine.mem_arr[addressChange] = machine.reg_arr[sr];
}

// This procedure stores a value from a general purpose register to
// memory using the indirect addressing mode.  This allows the memory
// address to which a value is stored to be on a different page than
// the page containing the current instruction.
void STI_I (unsigned short& instruction, unsigned short & addressChange, W05& machine)
{
    unsigned short pgoffset9, sr, page;
    
    // obtain the page offset from the instruction
    // pgoffset9 = (7 zeros # 9 ones) & instruction
    pgoffset9 = (511) & instruction;
    // obtain the page from the machine.pc
    // page = machine.pc & (7 ones # 9 zeros)
    page = machine.pc & (65024);
    // calculate effective address
    addressChange = pgoffset9 + page;
    
    // obtain source register from the instruction
    sr = instruction >> 9;
    // sr = sr & (13 zeros # 3 ones)
    sr = sr & (7);
    
    // get effective address
    addressChange = machine.mem_arr[addressChange];
    // store value to memory
    machine.mem_arr[addressChange] = machine.reg_arr[sr];
}

// This procedure stores a value from a general purpose register to
// memory using the register indexed addressing mode.
bool STR_I (unsigned short& instruction, unsigned short & addressChange, W05& machine)
{
    unsigned short sr, baseR, index;
    bool error=true;
    // obtain index from instruction
    // index = instruction & (10 zeros # 6 ones)
    index = instruction & (63);
    
    // obtain base register from instruction
    baseR = instruction >> 6;
    // baseR = baseR & (13 zeros # 3 ones)
    baseR = baseR & (7);
    
    // obtain source register from instruction
    sr = instruction >> 9;
    // sr = sr & (13 zeros # 3 ones)
    sr = sr & (7);
    
    // calculate effective address
    addressChange = machine.reg_arr[baseR] + index;
    
    // check for address overflow by checking that the base address
    // was a high number and that adding the index to it resulted in
    // an overflow (a low number)
    try
    {
	if ((machine.reg_arr[baseR] > 65472) && (addressChange < 63))
	{
	    // print warning
	    string err = "WARNING #I07:\n\nMaximum address has been exceeded. \nBaseR + index calculation will wrap back to address 0x0000.\nValue will be stored to address 0x";
	    throw err;
	}
    }
    catch(string e)
    {
	cerr<<e<<hex<<addressChange<<"."<<endl;
    }
    
    // store value to memory
    machine.mem_arr[addressChange] = machine.reg_arr[sr];
    return error;
    // return the address in memory that was changed
}

// This procedure takes a single argument taken from a general purpose
// register, computes the bitwise complement, and places the result in
// a general purpose register.
void NOT_I (unsigned short& instruction, W05& machine)
{
    unsigned short dr, sr;
    
    // obtain source register from instruction
    sr = instruction >> 6;
    // sr = sr & (13 zeros # 3 ones)
    sr = sr & (7);
    
    // obtain destination register from instruction
    dr = instruction >> 9;
    // dr = dr & (13 zeros # 3 ones)
    dr = dr & (7);
    
    // load register dr with bitwise complement of register sr
    machine.reg_arr[dr] = ~(machine.reg_arr[sr]);
    
    // set appropriate machine.ccr register
    // if machine.reg_arr[dr], when interpreted as an unsigned short, is
    // greater than 32767, the leading bit must be a 1; this
    // represents a negative number in signed 2's complement form
    if (machine.reg_arr[dr] > 32767)
    {
	// set negative bit
	machine.ccr = 4;
    }
    else if (machine.reg_arr[dr] == 0)
    {
	// set zero bit
	machine.ccr = 2;
    }
    // else machine.reg_arr[dr] is positive
    else
    {
	// set positive bit
	machine.ccr = 1;
    }
}

// This procedure copies the contents of the general purpose register
// R7 to the machine.pc.  The procedure can be used to return from subroutine
// calls (assuming R7 has not been modified by the subroutine).
void RET_I (W05& machine)
{
    // set machine.pc to the value in register R7
    machine.pc = machine.reg_arr[7];
}


// This function simulates a system call.  It may write a character to
// the screen, write a string to the screen, read in a character, halt
// program execution, write a decimal integer to the screen, read in a
// decimal number, or generate and store a random number in register
// 0.  The behavior is specified by the instruction.
bool TRAP_I(unsigned short& instruction, W05& machine)
{
    unsigned short trapvect8, ch, address;
    int counter = 0, tempInt;
    bool overflow = 0, error=true;
    char tempCh;
    
    // determine the trap number
    // trapvect8 = instruction & (8 zeros # 8 ones)
    trapvect8 = instruction & (255);
     // select appropriate trap behavior
     switch (trapvect8)
       {
       case 0x21: // output character from r0
	 //obtain last 7 bits of register 0; this is the character to
	 //be printed to the screen
	 ch = machine.reg_arr[0] & (127);
	 // print the ASCII character
	 cout << (char)ch;
	 break;
       case 0x22: // output a null-terminated string
	 // get address of string from r0
	 address = machine.reg_arr[0];
	 // get the value from the address
	 ch = machine.mem_arr[address];
	 // obtain the last 7 bits of the value; this represents an
	 // ASCII character
	 ch = ch & (127);
	 
	 // loop executes until a null character is encountered or
	 // until each cell in memory has been printed
	 while ((ch != 0) && (counter < 65536))
	   {
	     // print a character
	     cout << (char)ch;
	     
	     // get character from the next address
	     address++;
	     // check for address overflow
	     if (address == 0)
	       { 
		 // set overflow checker
		 overflow = 1;
	       }
	     ch = machine.mem_arr[address];
	     ch = ch & (127);
	     
	     // increment counter of characters printed
	     counter++;
	   }
	 // check if address overflow occurred during print string
	 // sequence
	 try{
	   if (overflow == 1)
	     {
	       // print warning
	       string err= "WARNING #I08:\n\nMaximum address reached during print sequence. \nNext character to be printed was obtained from address 0x0000.";
	       throw err;
	     }
	 }
	 catch(string e)
	   {
	     cerr<<e<<endl;
	   }
	 // check to see if print was stopped because of max character
	 // restriction
	 try
	   {
	     if (counter == 65536)
	       {
		 // report reason for halting print
		 string err="WARNING #I09:\n\nPrint sequence terminated because a character\nfrom every memory cell has been printed.";
		 throw err;
	       }
	   }
	 catch(string e)
	   {
	     cerr<<e<<endl;
	   }
	 break;
       case 0x23: // read in a character
	 //print a prompt for a character
	 cout << "\nPlease enter a character to be stored in register 0: ";
	 
	 // read in the character
	 cin >> tempCh;
	 // in case more than one character is entered, clear up to 1024
	 // extra characters from cin
	 cin.ignore(1024, '\n');
	 
	 // store the value to r0
	 machine.reg_arr[0] = (unsigned short)tempCh;
	 
	 // reprint character to screen 
	 cout << tempCh << "\n\n"; 
	 
	 // set machine.ccr 
	 if (machine.reg_arr[0] == 0) 
	   { 
	     // set zero bit of machine.ccr 
	     machine.ccr = 2; 
	   } 
	 else // register zero contains a positive value 
	   { 
	     // set positive bit of machine.ccr 
	     machine.ccr = 1; 
	   } 
	break; 
       case 0x25: // halt program execution 
	 //print exit program message
	   try
	   {
	       string err = "\n\nProgram exited normally.";
	       throw err;
	   }
	   catch (string e)
	   {
	       cerr<<e<<endl;
	       error= false;
	   }
	   break; 
       case 0x31: // print a decimal integer to screen 
	// output the value in register 0 as a signed decimal number
	   cout << dec << (signed short)(machine.reg_arr[0]) << hex << endl; 
	 break; 
       case 0x33: // read in a decimal number to register 0 
	 // print prompt for a decimal number 
	   cout << "\nEnter a decimal number between -32768 and 32767 "
		<< "(must begin with +/-): ";
	
	// read value into register 0 in signed 2's complement form 
	cin >> tempInt; 
	machine.reg_arr[0] = (signed short)tempInt; 
	// echo number to the screen 
	cout << dec << (signed short)(machine.reg_arr[0]) << "\n\n" << hex; 
	
	// set appropriate machine.ccr register 
	// if machine.reg_arr[0], when interpreted as an unsigned short, is
	// greater than 32767, the leading bit must be a 1; this
	// represents a negative number in signed 2's complement form
	if (machine.reg_arr[0] > 32767) 
	  { 
	    // set negative bit 
	    machine.ccr = 4; 
	  } 
	else if (machine.reg_arr[0] == 0) 
	  { 
	    // set zero bit 
	    machine.ccr = 2; 
	  } 
	// else machine.reg_arr[0] is positive 
	else 
	  { 
	    // set positive bit 
	    machine.ccr = 1;
	  }
	break;
       case 0x43: // store a random number in r0 
	 
	 // store a random number into register 0 
	 machine.reg_arr[0] = (signed short)(rand()); 
	
	 // set appropriate machine.ccr register 
	 // if machine.reg_arr[0], when interpreted as an unsigned short, is
	 // greater than 32767, the leading bit must be a 1; this
	 // represents a negative number in signed 2's complement form
	 if (machine.reg_arr[0] > 32767) 
	   { 
	     // set negative bit 
	     machine.ccr = 4; 
	   } 
	 else if (machine.reg_arr[0] == 0) 
	   { 
	     // set zero bit 
	     machine.ccr = 2; 
	   } 
	 // else machine.reg_arr[0] is positive 
	 else 
	   { 
	     // set positive bit 
	    machine.ccr = 1; 
	   } 
	 break; 
       default: //erroneous trap number 
	//print error message 
	 try{
	   string err="ERROR #I10:\n\nInvalid TRAP number.  Program execution halted."; 
	   throw err;
	 }
	 catch(string e)
	   {
	     cerr<<e<<endl;
	     error= false;
	   }
       } 

 // set r7 to machine.pc 
 machine.reg_arr[7] = machine.pc; 
 return error;
} 


// This function fetches and executes an instruction and returns the
// address of a memory location that was changed.
bool Interpreter(W05 & machine, unsigned short& addressChange)
{
  unsigned short instruction, opcode;
    

    // fetch instruction	 
    instruction = machine.mem_arr[machine.pc]; 
    //extract opcode from instruction 
    opcode = (instruction >> 12); 
    
    // increment machine.pc 
    machine.pc = machine.pc + 1; 
    
    // if adding one to machine.pc made it equal to zero, it had reached the
    // maximum memory address and started back at zero
    //print warning in the case of machine.pc wrap around
    try
    {
	if (machine.pc == 0) 
	{
	    string err="WARNING #I11:\n\nPC has surpassed maximum address. PC reset to 0x0000."; 
	    throw err;
	} 
    }
    catch(string e)
    {
	cerr<<e<<endl;
    }
    // determine and call appropriate instruction 
    switch (opcode) 
    {
    case 0x0:
	BR_I(instruction, machine); 
	addressChange = machine.pc; 
	return true;
	break; 
    case 0x1: 
	addressChange = machine.pc;
	return ADD_I(instruction, machine);
	break;
    case 0x2:
	addressChange = machine.pc;
	LD_I(instruction, machine);
	return true;
	break;
    case 0x3:
	  ST_I(instruction, addressChange, machine); 
	  return true;
	  break;
    case 0x4:
	addressChange = machine.pc;
	JSR_I(instruction, machine);
	return true;
	break;
    case 0x5:
	addressChange = machine.pc;
	AND_I(instruction, machine);
	return true;
	break;
    case 0x6:
	addressChange = machine.pc;
	return LDR_I(instruction, machine);
	break;
    case 0x7:
	return STR_I(instruction, addressChange, machine);
	break;
    case 0x8:
	DBUG_I(machine);
	addressChange = machine.pc;
	return true;
	break; 
    case 0x9: 
	NOT_I(instruction, machine); 
	addressChange = machine.pc; 
	return true;
	break; 
    case 0xA: 
	LDI_I(instruction, machine);
	addressChange = machine.pc;
	return true;
	break; 
    case 0xB: 
	STI_I(instruction, addressChange, machine);
	return true;
	break;
    case 0xC: 
	addressChange = machine.pc;
	return JSRR_I(instruction, machine);
	break; 
    case 0xD: 
	RET_I(machine); 
	addressChange = machine.pc; 
	return true;
	break; 
    case 0xE: 
	LEA_I(instruction, machine);
	addressChange = machine.pc;
	return true;
	break; 
    case 0xF: 
	addressChange = machine.pc;
	return TRAP_I(instruction, machine);
	break; 
    } 
    return false;   
} 

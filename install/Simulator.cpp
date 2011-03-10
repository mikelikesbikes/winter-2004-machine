// Simulator.cpp
// Author: Travis Nauman
// Date: January 20, 2005
// Updated: February 28, 2005
// Class: CSE560

#include "Simulator.h"

bool Simulator(ifstream& fileStream)
{
    // Welcome and start option
    cout << "\tIIIIIIII  GGGGGGGYYY      YYYYW       WWWW\n"
	 << "\tI      I G       G  Y    Y  Y W       W  W\n"
	 << "\tIII  IIIG   GGG   G  Y  Y  Y  W       W  W\n"
	 << "\t  I  I  G  G   GGGGY  YY  YW  W       W  W\n"
	 << "\t  I  I  G  G        Y    Y W  W       W  W\n"
	 << "\t  I  I  G  G  GGGGG  Y  Y  W  W       W  W\n"
	 << "\t  I  I  G  G  G   G  Y  Y  W  W  WWW  W  W\n"
	 << "\t  I  I  G  G  GG  G  Y  Y  W  W W   W W  W\n"
	 << "\t  I  I  G  G   G  G  Y  Y  W   W     W   W\n"
	 << "\tI      IG   GGG   G  Y  Y   W     W     W\n"
	 << "\tI      I G       G   Y  Y    W   W W   W\n"
	 << "\tIIIIIIII  GGGGGGG    YYYY     WWW   WWW\n\n\n"
	 << "\t         WI05 Machine Emulator \n"
	 << "\t  Copyright 2005 It Gives You Wings Inc.\n\n";
    
    
    // maxInst is the user-defined maximum number of instructions that
    // the machine can execute before timeout. instCount keeps track
    // of how many instructions have been executed.  startAddr is the
    // value returned by the loader and is the start of the footprint
    W05 state;
    int maxInst=0, instCount=0, xCount=0, addr=0, stepCount;
    bool done;
    string err;
    
    // opens the file and checks to make sure it is open
    // sets ERROR to true if the file could not be opened
    try
    {
	// initialize memory array, ccrs, register array, PC
	Initialize(state);
  
	// pass filename to loader
	// sets ERROR to true if the file was somehow invalid
	if(!Loader(state, fileStream, addr))
	{
	    fileStream.close();
	    err = "ERROR: #S03:\n\tProgram Could Not Be Loaded.\n";
	    throw err;
	}
	
	// sets user-defined max instructions before a timeout
	cout << "Enter maximum number of instructions that can "
	     << "be executed\n before a timeout (must be >= "
	     << "zero and in DECIMAL form):  ";
	cin >> maxInst;
	cout << '\n';
	
	// cannot execute fewer than 0 instructions
	if (maxInst < 0)
	{
	    cout << "Cannot execute negative instructions.  Max set to zero" << endl;
	    maxInst = 0;
	}
	
	// displays the initial state of the machine after loading
	// the file
	Display_Reg(state);
	Display_Mem(state, addr);
	Display_Next_Inst(state);
	
	// displays the help menu and gives a prompt to the user
	Display_Help(done);
	if(!Prompt(state, stepCount, done)) return true;
	
	// while the maximum number of instructions has not been reached
	while (instCount <= maxInst)
	{
	    // determines the behavior of the machine based on what
	    // mode it is running in
	    switch(state.run_mode)
	    {
		// in quiet mode, the machine simply calls the
		// interpreter again
	    case quietMode:
		// call the interpreter to execute the next instruction in
		// memory and increment instCount
		if(!Interpreter(state, (unsigned short)addr))
		    {
			err = "Exiting Simulator.";
			throw err;
		    }
		instCount++;
		break;
		
		// in trace mode, the gen. purpose registers,
		// memory and the next instruction are displayed
		// before calling the interpreter again to execute
		// the next instruction
	    case traceMode:
		// call the interpreter to execute the next instruction in
		// memory and increment instCount
		if(!Interpreter(state, (unsigned short) addr))
		{
		    err = "Execution Halted.";
		    throw err;
		}
		instCount++;
		Display_Reg(state);
		Display_Mem(state, addr);
		Display_Next_Inst(state);
		break;
		
		// in step mode gen. purpose registers, memory and
		// the next instruction are displayed and then the
		// user is prompted
	    case stepMode:
		// call the interpreter to execute the next instruction in
		// memory and increment instCount
		if(!Interpreter(state, (unsigned short) addr))
		{
		    err = "Execution Halted.";
		    throw err;
		}
		instCount++;
		Display_Reg(state);
		Display_Mem(state, addr);
		Display_Next_Inst(state);
		if(!Prompt(state, stepCount, done)) return true;
		break;
		
		// in stepX mode, user-defined X-1 instructions are
		// executed and then the machine goes into step
		// mode where the user is prompted
	    case stepxMode:
		xCount++;
		if (xCount <= stepCount)
		{
		    // call the interpreter to execute the next
		    // instruction in memory and increment instCount
		    if(!Interpreter(state, (unsigned short) addr))
		    {
			err = "Execution Halted.";
			throw err;
		    }
		    instCount++;
		}
		else
		{
		    Display_Reg(state);
		    Display_Mem(state, addr);
		    Display_Next_Inst(state);
		    if(!Prompt(state, stepCount, done)) return true;
		    xCount = 0;
		}
		break;
	    }
	}// end while
	
	if (instCount > maxInst)
	{
	    err = "ERROR: #S02\n\tMaximum instruction "
		"count reached. Program terminated.";
	    throw err;
	}
	
	// program finished
	done = true;
	
	// display the help menu and
	// prompt the user one last time so they can look at
	// registers, memory, etc. as many times as they want before
	// quitting
	Display_Help(done);
	(void)Prompt(state, stepCount, done);
	
	return true;
    }
    catch (string e)
    {
	cerr << e << endl;
	if (e != "ERROR: #S03:\n\tProgram Could Not Be Loaded.\n")
	{
	    (void) Prompt(state, stepCount, done);
	}
	return false;
    }
}
// displays a prompt where the user can enter any commands from the
// command set
bool Prompt(W05& state, int& x, bool& done)
{
    int begAddr;
    bool prompt_var = true;
    char ch;
    cout << CURSOR;
    cin >> ch;
    cin.ignore(80,'\n');
    cout << '\n';
    
    while (prompt_var)
    {
	switch(ch)
	{
	    // prompts user for beginning of memory and displays the
	    // contents of the next 128 cells then gives another
	    // command prompt
	case 'M':
	case 'm':
	    cout << "Enter beginning of memory you want displayed (in hex):  ";
	    cin >> hex >> begAddr;
	    Display_Mem(state, begAddr);
	    cout << CURSOR;
	    cin >> ch;
	    cin.ignore(80,'\n');
	    break;

	    // displays the contents of the 8 multipurpose registers
	    // then gives another command prompt
	case 'G':
	case 'g':
	    Display_Reg(state);
	    cout << CURSOR;
	    cin >> ch;
	    cin.ignore(80,'\n');
	    break;
	case 'I':
	case 'i':
	    Display_Next_Inst(state);
	    cout << CURSOR;
	    cin >> ch;
	    cin.ignore(80,'\n');
	    break;
	    // tells the simulator to run in quiet mode
	case 'R':
	case 'r':
	    state.run_mode = quietMode;
	    prompt_var = false;
	    cout << "Program running...\n\n";
	    break;

	    // tells the simulator to run in trace mode
	case 'T':
	case 't':
	    state.run_mode = traceMode;
	    prompt_var = false;
	    break;

	    // tells the simulator to run in step mode
	case 'S':
	case 's':
	    state.run_mode = stepMode;
	    prompt_var = false;
	    break;

	    // tells the simulator to run in stepx mode
	case 'X':
	case 'x':
	    cout << "Number of instructions to execute (in decimal): ";
	    cin >> x;
	
	    state.run_mode = stepxMode;
	    prompt_var = false;
	    break;
	    
	    // displays the commands and then gives another command prompt
	case 'H':
	case 'h':
	    Display_Help(done); 
	    cout << CURSOR;
	    cin >> ch;
	    cin.ignore(80,'\n');
	    break;

	    // exits the simulator where by it will ask if the user
	    // wants to enter another file
	case 'Q':
	case 'q':
	    return false;
	    break;

	    // if the input by the user was not one of the 8 commands
	    // then it tells the user the input was invalid and gives
	    // another prompt
	default:
	    cout << "Invalid Input! (type 'h' for a list of commands)\n\n";
	    cout << CURSOR;
	    cin >> ch;
	    cin.ignore(80,'\n');
	    break;
	}// end switch
    }// end while
    return true;
}

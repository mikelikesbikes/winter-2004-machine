#include "Parser.h"

bool Parser(ifstream& file, Tables& tables)
{
    // keeps track of the line that is currently being read
    tables.src_len = 1;
    
    bool foundError = false, relocate = false, symblen = true, litlen = true, extFound = false, entFound = false, entAllowed = true, extAllowed = true;
    int LC = 0, token_kind, opcode, errCount = 0;
    string token_text, err, program_name;

    // declare and initialize a tokenizing machine
    Tokenizing_Machine m;
    Initialize_Machine(m);
    
    // find the first non-comment line check that it is .ORIG
    try
    {
	Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
	
	// take off all leading comments and newlines, increment the src_len when token_kind is newline
	while(token_kind == COMMENT or token_kind == NEWLINE)
	{
	    if(token_kind == NEWLINE)
	    {
		tables.src_len ++;
		
		// check that comments and newlines dont exceed the maximum number of source lines
		if(tables.src_len > SRC_ARR_SZ)
		{
		    err = "Maximum Number of lines reached "
			"before .ORIG line [ERR #1]";
		    throw err;
		}
	    }
	    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
	}
	
	// .ORIG line must have a label (program_name) and it must not be longer than 6 letters
	if(token_kind != SYMBOL)
	{
	    err = "Expecting Program Name token on .ORIG line, got '"
		+ token_text + "'. [ERR #2]";
	    throw err;
	}
	else if(token_text.length() > 6)
	{
	    err = "Program Name '" + token_text + "' has length > 6. [ERR #39]";
	    throw err;
	}
	
	// store label in src_arr, set program_name
	tables.src_arr[tables.src_len].label = token_text;
	program_name = token_text;
	
	
	Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
	
	// next token must be the OP token ".ORIG"
	if(token_text != ".ORIG")
	{
	    err = "Expecting .ORIG opcode, got '" + token_text + "'. [ERR #3]";
	    throw err;
	}
	
	// save .ORIG op in table
	opcode = tables.ops_map[token_text];
	tables.src_arr[tables.src_len].opcode = opcode;
	
      	Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
	
	// if .ORIG has an operand it must be a number
	if(token_kind == NUMBER)
	{
	    // number must be a hex number
	    if(token_text[0] == '#')
	    {
		err = "Expecting hex NUMBER token, got '"
		    + token_text + "'. [ERR #4]";
		throw err;
	    }
	    
	    // number must be in the valid range 0x0000 - 0xFFFF (set LC)
	    if(!Text_To_Int((unsigned short)LC,
			    token_text, tables, tables.src_len))
	    {
		err = token_text + " is not a valid load address. [ERR #5]";
		throw err;
	    }
	    
	    // store operand
	    tables.src_arr[tables.src_len].op1 = token_text;
	    
            // set relocatable to false
	    relocate = false;

	    
	    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
	    
	    // if operand is followed by a comment get a new token
	    if(token_kind == COMMENT)
	    {
		Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
	    }
	    
	    // token_kind must be NEWLINE
	    if(token_kind != NEWLINE)
	    {
		err = "Expecting NEWLINE token, got '"
		    + token_text + "'. [ERR #6]";
		throw err;
	    }
	}// .ORIG has operand
	
	// .ORIG has no operand and a comment
	else if(token_kind == COMMENT)
	{
	    // set LC = 0
	    LC = 0;
	    
	    // set relocatable to true
	    relocate = true;

	    //store value in entry table only if it is relocatable
	    tables.ent_arr[tables.ent_len].name = program_name;
	    tables.ent_arr[tables.ent_len].value = 0;
	    tables.ent_arr[tables.ent_len].defined = true;
	    tables.ent_len++;
	    
	    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
	}
	// .ORIG has nothing after it
	else if(token_kind == NEWLINE)
	{
	    // set LC = 0
	    LC = 0;
	    
	    // set relocatable to true
	    relocate = true;

	    //store value in entry table only if it is relocatable
	    tables.ent_arr[tables.ent_len].name = program_name;
	    tables.ent_arr[tables.ent_len].value = 0;
	    tables.ent_arr[tables.ent_len].defined = true;
	    tables.ent_len++;
	}
	
	// token following .ORIG is not valid
	else
	{
	    err = "Expecting NUMBER/COMMENT/NEWLINE token, got '"
		+ token_text + "'. [ERR #7]";
	    throw err;
	}
	//insert program name symbol into symbol table
	Insert(tables, program_name, LC, relocate);
	
	// set start address = initial LC
	tables.start = LC;
	
	// set line to relocatable if the program is relocatable
	tables.src_arr[tables.src_len].rel = relocate;	
	
	// check for end of file
	if(token_text.length() == 0)
	{
	    err = "End of file before .END. [ERR #8]";
	    throw err;
	}
	
	// token_kind == NEWLINE, increment src_len
	else
	{
	    tables.src_len ++;
	}
	
	// check that the number of lines is less that the max allowed
	if(tables.src_len > SRC_ARR_SZ)
	{
	    err = "Maximum Number of lines encountered before .END line. [ERR #9]";
	    throw err;
	}

	// get next token after newline
	Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
    }

    // catch errors in the .ORIG line or before
    catch(string str)
    {
	// output error message
	cerr << "(" << tables.src_len << ") ERROR:\n\t" << str << endl;
	
	foundError = true;
	
	// all errors are fatal end parser
	return !foundError;
    }			
    
// parse remaining lines until .END line is found or end of file
    while(true)
    {
	// try parsing a line
	try
	{
	    // set all lines relocatability to false
	    tables.src_arr[tables.src_len].rel = false;

	    // token_kind is COMMENT
	    if(token_kind == COMMENT)
	    {
		Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);

		// check for end of file
		if(token_text.length() == 0)
		{
		    err = "Expecting NEWLINE token, got '"
			+ token_text + "'. [ERR #6]"; 
		    throw err;
		}
	    }

	    // token_kind is a SYMBOL or OP
	    else if(token_kind == SYMBOL or token_kind == OP)
	    {
		bool symbol_found = false, symbol_rel;
		string symbol_name;
		unsigned int symbol_value;

		// token_kind is SYMBOl
		if(token_kind == SYMBOL)
		{
		    // save the symbol_name
		    symbol_name = token_text;
		    		    
		    unsigned short dummy;
		    bool dummyrel = false;
		    
		    // check if the symbol name is already defined
		    if(Find(tables, symbol_name, dummy, dummyrel))
		    {
			err = "Symbol '" + symbol_name
			    + "' already defined. [ERR #10]";
			throw err;
		    }		    

		    //check if symbol is an already defined entry point
		    if (Find(tables, symbol_name, dummyrel, dummy))
		    {
			if (dummyrel)
			{
			    err = "Entry point '" + symbol_name
			    + "' already defined. [ERR #41]";
			    throw err;
			}
		    }
		    //check if symbol is in external symbol table
		    if (Find(tables, symbol_name))
		    {
			err = "External symbol '" + symbol_name
			    + "' may not be defined. [ERR #42]";
			throw err;
		    }
		    
		    //To restrict the allowable length of symbols
		    //uncomment the following line and change the
		    //number appropriately.

//		    if(token_text.length() > ####)
// 		    {
// 			err = "Symbol '"+token_text+"' has length > ####.";
// 			throw err;
// 		    }

		    // set symbol_found to true
		    symbol_found = true;
		    
		    // save label in src_arr
		    tables.src_arr[tables.src_len].label = token_text;
		    
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
		    
		    // set the value of the symbol to the LC, if it is
		    // a .EQU the value will be changed later
		    symbol_value = LC;
		    symbol_rel = relocate;

		    // token_kind after a SYMBOL must be OP
		    if (token_kind != OP)
		    {
			err = "Expecting OP token, got '"
			    + token_text + "'. [ERR #11]";
			throw err;
		    }
		}

		// token_kind = OP
		// get the integer enumerated to the OP text
		opcode = tables.ops_map[token_text];
		tables.src_arr[tables.src_len].opcode = opcode;

		//.EXT and .ENT must be after .ORIG and before any
		//other instructions; if another instruction is found,
		//disallow .ENT and .EXT in the future
		if (opcode != ENT and opcode !=EXT)
		{
		    extAllowed = false;
		    entAllowed = false;
		}
		
		// based on the OP integer
		switch(opcode)
		{
		// Instructions with 3 operands
		case ADD:
		case AND:
		{
		    
                    // store and increment LC
		    tables.src_arr[tables.src_len].address = LC;
		    LC++;

    		    // get 3 operands seperated with SEPERATOR tokens
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
		    if(! (token_kind == SYMBOL or token_kind == REGISTER))
		    {
			err = "Invalid Token in operand 1, '"
			    + token_text + "'. [ERR #12]";
			throw err;
		    }
		    tables.src_arr[tables.src_len].op1 = token_text;
		    
		    Get_Next_Non_Whitespace_Token(m,file,token_text, token_kind);
		    if(token_kind != SEPARATOR)
		    {
			err = "Expecting ',' separator, got '"
			    + token_text + "'. [ERR #15]";
			throw err;
		    }
		    
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
		    if(! (token_kind == SYMBOL or token_kind == REGISTER))
		    {
			err = "Invalid Token in operand 2, '"
			    + token_text + "'. [ERR #13]";
			throw err;
		    }
		    tables.src_arr[tables.src_len].op2 = token_text;
		    
		    Get_Next_Non_Whitespace_Token(m,file,token_text, token_kind);
		    if(token_kind != SEPARATOR)
		    {
			err = "Expecting ',' separator, got '"
			    + token_text + "'. [ERR #15]";
			throw err;
		    }

		    // operand 3 can be SYMBOL/NUMBER/REGISTER
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
		    if(! (token_kind == SYMBOL or
			  token_kind == NUMBER or
			  token_kind == REGISTER))
		    {
			err = "Invalid Token in operand 3, '"
			    + token_text + "'. [ERR #14]";
			throw err;
		    }
		    tables.src_arr[tables.src_len].op3 = token_text;

		    //get lookahead token
		    Get_Next_Non_Whitespace_Token(m,file,token_text, token_kind);		    
		}
		break;
		
		case LDR:
		case STR:
		{
		    // store and increment LC
		    tables.src_arr[tables.src_len].address = LC;
		    LC++;
		    
		    // get 3 operands seperated with SEPERATOR tokens
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
		    if(! (token_kind == SYMBOL or token_kind == REGISTER))
		    {
			err = "Invalid Token in operand 1, '" + token_text + "'. [ERR #12]";
			throw err;
		    }
		    tables.src_arr[tables.src_len].op1 = token_text;
		    
		    Get_Next_Non_Whitespace_Token(m,file,token_text, token_kind);
		    if(token_kind != SEPARATOR)
		    {
			err = "Expecting ',' separator, got '"
			    + token_text + "'. [ERR #15]";
			throw err;
		    }
		    
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
		    if(! (token_kind == SYMBOL or token_kind == REGISTER))
		    {
			err = "Invalid Token in operand 2, '"
			    + token_text + "'. [ERR #13]";
			throw err;
		    }
		    tables.src_arr[tables.src_len].op2 = token_text;
		    
		    Get_Next_Non_Whitespace_Token(m,file,token_text, token_kind);
		    if(token_kind != SEPARATOR)
		    {
			err = "Expecting ',' separator, got '"
			    + token_text + "'. [ERR #15]";
			throw err;
		    }

		    // operand 3 can be SYMBOL/NUMBER
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
		    if(! (token_kind == SYMBOL or token_kind == NUMBER))
		    {
			err = "Invalid Token in operand 3, '"
			    + token_text + "'. [ERR #14]";
			throw err;
		    }
		    tables.src_arr[tables.src_len].op3 = token_text;
		    
		    //get lookahead token
		    Get_Next_Non_Whitespace_Token(m,file,token_text, token_kind);		    
		}
		break;
		
		// Instructions with 2 operands
		case NOT:
		{
		    // store and increment LC
		    tables.src_arr[tables.src_len].address = LC;
		    LC++;

		    // get 2 operands
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
		    if(! (token_kind == SYMBOL or token_kind == REGISTER))
		    {
			err = "Invalid Token in operand 1, '"
			    + token_text + "'. [ERR #12]";
			throw err;
		    }
		    tables.src_arr[tables.src_len].op1 = token_text;
		    
		    Get_Next_Non_Whitespace_Token(m,file,token_text, token_kind);
		    if(token_kind != SEPARATOR)
		    {
			err = "Expecting ',' separator, got '"
			    + token_text + "'. [ERR #15]";
			throw err;
		    }

		    // operand 2 can be SYMBOL/REGISTER
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
		    if(! (token_kind == SYMBOL or token_kind == REGISTER))
		    {
			err = "Invalid Token in operand 2, '"
			    + token_text + "'. [ERR #13]";
			throw err;
		    }
		    tables.src_arr[tables.src_len].op2 = token_text;

		    //get lookahead token
		    Get_Next_Non_Whitespace_Token(m,file,token_text, token_kind);		    
		}
		break;
		
		case LDI:
		case LEA:
		case ST:
		case STI:
		case JSRR:
		case JMPR:
		{
		    // store and increment LC
		    tables.src_arr[tables.src_len].address = LC;
		    LC++;
		    
		    // get 2 operands
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
		    if(! (token_kind == SYMBOL or token_kind == REGISTER))
		    {
			err = "Invalid Token in operand 1, '"
			    + token_text + "'. [ERR #12]";
			throw err;
		    }
		    tables.src_arr[tables.src_len].op1 = token_text;
		    
		    Get_Next_Non_Whitespace_Token(m,file,token_text, token_kind);
		    if(token_kind != SEPARATOR)
		    {
			err = "Expecting ',' separator, got '"
			    + token_text + "'. [ERR #15]";
			throw err;
		    }

		    // operand 2 can be SYMBOL/NUMBER
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
		    if(! (token_kind == SYMBOL or token_kind == NUMBER))
		    {
			err = "Invalid Token in operand 2, '"
			    + token_text + "'. [ERR #13]";
			throw err;
		    }
		    tables.src_arr[tables.src_len].op2 = token_text;

		    //get lookahead token
		    Get_Next_Non_Whitespace_Token(m,file,token_text, token_kind);		    
		}
		break;
		
		// Instructions with 1 operand
		case JSR:
		case JMP:
		case BR:
		case BRN:
		case BRZ:
		case BRP:
		case BRNZ:
		case BRZP:
		case BRNP:
		case BRNZP:
		case TRAP:
		{
		    // store and increment LC
		    tables.src_arr[tables.src_len].address = LC;
		    LC++;
		    
		    // operand must be SYMBOL/NUMBER
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
		    if(! (token_kind == SYMBOL or token_kind == NUMBER))
		    {
			err = "Invalid Token in operand 1, '"
			    + token_text + "'. [ERR #12]";
			throw err;
		    }
		    tables.src_arr[tables.src_len].op1 = token_text;

		    //get lookahead token
		    Get_Next_Non_Whitespace_Token(m,file,token_text, token_kind);		    
		}
		break;

		// Instructions with no operand
		case RET:
		case DBUG:
		{
		    // store and increment LC
		    tables.src_arr[tables.src_len].address = LC;
		    LC++;

		    //get lookahead token
		    Get_Next_Non_Whitespace_Token(m,file,token_text, token_kind);		    
		}
		break;

		// Special Case, must look in operand 3 for a literal
		case LD:
		{
		    // store and increment LC
		    tables.src_arr[tables.src_len].address = LC;
		    LC++;
		    
//		    tables.src_arr[tables.src_len].rel = relocate;
		    
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
		    if(! (token_kind == SYMBOL or token_kind == REGISTER))
		    {
			err = "Invalid Token in operand 1, '"
			    + token_text + "'. [ERR #12]";
			throw err;
		    }
		    tables.src_arr[tables.src_len].op1 = token_text;
		    
		    Get_Next_Non_Whitespace_Token(m,file,token_text, token_kind);
		    if(token_kind != SEPARATOR)
		    {
			err = "Expecting ',' separator, got '"
			    + token_text + "'. [ERR #15]";
			throw err;
		    }
		    
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
		    if(! (token_kind == SYMBOL or
			  token_kind == NUMBER or
			  token_kind == LITERAL))
		    {
			err = "Invalid Token in operand 2, '"
			    + token_text +"'. [ERR #13]";
			throw err;
		    }
		    tables.src_arr[tables.src_len].op2 = token_text;

		    // if operand 3 was a literal check if its value is already in
		    // the lit_arr, if not add it
		    if(token_kind == LITERAL)
		    {
			unsigned short literalValue;
			bool dummyrel;
			
			if (!Text_To_Int(literalValue, token_text, tables, tables.src_len))
			{
			    err = "Literal number not in valid range (0x0000 - 0xFFFF hex "
				"and #-32768 - #32767 decimal), skipping LITERAL '"
				+ token_text + "'. [ERR #31]";
			    throw err;
			}		       
			
			unsigned short dummy;

                        // check to see if the value is defined in the Literals table
			if(!Find(tables, literalValue, dummy, dummyrel))
			{
			    // check that max lit_len hasn't been reached
			    if(tables.lit_len >= LIT_ARR_SZ)
			    {
				litlen = false;
				err = "Maximum number of Literals defined, "
				    "skipping LITERAL '"
				    + token_text + "'. [ERR #16]";
				throw err;
			    }
			    
			    Insert(tables, literalValue, relocate);
			}
		    }

		    //get lookahead token
		    Get_Next_Non_Whitespace_Token(m,file,token_text, token_kind);		    
		}
		break;

		// Special case .ORIG (duplicate .ORIG line NOT ALLOWED)
		case ORIG:
		{
		    // throw error
		    err = "Encountered another .ORIG opcode after the "
			"first .ORIG. [ERR #17]";
		    throw err;
		}
		break;

		// Special case .END
		case END:
		{
		    try
		    {
			// set relocatability of line to relocate
			tables.src_arr[tables.src_len].rel = relocate;

			// .END cannot define a symbol
			if(symbol_found)
			{
			    err = "SYMBOL '" + symbol_name
				+ "' cannot be defined on .END line. [ERR #37]";
			    throw err;
			}
			
			Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
			//check to see if all entry points have been
			// defined before the .END line
			if (!Check_Entry_Table(tables))
			{
			    err = "Undefined entry points exist. [ERR #44]";
			    throw err;
			}
			
			// .END must be followed by NUMBER/SYMBOL/COMMENT/NEWLINE or end of file
			// .END operand is NUMBER
			if(token_kind == NUMBER)
			{
			    // save the operand in src_arr
			    tables.src_arr[tables.src_len].op1 = token_text;

			    // operand must be a hex number
			    if(token_text[0] == '#')
			    {
				err = "Expecting hex NUMBER token, got '"
				    + token_text + "'. [ERR #4]";
				throw err;
			    }
			    
			    // store the execution start in tables.exec if its not valid throw an error
			    if(!Text_To_Int(tables.exec, token_text, tables, tables.src_len))
			    {
				err = "NUMBER '" + token_text + "' is not a valid execution start address. [ERR #29]";
				throw err;
			    }
			}

			// .END operand is SYMBOL
			else if(token_kind == SYMBOL)
			{
			    unsigned short dummy_value;
			    bool dummy_rel;
			    
			    // store the execution start if the symbol is defined
			    if(!Find(tables, token_text, tables.exec, dummy_rel))
			    {
				if(!Find(tables, token_text, dummy_rel, tables.exec))
				{
				    
				    err = "SYMBOL '" + token_text
					+ "' not previously defined. [ERR #18]";
				    throw err;
				}
			    }
			    
			    // store the operand in src_arr
			    tables.src_arr[tables.src_len].op1 = token_text;
			}

			// .END followed by COMMENT/NEWLINE
			else if(token_kind == COMMENT or token_kind == NEWLINE)
			{
			    // the execution start is the first address of memory
			    tables.exec = tables.start;
			}

			// not end of file
			else if(token_text.length() != 0)
			{
			    // execution start is the first address of memory
			    tables.exec = tables.start;

			    // throw error
			    err = "Invalid token after .END, '"
				+ token_text + "'. [ERR #19]";
			    throw err;
			}

			// update the literal table using the LC and relocate
			// (all literals are relocatable if the program is relocatable
			Update_Literal_Table(tables, LC, relocate);

			// end address = LC - 1
			tables.end = LC - 1;

			// there must executable code
			if((LC+tables.lit_len)-tables.start == 0)
			{
			    err = "Length of the segment must not be zero. [ERR #40]";
			    throw err;
			}

			// exec must be inside footprint
			if(!((tables.exec >= tables.start) and (tables.exec <= tables.end)))
			{
			    err = "Execution start must be within footprint. [ERR #30]";
			    throw err;
			}

			// return whether any errors have been found
			return !foundError;
		    }

		    // catch errors (all errors are fatal ie return false)
		    catch(string str)
		    {
			// print error message
			cerr << "(" << tables.src_len << ") ERROR:\n\t" << str << endl;
			
			foundError = true;

                        // return false
			return !foundError;
		    }	
		}
		break;

		case ENT:
		{
		    //check if used in relocatable program
		    if (relocate == false)
		    {
			err = ".ENT/.EXT may only be used in relocatable programs. [ERR #45]";
			throw err;
		    }
		    //check that op not used before
		    if (entFound == true)
		    {
			
			err = ".ENT/.EXT pseudo ops may only be used once. [ERR#46]";;
			throw err;
		    }
		    //check that op used only at beginning
		    if (entAllowed == false)
		    {
			err = ".ENT/.EXT may only be used after .ORIG and before any other instructions. [ERR #47]";
			throw err;
		    }
		    //check that no label was found on line
		    if (symbol_found == true)
		    {
			err = ".ENT/.EXT pseudo ops may not have labels. [ERR #51]";
			throw err;
		    }
		    //disallow op in future
		    entFound = true;
		    
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
		    //first operand must be a symbol
		    if (token_kind != SYMBOL)
		    {
			err = "Invalid Token in operand 1, '"
			    + token_text + "'. [ERR #12]";
			throw err;
		    }
		    //check that symbol was not already declared
		    //somewhere
		    unsigned short tempnum;
		    bool tempbool;
		    if(Find(tables, token_text, tempbool, tempnum) or
		       Find(tables, token_text, tempnum, tempbool) or
		       Find(tables, token_text))
		    {
			err = "Symbol '" + token_text
			    + "' already declared. [ERR #48]";
			throw err;
		    }
		    
		    
		    //token_kind = SYMBOL, store in source array
		    tables.src_arr[tables.src_len].op1 = token_text;

		    //store in ent table
		    tables.ent_arr[tables.ent_len].name = token_text;
		    tables.ent_len++;

		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
		    //if comma, check for second operand; else, break
		    if (token_kind == SEPARATOR)
		    {
			Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
			//second operand must be a symbol
			if (token_kind != SYMBOL)
			{
			    err = "Invalid Token in operand 2, '"
				+ token_text + "'. [ERR #13]";
			    throw err;
			}
			//check that symbol was not already declared
			//somewhere
			if(Find(tables, token_text, tempbool, tempnum) or
			   Find(tables, token_text, tempnum, tempbool) or
			   Find(tables, token_text))
			{
			    err = "Symbol '" + token_text
				+ "' already declared. [ERR #48]";
			    throw err;
			}
			
			
			//token_kind = SYMBOL, store in source array
			tables.src_arr[tables.src_len].op2 = token_text;
			
			//store in ent table
			tables.ent_arr[tables.ent_len].name = token_text;
			tables.ent_len++;

			Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
			//if comma, check for third operand; else, break
			if (token_kind == SEPARATOR)
			{
			    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
			    //third operand must be a symbol
			    if (token_kind != SYMBOL)
			    {
				err = "Invalid Token in operand 3, '"
				    + token_text + "'. [ERR #14]";
				throw err;
			    }
			    //check that symbol was not already declared
			    //somewhere
			    if(Find(tables, token_text, tempbool, tempnum) or
			       Find(tables, token_text, tempnum, tempbool) or
			       Find(tables, token_text))
			    {
				err = "Symbol '" + token_text
				    + "' already declared. [ERR #48]";
				throw err;
			    }
			    
			    
			    //token_kind = SYMBOL, store in source array
			    tables.src_arr[tables.src_len].op3 = token_text;
			    
			    //store in ent table
			    tables.ent_arr[tables.ent_len].name = token_text;
			    tables.ent_len++;

			    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
			    //if comma, check for fourth operand; else, break
			    if (token_kind == SEPARATOR)
			    {
				Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
				//fourth operand must be a symbol
				if (token_kind != SYMBOL)
				{
				    err = "Invalid Token in operand 4, '"
					+ token_text + "'. [ERR #49]";
				    throw err;
				}
				//check that symbol was not already declared
				//somewhere
				if(Find(tables, token_text, tempbool, tempnum) or
				   Find(tables, token_text, tempnum, tempbool) or
				   Find(tables, token_text))
				{
				    err = "Symbol '" + token_text
					+ "' already declared. [ERR #48]";
				    throw err;
				}
				
				
				//token_kind = SYMBOL, store in source array
				tables.src_arr[tables.src_len].op4 = token_text;
				
				//store in ent table
				tables.ent_arr[tables.ent_len].name = token_text;
				tables.ent_len++;

				//get lookahead token
				Get_Next_Non_Whitespace_Token(m, file, token_text,
							      token_kind);

				//check if more operands exist
				if(token_kind == SEPARATOR)
				{
				    err = ".ENT/.EXT pseudo ops may only have 4 operands. [ERR #50]";
				    throw err;
				}
			    }
			}
	       
		    }
		}
		break;

		case EXT:
		{
		    //check if used in relocatable program
		    if (relocate == false)
		    {
			err = ".ENT/.EXT may only be used in relocatable programs. [ERR #45]";
			throw err;
		    }
		    //check that op not used before
		    if (extFound == true)
		    {
			
			err = ".ENT/.EXT pseudo ops may only be used once. [ERR#46]";;
			throw err;
		    }
		    //check that op used only at beginning
		    if (extAllowed == false)
		    {
			err = ".ENT/.EXT may only be used after .ORIG and before any other instructions. [ERR #47]";
			throw err;
		    }
		    //check that no label was found on line
		    if (symbol_found == true)
		    {
			err = ".ENT/.EXT pseudo ops may not have labels. [ERR #51]";
			throw err;
		    }
		    //disallow op in future
		    extFound = true;
		    
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
		    //first operand must be a symbol
		    if (token_kind != SYMBOL)
		    {
			err = "Invalid Token in operand 1, '"
			    + token_text + "'. [ERR #12]";
			throw err;
		    }
		    //check that symbol was not already declared
		    //somewhere
		    unsigned short tempnum;
		    bool tempbool;
		    if(Find(tables, token_text, tempbool, tempnum) or
		       Find(tables, token_text, tempnum, tempbool) or
		       Find(tables, token_text))
		    {
			err = "Symbol '" + token_text
			    + "' already declared. [ERR #48]";
			throw err;
		    }
		    
		    
		    //token_kind = SYMBOL, store in source array
		    tables.src_arr[tables.src_len].op1 = token_text;

		    //store in ext table
		    tables.ext_arr[tables.ext_len].name = token_text;
		    tables.ext_len++;

		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
		    //if comma, check for second operand; else, break
		    if (token_kind == SEPARATOR)
		    {
			Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
			//second operand must be a symbol
			if (token_kind != SYMBOL)
			{
			    err = "Invalid Token in operand 2, '"
				+ token_text + "'. [ERR #13]";
			    throw err;
			}
			//check that symbol was not already declared
			//somewhere
			if(Find(tables, token_text, tempbool, tempnum) or
			   Find(tables, token_text, tempnum, tempbool) or
			   Find(tables, token_text))
			{
			    err = "Symbol '" + token_text
				+ "' already declared. [ERR #48]";
			    throw err;
			}
			
			
			//token_kind = SYMBOL, store in source array
			tables.src_arr[tables.src_len].op2 = token_text;
			
			//store in ext table
			tables.ext_arr[tables.ext_len].name = token_text;
			tables.ext_len++;

			Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
			//if comma, check for third operand; else, break
			if (token_kind == SEPARATOR)
			{
			    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
			    //third operand must be a symbol
			    if (token_kind != SYMBOL)
			    {
				err = "Invalid Token in operand 3, '"
				    + token_text + "'. [ERR #14]";
				throw err;
			    }
			    //check that symbol was not already declared
			    //somewhere
			    if(Find(tables, token_text, tempbool, tempnum) or
			       Find(tables, token_text, tempnum, tempbool) or
			       Find(tables, token_text))
			    {
				err = "Symbol '" + token_text
				    + "' already declared. [ERR #48]";
				throw err;
			    }
			    
			    
			    //token_kind = SYMBOL, store in source array
			    tables.src_arr[tables.src_len].op3 = token_text;
			    
			    //store in ext table
			    tables.ext_arr[tables.ext_len].name = token_text;
			    tables.ext_len++;

			    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
			    //if comma, check for fourth operand; else, break
			    if (token_kind == SEPARATOR)
			    {
				Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
				//fourth operand must be a symbol
				if (token_kind != SYMBOL)
				{
				    err = "Invalid Token in operand 4, '"
					+ token_text + "'. [ERR #49]";
				    throw err;
				}
				//check that symbol was not already declared
				//somewhere
				if(Find(tables, token_text, tempbool, tempnum) or
				   Find(tables, token_text, tempnum, tempbool) or
				   Find(tables, token_text))
				{
				    err = "Symbol '" + token_text
					+ "' already declared. [ERR #48]";
				    throw err;
				}
				
				
				//token_kind = SYMBOL, store in source array
				tables.src_arr[tables.src_len].op4 = token_text;
				
				//store in ext table
				tables.ext_arr[tables.ext_len].name = token_text;
				tables.ext_len++;

				//get lookahead token
				Get_Next_Non_Whitespace_Token(m, file, token_text,
							      token_kind);

				//check if more operands exist
				if(token_kind == SEPARATOR)
				{
				    err = ".ENT/.EXT pseudo ops may only have 4 operands. [ERR #50]";
				    throw err;
				}
			    }
			}
	       
		    }
		}
		break;
		    
		// Special Case .EQU reset symbol_value
		case EQU:
		{
		    // make sure .EQU has a label
		    if(!symbol_found)
		    {
			err = ".EQU must be preceded by a symbol. [ERR #20]";
			throw err;
		    }
		    
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);

		    // save the operand of .EQU
		    tables.src_arr[tables.src_len].op1 = token_text;

		    // .EQU defined by NUMBER
		    if(token_kind == NUMBER)
		    {
			// symbol is absolute
			symbol_rel = false;

			// make sure Number is in valid range, set symbol_value
			if(!Text_To_Int((unsigned short)symbol_value,
					token_text, tables, tables.src_len))
			{
			    err = "Symbol number  '" + token_text
				+ " not in valid range (0x0000 - 0xFFFF hex and #-32768 - #65535 dec). [ERR #32]";
			    throw err;
			}
		    }

		    // .EQU defined by a SYMBOL
		    else if(token_kind == SYMBOL)
		    {
			// symbol must be defined, symbol_value/symbol_rel = symbol_value/symbol_rel of operand
			if(!Find(tables, token_text,
				 (unsigned short)symbol_value, symbol_rel))
			{
			    if (!Find(tables, token_text,
				      symbol_rel, (unsigned short)symbol_value))
			    {
				err = "SYMBOL '" + token_text
				    + "' not previously defined. [ERR #21]";
				throw err;
			    }
			    else
			    {
				if (symbol_rel == false)
				{
				    err = "SYMBOL '" + token_text
				    + "' not previously defined. [ERR #21]";
				    throw err;
				}
			    }
			}
		    }

		    // .EQU operand is not valid
		    else
		    {
			err = "Expecting NUMBER/SYMBOL token for .EQU operand, got '"
			    + token_text + "'. [ERR #22]";
			throw err;
		    }

		    //get lookahead token
		    Get_Next_Non_Whitespace_Token(m,file,token_text, token_kind);		    
		}
		break;

		// Special case .FILL
		case FILL:
		{
		    // store and increment LC
		    tables.src_arr[tables.src_len].address = LC;
		    LC++;
		    
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);

		    // check if operand is NUMBER/SYMBOL
		    if((token_kind != NUMBER and token_kind != SYMBOL))
		    {
			err = "Expecting NUMBER/SYMBOL token as "
			    "the operand of .FILL op, got '"
			    + token_text + "'. [ERR #23]";
			throw err;
		    }

		    // store operand in src_arr
		    tables.src_arr[tables.src_len].op1 = token_text;

		    //get lookahead token
		    Get_Next_Non_Whitespace_Token(m,file,token_text, token_kind);		    
		}
		break;

		// Special case .STRZ
		case STRZ:
		{
		    // store LC
		    tables.src_arr[tables.src_len].address = LC;
		    
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);

		    // Operand must be STRING
		    if(token_kind != STRING)
		    {
			err = "expecting STRING token after .STRZ op, got '"
			    + token_text + "'. [ERR #24]";
			throw err;
		    }

		    // store operand
		    tables.src_arr[tables.src_len].op1 = token_text;
		    
		    // update LC to length of string inside "'s add 1 for NULL character
		    LC += token_text.length() - 1;

		    //get lookahead token
		    Get_Next_Non_Whitespace_Token(m,file,token_text, token_kind);		    
		}
		break;

		// Special case .BLKW
		case BLKW:
		{
		    // store LC
		    tables.src_arr[tables.src_len].address = LC;

		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);

		    // store operand
		    tables.src_arr[tables.src_len].op1 = token_text;
		    
		    unsigned short tempValue;

		    // operand is a NUMBER
		    if(token_kind == NUMBER)
		    {
			//make sure number is in valid range
			if(!(Text_To_Int(tempValue, token_text, tables, tables.src_len)))
			{
			    err = "Operand of .BLKW, '" + token_text
				+ "' is not in range 0x0001 - 0xFFFF hex and #1 - #65535 decimal. [ERR #33]";
			    throw err;
			}
			// make sure number is not 0
			if(tempValue == 0)
			{
			    err = "Operand of .BLKW cannot be value 0";
			    throw err;
			}
		    }

		    // operand is a SYMBOL
		    else if(token_kind == SYMBOL)
		    {
			bool tempRel;
			if(Find(tables, token_text, tempValue, tempRel))
			{
			    // .BLKW cannot be defined by relocatable symbols
			    if(tempRel)
			    {
				err = "Operand of .BLKW, '" + token_text
				    + "' must be a previously defined, non-relocatable symbol. [ERR #25]";
				throw err;
			    }
			}

			// .BLKW operand must be a previously defined symbol
			else
			{
			    err = "Operand of .BLKW, '" + token_text
				    + "' must be a previously defined, non-relocatable symbol. [ERR #25]";
			    throw err;
			}
		    }

		    // operand is NOT valid
		    else
		    {
			err = "Expecting NUMBER/SYMBOL token following .BLKW, got '"
			    + token_text + "'. [ERR #26]";
			throw err;
		    }
		    
		    // update LC with the length of the block of words
		    LC += (int)tempValue;

		    //get lookahead token
		    Get_Next_Non_Whitespace_Token(m,file,token_text, token_kind);		    
		}
		break;
		}// end case select
		
		// after a symbol op and all legal operands have been
		// removed

		// token following a complete line must be COMMENT/NEWLINE
		// COMMENT
		if (token_kind == COMMENT)
		{
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
		}

		// must be NEWLINE
		if(token_kind != NEWLINE)
		{
		    err = "Expecting NEWLINE token, got '"
			+ token_text + "'. [ERR #6]";
		    throw err;
		}
		
		// if a symbol was found we can now define its value
		if(symbol_found)
		{
		    bool dummydefined;
		    unsigned short dummyshort;
                    // put the symbol and it's value into the symbol table
		    if(symbol_name == program_name)
		    {
			err = "Label '" + symbol_name + "' is the same as the Program Name. [ERR #34]";
			throw err;
		    }
		    if(tables.sym_len >= SYM_ARR_SZ+1)
		    {
			symblen = false;
			err = "Maximum number of symbols already defined, skipping symbol '"
			    + symbol_name + "'. [ERR #27]";
			throw err;
		    }

		    //if it is in entry point table, define only if
		    //value is relative
		    if (Find(tables, symbol_name, dummydefined, dummyshort))
		    {
			if (symbol_rel == false)
			{
			    err = "Entry point '" + symbol_name + "' must be defined by a relative value. [ERR #43]";
			    throw err;
			}
			else
			{
			    Define_Entry_Point(tables, symbol_name, symbol_value);
			    
			}
		    }
		    //insert into symbol table
		    else
		    {
			Insert(tables, symbol_name, symbol_value, symbol_rel);
		    }
		    
		}
	    }// end token_kind = SYMBOL/OP

	    // token is invalid
	    else if(token_kind != NEWLINE)
	    {
		err = "Expecting SYMBOL/OP/COMMENT/NEWLINE token, got '"
		    + token_text + "'. [ERR #28]";

		throw err;
	    }

	    // check segment length of relocatable program
	    if(relocate and (LC+tables.lit_len > 0x200))
	    {
		err = "Segment length of relocatable program exceeded one page. [ERR #35]";
		throw err;
	    }

	    // check segment length of normal program
	    if(LC+tables.lit_len > 0x10000)
	    {
		err = "Segment length exceeded program boundaries. [ERR #36]";
		throw err;
	    }
	    
	    // check special case (0x0000 - 0xFFFE or 0x0001 - 0xFFFF)
	    if((LC+tables.lit_len)-tables.start > 0xFFFF)
	    {
		err = "Segment length has exceeded program boundaries. [ERR #38]";
		throw err;
	    }

	    // increment src_len (token_kind == NEWLINE)
	    tables.src_len ++;

	    // check that the max number of lines haven't already been read in
	    if(tables.src_len > SRC_ARR_SZ)
	    {
		err = "Maximum Number of lines encountered before .END line. [ERR #9]";
		throw err;
	    }

	    // get next token
	    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
	    
	}// end try

	// catch errors
	catch(string str)
	{
	    errCount++;
	    
            // output error message
	    cerr << "(" << tables.src_len << ") ERROR:\n\t" << str << endl;

	    // set foundError to true
	    foundError = true;

	    // all conditions except else are fatal errors return false
	    if((LC+tables.lit_len)-tables.start > 0xFFFF)
	    {
		return !foundError;
	    }
	    else if(tables.src_len > SRC_ARR_SZ)
	    {
		return !foundError;
	    }
	    else if(token_text.length() == 0)
	    {
		cerr << "(" << tables.src_len << ") ERROR:\n\tEnd of file before .END. [ERR #8]" << endl;
		return !foundError;
	    }
	    else if(relocate and (LC+tables.lit_len > 0x200))
	    {
		return !foundError;
	    }
	    else if(LC+tables.lit_len > 0x10000)
	    {
		return !foundError;
	    }
	    else if (symblen == false)
	    {
		return !foundError;
	    }
	    else if (litlen == false)
	    {
		return !foundError;
	    }
	    else if (errCount >= 20)
	    {
		cerr << "Too many errors - aborting execution.\n";
		return !foundError;
	    }
	    
	    // non-fatal error
	    else
	    {
		// take tokens out until NEWLINE is encountered
		while(token_kind != NEWLINE)
		{
		    Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);

		    // if we reach end of file display error and exit false
		    if(token_text.length() == 0)
		    {
			cerr << "(" << tables.src_len << ") ERROR:\n\t"
			     << "End of file before .END. [ERR #8]" << endl;
			return !foundError;
		    }
		}
		
		// increment src_len
		tables.src_len++;
		// get next token after newline
		Get_Next_Non_Whitespace_Token(m, file, token_text, token_kind);
		
	    }
	}
    }
}

void Update_Literal_Table(
    Tables& tables,
    int& LC,
    bool rel
    )
{
    int n = 0;
    while (n < tables.lit_len)
    {
	tables.lit_arr[n].rel = rel;
	tables.lit_arr[n].address = LC;
	LC++;
	n++;
    }
}

//check if all entry points have been defined
bool Check_Entry_Table(
    Tables& tables
    )
{
    bool alldefined = true;
    int k = 0;
    while (k < tables.ent_len)
    {
	if (tables.ent_arr[k].defined == false)
	{
	    alldefined = false;
	    cerr << "Entry point '" + tables.ent_arr[k].name
				+ "' was never defined.\n";
	}
	k++;
    }
    return alldefined;
}

//Defines an entry point's value into the entry point table
bool Define_Entry_Point(
    Tables& tables,
    string name,
    unsigned short value
    )
{
    int k = 0;
    while (k < tables.ent_len)
    {
	if (tables.ent_arr[k].name == name)
	{
	    tables.ent_arr[k].value = value;
	    tables.ent_arr[k].defined = true;
	    return true;
	}
	k++;
    }
    return false;
}
    
    
void Output_Literal_Table(
    Tables& tables
    )
{
    cout << "\n\nLiteral Table\n"
	 << "----------------" << endl;

    int n=0;
    while(n < tables.lit_len)
    {
	
	cout << "Addr = " << hex << tables.lit_arr[n].address
	     << "\tValue = " << tables.lit_arr[n].value << dec << "\tRel = ";
	if(tables.lit_arr[n].rel)
	{
	    cout << "REL";
	}
	else
	{
	    cout << "ABS";
	}
	n++;
    }
}

void Print_Op_Code(
    int op
    )
{
    switch(op)
    {
    case ADD:
    {
	cout << "ADD";
    }
    break;
    case LD:
    {
	cout << "LD";
    }
    break;
    case ST:
    {
	cout << "ST";
    }
    break;
    case JSR:
    {
	cout << "JSR";
    }
    break;
    case AND:
    {
	cout << "AND";
    }
    break;
    case LDR:
    {
	cout << "LDR";
    }
    break;
    case STR:
    {
	cout << "STR";
    }
    break;
    case DBUG:
    {
	cout << "DBUG";
    }
    break;
    case NOT:
    {
	cout << "NOT";
    }
    break;
    case LDI:
    {
	cout << "LDI";
    }
    break;
    case STI:
    {
	cout << "STI";
    }
    break;
    case JSRR:
    {
	cout << "JSRR";
    }
    break;
    case RET:
    {
	cout << "RET";
    }
    break;
    case LEA:
    {
	cout << "LEA";
    }
    break;
    case TRAP:
    {
	cout << "TRAP";
    }
    break;
    case ORIG:
    {
	cout << ".ORIG";
    }
    break;
    case END:
    {
	cout << ".END";
    }
    break;
    case EQU:
    {
	cout << ".EQU";
    }
    break;
    case FILL:
    {
	cout << ".FILL";
    }
    break;
    case JMP:
    {
	cout << "JMP";
    }
    break;
    case STRZ:
    {
	cout << ".STRZ";
    }
    break;
    case BLKW:
    {
	cout << ".BLKW";
    }
    break;
    case JMPR:
    {
	cout << "JMPR";
    }
    break;
    case BR:
    {
	cout << "BR";
    }
    break;
    case BRP:
    {
	cout << "BRP";
    }
    break;
    case BRZ:
    {
	cout << "BRZ";
    }
    break;
    case BRZP:
    {
	cout << "BRZP";
    }
    break;
    case BRN:
    {
	cout << "BRN";
    }
    break;
    case BRNP:
    {
	cout << "BRNP";
    }
    break;
    case BRNZ:
    {
	cout << "BRNZ";
    }
    break;
    case BRNZP:
    {
	cout << "BRNZP";
    }
    break;
    }
    cout << "\n";
}

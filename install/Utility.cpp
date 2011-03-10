#include "Utility.h"

bool Remove(string& str, char& ch, int pos)
{
    if(str.length()-1 < pos) // pos not within string
    {
	return false;
    }
    ch = str[pos];
    str.erase(pos,pos+1);
    return true;
}

void Print_Bin(unsigned short x, ofstream& file)
{
    string num="";
    for (int k=0; k<16; k++)
    {
	num = (char)(48 + x%2) + num;
	x = x/2;
    }
    file << num;
}

bool Text_To_Int(unsigned short& ret, string op, Tables& tab, int line)
{
    
    if (op.length() > 0)
    {
	char ch;
	(void) Remove(op, ch, 0);
	switch(ch)
	{
	case 'R':
	    return Convert_Register(ret, op, line);
	case 'x':
	    return Convert_Hex(ret, op, line);
	case '#':
	    return Convert_Decimal(ret, tab, op, line);
	case '=':
	    return Convert_Literal(ret, op, tab, line);
	case '"':	  
	    return true;
	default:
	    op = ch + op;
	    return Convert_Symbol(ret, op, tab, line);
	}//end case
    }//end if
}//end function

bool Convert_Literal(unsigned short& ret, string op, Tables& tab, int line)
{
    if (op.length() > 1)
    {
	char ch;
	(void) Remove(op, ch, 0);
	switch(ch)
	{
	case 'x':
	    return Convert_Hex(ret, op, line);
	default: // must be decimal
	    int neg=1;
	    long integer, sum=0;
	    
	    try
	    {
		//set determine if it's a negative number, set
		//counters appropriately
		if(op[0]=='-')
		{
		    neg=-1;
		    char ch;
		    (void) Remove(op, ch, 0);// remove negative sign
		}//end if
		//the number is not negative
		else if(op[0]=='+')
		{
		    char ch;
		    (void) Remove(op, ch, 0);// remove plus sign
		}//end else if
		// else, no sign = positive
		for(int k=0;k<op.length();k++)
		{
		    //convert the character
		    integer=(long)op[k] - 48;
		    sum = sum*10 + integer;
		}//end for
		
		sum*=neg;
		
		if(sum < -32768 or sum > 32767)
		    throw "ERROR: #CL01\n\tInvalid Literal String: '=#" + op +
			"' Out of Range: [-32,768 : 32,767]";
		
		ret=(unsigned short)sum;
		return true;
	    }//end try
	    catch (string e)
	    {
		cerr << "(" << line << ") " << e << endl;
		return false;
	    }
	}
    }
}

bool Convert_Symbol(unsigned short& ret, string op, Tables& tab, int line)
{
    int i;// loop counter
    
    try
    {
	// lookup symbol in external table
	for(i=0;i<tab.ext_len;i++)
	{
	    //if symbol has been found
	    if(op==(tab.ext_arr[i].name))
	    {
		//set ret= symbol's value
		ret=0;
		//break out of loop
		return true;
	    }//end if
	}// end for

	// lookup symbol in entry point table
	//while still searching & haven't found the symbol
	for(i=0;i<tab.ent_len;i++)
	{
	    //if symbol has been found
	    if(op==(tab.ent_arr[i].name))
	    {
		//set ret= symbol's value
		ret=(unsigned short)tab.ent_arr[i].value;
		//break out of loop
		return true;
	    }//end if
	}//end for

	//lookup symbol in table
	//while still searching & haven't found the symbol
	for(i=0;i<tab.sym_len;i++)
	{
	    //if symbol has been found
	    if(op==(tab.sym_arr[i].name))
	    {
		//set ret= symbol's value
		ret=(unsigned short)tab.sym_arr[i].value;
		//break out of loop
		return true;
	    }//end if
	}//end for

	// not found in ext_table, ent_table, or sym_table
	throw "ERROR: #CS02\n\tSymbol '" + op + "' Undefined";
    }
    catch(string e)
    {
	cerr << "(" << line << ") " << e << endl;
	return false;
    }
}

int Convert_Hex(char ch[]) // used by Loader
{
    int value=0,k=0;
    // get hex value for one character at a time
    for(int i=0;i<4;i++)
    {
	// convert hex numbers
	if('0' <= ch[i] && ch[i] <= '9') k = (ch[i] - '0');
	// convert hex letters
	else if('A' <= toupper(ch[i]) && toupper(ch[i]) <= 'F') k = (toupper(ch[i]) - '7');
	// flag for non-hex characters: invalid char found if returned
	// value is greater than 0xFFFF
	else return 0xFFFFF;
	value = value*16 + k;
    }
    return value;
}

bool Convert_Hex(unsigned short& ret, string op, int line)
{
  //each hex# is added to the sum
  //integer is the hex value of the converted character
  long sum=0, integer;

  try
  {
      for(int k=0; k<op.length(); k++)
      {
	  // convert hex numbers
	  if('0' <= op[k] and op[k] <= '9')
	      integer = (long)(op[k] - 48);
	  // convert uppercase hex letters
	  else if('A' <= toupper(op[k]) and
		  'F' >= toupper(op[k]))
	      integer = (long)(toupper(op[k]) - 55);
	  
	  //add all the converted characters up
	  sum=sum*16 + integer;
      }//end for
      
      //hex number is greater than max allowed value
      if (sum > 0xFFFF)
      {
	  throw "ERROR: #CH01\n\tInvalid Hex String: 'x" + op +
	      "' Out of Range: [0x0000 : 0xFFFF]";
      }
      
      ret=(unsigned short)sum;
      return true;
  }
  catch (string e)
  {
      cerr << "(" << line << ") " << e << endl;
      return false;
  }
}

bool Convert_Decimal(unsigned short& ret, Tables& table, string op, int line)
{
  //each dec num is added to the sum
  //k is a loop counter
  //integer is the dec value of the converted character
  //neg is -1 if there's a negative number
  int neg=1;
  long integer, sum=0;

  try
  {
      //set determine if it's a negative number, set counters appropriately
      if(op[0]=='-')
      {
	  neg=-1;
	  char ch;
	  (void) Remove(op, ch, 0);// remove negative sign
      }//end if
      //the number is not negative
      else if(op[0]=='+')
      {
	  char ch;
	  (void) Remove(op, ch, 0);// remove plus sign
      }//end else if
      
      for(int k=0;k<op.length();k++)
      {
	  //convert the character
	  integer=(long)op[k] - 48;
	  sum = sum*10 + integer;
      }//end for
      
      sum*=neg;

      table.op = (Operations)table.src_arr[line].opcode;
      switch(table.op)
      {
	  // 0-65535 range
      case BR: case BRP: case BRZ: case BRZP: case BRN: case BRNP:
      case BRNZ: case BRNZP: case JSR: case JMP: case LD: case LDI:
      case LEA: case ST: case STI: case BLKW:
	  if(sum < 0 or sum > 65535)
	      throw "ERROR: #CD02\n\tInvalid Decimal String: '#" + op +
		  "' Out of Range: [0 : 65,535]";
	  break;
      case EQU:
	  if(sum < -32768 or sum > 65535)
	      throw "ERROR: #CD03\n\tInvalid Decimal String: '#" + op +
		  "' Out of Range: [-32,768 : 65,535]";
	  break;
      default:
	  if(sum < -32768 or sum > 32767)
	      throw "ERROR: #CD01\n\tInvalid Decimal String: '#" + op +
		  "' Out of Range: [-32,768 : 32,767]";
	  break;
      }// end switch
      
      ret=(unsigned short)sum;
      return true;
  }//end try
  catch (string e)
  {
      cerr << "(" << line << ") " << e << endl;
      return false;
  }
}

bool Convert_Register(unsigned short& ret, string op, int line)
{
    long sum=0, integer;
    try
    {
	for(int k=0;k<op.length();k++)
	{
	    //convert the character
	    integer=(long)op[k] - 48;
	    sum = sum*10 + integer;
	}//end for
	if (sum < 0 or sum > 7)
	{
	    throw "ERROR: #CR01\n\tInvalid Operand: Register Index '" +
		op + "' is not 0-7";
	}
	ret = (unsigned short)sum;
	return true;
    }
    catch(string e)//register index not 0-7
    {
	cerr << "(" << line << ") " << e << endl;
	return false;
    }
}

// finds symbols in table
bool Find(Tables& table, string sym, unsigned short& value, bool& rel)
{
    if(table.sym_len == 0)
    {
	return false;
    }
    else
    {
	for(int k=0; k<table.sym_len;k++)// loop through all defined symbols
	{
	    if (sym == table.sym_arr[k].name)// if symbol is found
	    {
		value = table.sym_arr[k].value;// pass back its address
		rel = table.sym_arr[k].rel;// pass back 'relativity'
		return true;
	    }
	}
	return false;// specified symbol not found
    }
}

// finds entry symbols in entry table
bool Find(Tables& table, string sym, bool& defined, unsigned short& value)
{
    if(table.ent_len == 0)
    {
	return false;
    }
    else
    {
	for(int k=0; k<table.ent_len;k++)// loop through all defined symbols
	{
	    if (sym == table.ent_arr[k].name)// if symbol is found
	    {
		value = table.ent_arr[k].value;// pass back its address
		defined = table.ent_arr[k].defined; //value defined or not
		return true;
	    }
	}
	return false;// specified symbol not found
    }
}

// finds external symbols in table
bool Find(Tables& table, string sym)
{
    if(table.ext_len == 0)
    {
	return false;
    }
    else
    {
	for(int k=0; k<table.ext_len;k++)// loop through all defined symbols
	{
	    if (sym == table.ext_arr[k].name)// if symbol is found
	    {
		return true;
	    }
	}
	return false;// specified symbol not found
    }
}

// finds literals in table
bool Find(Tables& table, unsigned short value, unsigned short& address, bool& rel)
{
    for(int k=0; k<table.lit_len;k++)// loop through all defined literals
    {
	if (value == table.lit_arr[k].value)// if literal is found
	{
	    address = table.lit_arr[k].address;// pass back its address
	    rel = table.lit_arr[k].rel;// pass back literal relativity
	    return true;
	}
    }
    return false;// specified literal not found
}

// inserts symbols into table
void Insert(Tables& table, string str, unsigned short value, bool rel)
{
    table.sym_arr[table.sym_len].name = str;
    table.sym_arr[table.sym_len].value = value;
    table.sym_arr[table.sym_len].rel = rel;
    table.sym_len++;
}

// inserts literals into table
void Insert(Tables& table, unsigned short value, bool rel)
{
    table.lit_arr[table.lit_len].rel = rel;
    table.lit_arr[table.lit_len].value = value;
    table.lit_len++;
}

void Initialize(Tables& table)
{
    // initialize operations map
    table.ops_map["BR"]=BR;
    table.ops_map["ADD"]=ADD;
    table.ops_map["LD"]=LD;
    table.ops_map["ST"]=ST;
    table.ops_map["JSR"]=JSR;
    table.ops_map["AND"]=AND;
    table.ops_map["LDR"]=LDR;
    table.ops_map["STR"]=STR;
    table.ops_map["DBUG"]=DBUG;
    table.ops_map["NOT"]=NOT;
    table.ops_map["LDI"]=LDI;
    table.ops_map["STI"]=STI;
    table.ops_map["JSRR"]=JSRR;
    table.ops_map["RET"]=RET;
    table.ops_map["LEA"]=LEA;
    table.ops_map["TRAP"]=TRAP;
    table.ops_map[".ORIG"]=ORIG;
    table.ops_map[".END"]=END;
    table.ops_map[".EQU"]=EQU;
    table.ops_map[".FILL"]=FILL;
    table.ops_map["JMP"]=JMP;
    table.ops_map[".STRZ"]=STRZ;
    table.ops_map[".BLKW"]=BLKW;
    table.ops_map[".ENT"]=ENT;
    table.ops_map[".EXT"]=EXT;
    table.ops_map["JMPR"]=JMPR;
    table.ops_map["BRN"]=BRN;
    table.ops_map["BRNZ"]=BRNZ;
    table.ops_map["BRNZP"]=BRNZP;
    table.ops_map["BRZ"]=BRZ;
    table.ops_map["BRZP"]=BRZP;
    table.ops_map["BRP"]=BRP;
    table.ops_map["BRNP"]=BRNP;

    // initialize source table opcode value
    for(int k=1;k<=SRC_ARR_SZ;k++)
    {
	table.src_arr[k].opcode = CMNT;
    }
    
    
    table.src_len = 0;
    table.sym_len = 0;
    table.lit_len = 0;
    table.ext_len = 0;
    table.ent_len = 0;

    for(int k=1;k<ENT_ARR_SZ;k++)
    {
	table.ent_arr[k].defined = false;
    }

    table.start = 0;
    table.end = 0;
    table.exec = 0;
}

void Initialize(
    W05& machine
    )
{
    machine.ccr = 2;
    machine.pc = 0;
    for (int i=0; i<8; i++)
    {
	machine.reg_arr[i] = 0;
    }
    for (int i=0; i<65536; i++)
    {
	machine.mem_arr[i] = 0;
    } 
}

bool Convert_Hex(unsigned short& ret, char op[], int line, char file[])
{
  //each hex# is added to the sum
  //integer is the hex value of the converted character
  long sum=0, integer=0;
  try
    {
      string err;
      for(int k=0; k<strlen(op); k++)
	{
	  // convert hex numbers
	  if('0' <= op[k] and op[k] <= '9')
	    {
	      integer = (long)(op[k] - 48);
	    }//fi
	  // convert uppercase hex letters
	  else if('A' <= toupper(op[k]) and 'F' >= toupper(op[k]))
	    {
	      integer = (long)(toupper(op[k]) - 55);
	    }//esle
	  else
	    {
	      err="link ERROR: #2\n\tString contains non-hex characters.";
	      throw err;
	    }//esle
	  //add all the converted characters up
	  sum=sum*16 + integer;
	}//rof
      
    }
  catch(string e)
    {
      ret=0;
      cerr<<e<<endl<<"\tFILE: "<<file<<"\tLINE: "<<line<<endl;
    return false;
    }
  
  //hex number is greater than max allowed value
  
  ret=(unsigned short)sum;
  return true;
  
}

bool Defined(Total& table, char value[], int &counter)
{
  for(int k=0; k<table.N_length;k++)// loop through all defined external symbols
    {
      if (strcmp(table.external[k].name, value)==0)// if external symbol is found
	{
	  counter=k;  //pass back the index where the symbol was found
	  return true;
	}//fi
    }//rof
  return false;// specified symbol not found
}

//get a specified number of charcters or throw error if there aren't that many
bool Get(char arr[], int num, ifstream &input, int line, char file[])
{
  int counter=0;
  char ch;
  try
    {
      do
	{
	  input.get(ch);
	  
	  if(ch!='\n')
	    {
	      arr[counter]=ch;
	    }//fi
	  else
	    {
	      input.unget();
	      string err ="link ERROR: #1\n\tCharacter expected, newline received.";
	      throw err;	      
	    }//esle
	  
	  counter++;
	}while(counter<num & ch!='\n');	
    }
  catch(string e)
    {
      cerr<<e<<endl<<"\tFILE: "<<file<<"\tLINE: "<<line<<endl;
      return false;
    }
  return true;
}

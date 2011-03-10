// Writer.cpp
// Author: Travis Nauman
// Date: February 14, 2005
// Modified: Travis Nauman, February 28, 2005
// Class: CSE560

#include "Writer.h"

void Writer(
    Tables& tab,
    int index,
    ofstream& object,
    ofstream& list,
    unsigned short instruction
    )
{

    // initialize opcodes array
    string opcodes[37] = {
	"","ADD    ","LD     ","ST     ","JSR    ","AND    ","LDR    ",
	"STR    ","DBUG   ","NOT    ","LDI    ","STI    ","JSRR   ",
	"RET    ","LEA    ","TRAP   ",".ORIG  ",".END   ",".EQU   ",
	".FILL  ","JMP    ",".STRZ  ",".BLKW  ","CMNT   ",
	".EXT   ",".ENT   ","","","JMPR   ","BR     ","BRP    ",
	"BRZ    ","BRZP   ","BRN    ","BRNP   ","BRNZ   ","BRNZP  "
    };

    tab.op = (Operations)tab.src_arr[index].opcode;

    // following 4 used only in STRZ case
    bool object_open, list_open, bdummy;
    int opcode = tab.src_arr[index].opcode;
    unsigned short udummy;
    char ch;

    if(opcode > 28)
    {
	// move BR ops down to use in the string ARRAY
	opcode -= 51;
    }

    //cout << "OPCODE = " << opcodes[opcode] << endl;
    
    object_open = object.is_open();
    list_open = list.is_open();
    
    string st = tab.src_arr[index].op1;
    
    switch (tab.op)
    {
    case ORIG:
    {
	if(object_open)
	{   
	    // create header record
	    if(tab.src_arr[index].rel)
	    {
		object << "R";
	    }
	    else
	    {
		object << "H";
	    }

	    object << setw(6) << left << setfill(' ')
		   << tab.src_arr[index].label
		   << setw(4) << setfill('0')
		   << right << uppercase
		   << hex << tab.start
		   << setw(4) << tab.end-tab.start+1 << endl;
	}
	if(list_open)
	{ 
	    list << setw(32) << setfill(' ') << " "
		 << "(" << dec << setw(4) << right << index << ") "
		 << setw(15) << left << tab.src_arr[index].label
		 << setw(6) << opcodes[opcode]
		 << tab.src_arr[index].op1 << endl;
	}
    }
    break;

    case ENT:
    {
	unsigned short value;
	
	if(list_open)
	{
	    list << setw(32) << setfill(' ') << " "
		 << "(" << dec << setw(4) << right << index << ") "
		 << setw(15) << opcodes[opcode] << " "
		 << tab.src_arr[index].op1 << ' '
		 << tab.src_arr[index].op2 << ' '
		 << tab.src_arr[index].op3 << ' '
		 << tab.src_arr[index].op4 
		 << endl;
	}
	if(object_open)
	{
	    Find(tab, tab.src_arr[index].op1, bdummy, value);
	    object << 'N' << tab.src_arr[index].op1 << "="
		   << setw(4) << setfill('0') << value << endl;
	    if (tab.src_arr[index].op2.length() > 0)
	    {
		Find(tab, tab.src_arr[index].op2, bdummy, value);
		object << 'N' << tab.src_arr[index].op2 << "="
		       << setw(4) << setfill('0') << value << endl;
		if (tab.src_arr[index].op3.length() > 0)
		{
		    Find(tab, tab.src_arr[index].op3, bdummy, value);
		    object << 'N' << tab.src_arr[index].op3 << "="
			   << setw(4) << setfill('0') << value << endl;
		    if (tab.src_arr[index].op4.length() > 0)
		    {
			Find(tab, tab.src_arr[index].op4, bdummy, value);
			object << 'N' << tab.src_arr[index].op4 << "="
			       << setw(4) << setfill('0') << value << endl;
		    }
		}
	    }
	    
	}
    }
    break;

    case EXT:
    {
	if(list_open)
	{
	    list << setw(32) << setfill(' ') << " "
		 << "(" << dec << setw(4) << right << index << ") "
		 << setw(15) << opcodes[opcode] << " "
		 << tab.src_arr[index].op1 << ' '
		 << tab.src_arr[index].op2 << ' '
		 << tab.src_arr[index].op3 << ' '
		 << tab.src_arr[index].op4 
		 << endl;
	}
    }
    break;
    
    case END:
    {
	if(list_open)
	{   
	    list << setw(32) << setfill(' ') << " "
		 << "(" << dec << setw(4) << right << index << ") "
		 << setw(15) << left << tab.src_arr[index].label
		 << setw(6) << opcodes[opcode] << " "
		 << tab.src_arr[index].op1 << endl;
	}
    }
    break;
    
    case EQU:
    {
	if(list_open)
	{   
	    list << setw(32) << setfill(' ') << " "
		 << "(" << dec << setw(4) << right << index << ") "
		 << setw(15) << left << tab.src_arr[index].label
		 << setw(6) << opcodes[opcode]
		 << tab.src_arr[index].op1 << endl;
	}
    }   
    break;
	
    case STRZ:
    {
	int startAddr = tab.src_arr[index].address,
	    length = tab.src_arr[index].op1.length(), i = 2;
	if (length == 2)
	{
	    ch = '\0';
	}
	else
	{
	    
	    ch = tab.src_arr[index].op1[1];
	}

	if(object_open)
	{
	    object << "T" << right << hex << uppercase
		   << setw(4) << setfill('0')
		   << startAddr << setw(4)
		   << (int)ch << endl; 
	}
	if(list_open)
	{
	    list << "(" << hex << setw(4) << setfill('0')
		 << uppercase << startAddr
		 << ") " << setw(4) << right
		 << nouppercase << (int)ch << "  ";
	    
	    Print_Bin((int)ch, list);
	    list << "   (" << dec << setfill(' ') << setw(4)
		 << right << index << ")   "
		 << left << setw(15) << setfill(' ')
		 << tab.src_arr[index].label
		 << opcodes[opcode] << " "
		 << tab.src_arr[index].op1 << endl;
	}

	startAddr++;

	ch = tab.src_arr[index].op1[i];
	while(true)
	{
	    if(object_open)
	    {
		object << "T" << right << uppercase
		       << hex << setw(4)
		       << setfill('0') << startAddr
		       << setw(4)
		       << (int)ch << '\n';
	    }
	    if(list_open)
	    {
		list << "(" << hex << uppercase
		     << setw(4) << setfill('0') << startAddr
		     << ") " << setw(4) << right
		     << nouppercase << (int)ch << "  ";
		Print_Bin((unsigned short)ch,list);
		list << "   (" << dec << setw(4)
		     << setfill(' ') << right << index << ")" << endl;
	    }
	    startAddr++;
	    i++;

	    ch = tab.src_arr[index].op1[i];
	    
	    if(ch == '"')
	    {
		ch = '\0';
		if(object_open)
		{
		    object << right << "T" << hex << setw(4)
			   << uppercase << setfill('0')
			   << startAddr << setw(4)
			   << (int)ch << '\n';
		}
		if(list_open)
		{
		    list << "(" << hex << setw(4)
			 << setfill('0')
			 << uppercase << startAddr
			 << ") " << setw(4) << right
			 << nouppercase << (int)ch << "  ";
		    Print_Bin((unsigned short)ch,list);
		    list << "   (" << right << dec << setw(4)
			 << setfill(' ') << index << ")" << endl;
		}
		break;
	    }
	}
    }
    break;
	
    case FILL:
    {
	if(list_open)
	{
	    list << "(" << hex << setw(4) << uppercase
		 << right << setfill('0')
		 << tab.src_arr[index].address << ") "
		 << setfill('0') << setw(4) << right << instruction
		 << "  ";
	    Print_Bin(instruction, list);
	    list << " (" << dec << setw(4) << setfill(' ')
		 << right << nouppercase << index << ") "
		 << setw(15) << setfill(' ')
		 << left << tab.src_arr[index].label
		 << opcodes[opcode] << " "
		 << tab.src_arr[index].op1 << endl;
	}
	if(object_open)
	{
	    if (Find(tab, tab.src_arr[index].op1))
	    {
		object << "Y";
	    }
	    else if (tab.src_arr[index].rel)
	    {
		object << "L";
	    }
	    else
	    {
		object << "T";
	    }
	    object << right << uppercase << hex
		   << setw(4) << setfill('0')
		   << tab.src_arr[index].address
		   << setw(4) << instruction;
	    if (Find(tab, tab.src_arr[index].op1))
	    {
		object << tab.src_arr[index].op1;
	    }
	    object << endl;
	}
    }
	    
    case CMNT:
    {
    }
    break;
    
    case BR:
    case BRN:
    case BRZ:
    case BRP:
    case BRNZ:
    case BRNP:
    case BRZP:
    case BRNZP:
    case JSR:
    case JMP:
    {
	if(object_open)
	{
	    if (Find(tab, tab.src_arr[index].op1))
	    {
		object << "X";
	    }
	    else if (tab.src_arr[index].rel)
	    {
		object << "S";
	    }
	    else
	    {
		object << "T";
	    }
	    object << right << hex << uppercase
		   << setw(4) << setfill('0')
		   << tab.src_arr[index].address
		   << setw(4) << instruction;
	    if (Find(tab, tab.src_arr[index].op1))
	    {
		object << tab.src_arr[index].op1;
	    }
	    object << endl;
	}
	
	if(list_open)
	{
	    list << "(" << hex << right << uppercase
		 << setw(4) << setfill('0')
		 << tab.src_arr[index].address << ") "
		 << hex << setw(4) << setfill('0')
		 << right << uppercase << instruction << "  ";
	    Print_Bin(instruction, list);
	    if (Find(tab, tab.src_arr[index].op1))
	    {
		list << " X";
	    }
	    else
	    {
		list << "  ";
	    }
	    
	    list << " (" << dec << setw(4) << setfill(' ')
		 << right << index << ") " << setw(15) << setfill(' ')
		 << left << tab.src_arr[index].label
		 << opcodes[opcode] << " " 
		 << tab.src_arr[index].op1 << " " 
		 << tab.src_arr[index].op2 << " "
		 << tab.src_arr[index].op3 << endl;
	}	
    }
    break;
    case LD:
    case LDI:
    case LEA:
    case ST:
    case STI:
    {
	if(object_open)
	{
	    if (Find(tab, tab.src_arr[index].op2))
	    {
		object << "X";
	    }
	    else if (tab.src_arr[index].rel)
	    {
		object << "S";
	    }
	    else
	    {
		object << "T";
	    }
	    object << right << hex << uppercase
		   << setw(4) << setfill('0')
		   << tab.src_arr[index].address
		   << setw(4) << instruction;
	    if (Find(tab, tab.src_arr[index].op2))
	    {
		object << tab.src_arr[index].op2;
	    }
	    object << endl;
	}

	if(list_open)
	{
	    list << "(" << hex << right << uppercase
		 << setw(4) << setfill('0')
		 << tab.src_arr[index].address << ") "
		 << hex << setw(4) << setfill('0')
		 << right << uppercase << instruction << "  ";
	    Print_Bin(instruction, list);
	    if (Find(tab, tab.src_arr[index].op2))
	    {
		list << " X";
	    }
	    else
	    {
		list << "  ";
	    }
	    
	    list << " (" << dec << setw(4) << setfill(' ')
		 << right << index << ") " << setw(15) << setfill(' ')
		 << left << tab.src_arr[index].label
		 << opcodes[opcode] << " " 
		 << tab.src_arr[index].op1 << " " 
		 << tab.src_arr[index].op2 << " "
		 << tab.src_arr[index].op3 << endl;   
	    
	}
    }
    break;
    default:
    {
	if(object_open and tab.op != BLKW)
	{
	    if (tab.src_arr[index].rel)
	    {
		object << "S";
	    }
	    else
	    {
		object << "T";
	    }
	    object << right << hex << uppercase
		   << setw(4) << setfill('0')
		   << tab.src_arr[index].address
		   << setw(4) << instruction;
	    if (Find(tab, tab.src_arr[index].op2))
	    {
		object << tab.src_arr[index].label;
	    }
	    object << endl;
	}

	if(list_open)
	{
	    list << "(" << hex << right << uppercase
		 << setw(4) << setfill('0')
		 << tab.src_arr[index].address << ") ";
	    if (tab.op != BLKW)
	    {
		list << hex << setw(4) << setfill('0')
		     << right << uppercase << instruction << "  ";
		Print_Bin(instruction, list);
	    }
	    else
	    {
		list << setw(22) << setfill(' ') << " ";
	    }	   
	    list << "   (" << dec << setw(4) << setfill(' ')
		 << right << index << ") " << setw(15) << setfill(' ')
		 << left << tab.src_arr[index].label
		 << opcodes[opcode] << " " 
		 << tab.src_arr[index].op1 << " " 
		 << tab.src_arr[index].op2 << " "
		 << tab.src_arr[index].op3 << endl;   
	}
    }
    break;  
    }
}
    

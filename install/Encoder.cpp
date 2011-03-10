#include "Encoder.h"

bool Encoder(
    Tables& table,
    int line,
    unsigned short& instruction
    )
{
    unsigned short op1=0,op2=0,op3=0; // operands 1, 2, and 3
    bool r1=false,r2=false,r3=false,rel=false; // relativity of operands
    unsigned short dummy;

    instruction = table.src_arr[line].opcode << 12, // opcode
    table.op = (Operations)table.src_arr[line].opcode;

    if(table.op != ENT and table.op != EXT)
    {
	(void)Text_To_Int(op1,table.src_arr[line].op1,table,line); // get op1
	(void)Text_To_Int(op2,table.src_arr[line].op2,table,line); // get op2
	(void)Text_To_Int(op3,table.src_arr[line].op3,table,line); // get op3
    }

    // Register operands never less than zero due to
    // range of 'unsigned short'
    // all other operands must be checked below
    try{
	// check relativity of operands
	// if symbol not found in table, operand is absolute
	// else if symbol IS in table, operand relativity is
	// stored in r1/r2
	switch (table.op)
	{
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
	case FILL:
	{
	    // look in OP1
	    if(!(table.src_arr[line].op1[0] == 'x' or
		 table.src_arr[line].op1[0] == '#'))
	    {
		// operand is a symbol
		// if the symbol is found set the relativity of the
		// line to the relativity of the symbol
     		(void)Find(table, table.src_arr[line].op1,
			   dummy, rel);
		table.src_arr[line].rel = rel;		
	    }
	}
	break;
	case LD:
	case LDI:
	case LEA:
	case ST:
	case STI:
	{
	    // check op 2
	    if(table.src_arr[line].op2[0] == '=')
	    {
		// look up the relativity of the literal
		unsigned short value;
		string tempstring = table.src_arr[line].op2;
		
		(void)Text_To_Int(value, tempstring, table, line);
		// value is now the value of the literal, so we can
		// look it up using Find

		(void)Find(table, value, dummy, rel);
	    }
	    else if(!(table.src_arr[line].op2[0] == 'x' or
		      table.src_arr[line].op2[0] == '#'))
	    {
		// operand is a symbol
		// if the symbol is found set the relativity of the
		// line to the relativity of the symbol
     		(void)Find(table, table.src_arr[line].op2, dummy, rel);
	    }
	    table.src_arr[line].rel = rel;
	}
	break;
	}
	    
	// not in symbol/external/entry table
	if(!Find(table, table.src_arr[line].op1, dummy, r1) and
	   !Find(table, table.src_arr[line].op1, r1, dummy) and
	   !Find(table, table.src_arr[line].op1))  
	{
	    r1 = false;
	}
	if(!Find(table, table.src_arr[line].op2, dummy, r2) and
	   !Find(table, table.src_arr[line].op2, r2, dummy) and
	   !Find(table, table.src_arr[line].op2))
	{
	    r2 = false;
	}
	// if symbol is found in table and is relative,
	// or is in entry/external table,
	// must be invalid (no operand 3 can ever be relative)
	if(Find(table, table.src_arr[line].op3, dummy, r3))
	{
	    if(r3 or Find(table, table.src_arr[line].op2, r3, dummy) or
	       Find(table, table.src_arr[line].op2))
	    {
		throw "ERROR: #ENC16\n\tInvalid Operand '" +
		    table.src_arr[line].op3 +
		    "'. Relocatable Symbol used in Invalid Location";
	    }
	}
	
	// switch case according to operation
	switch (table.op)
	{
	case ADD:
	case AND:
	    // check relativity of operands
	    if(r1) throw "ERROR: #ENC16\n\tInvalid Operand '" +
		       table.src_arr[line].op1 +
		       "'. Relocatable Symbol used in Invalid Location";
	    if(r2) throw "ERROR: #ENC16\n\tInvalid Operand '" +
		       table.src_arr[line].op2 +
		       "'. Relocatable Symbol used in Invalid Location";
	    
	    if(op1 > 7) // op1 (DR) out of range
	    {
		throw "ERROR: #ENC01\n\t'" + table.src_arr[line].op1 + "'" +
		    " Operand #1 (DR) Out of Range [0 : 7]";
	    }
	    if(op2 > 7) // op2 (SR1) out of range
	    {
		throw "ERROR: #ENC02\n\t'" + table.src_arr[line].op2 + "'" +
		    " Operand #2 (SR1) Out of Range [0 : 7]";
	    }
	    
	    op1 <<= 9; // adjust DR into position
	    op2 <<= 6; // adjust SR1 into position
	    
	    // if third operand is not register, adjust it and add flag
	    // bit; otherwise SR2 is already adjusted properly
	    if(table.src_arr[line].op3[0] != 'R') // op3 isn't a register
	    {
		if(op3 > 0xF and op3 < 0xFFF0) // op3 (imm5) out of range
	 	{
		    throw "ERROR: #ENC03\n\t'" +
			table.src_arr[line].op3 + "'" +
			" Operand #3 (imm5) Out of Range [-16 : 15]";
		}
		op3 &= 0x3F; // mask off top bits to get imm5
		op3 |= 0x20; // put 'ADD register/imm5' flag bit into op3
	    }
	    else if(op3 > 7)
	    {
		throw "ERROR: #ENC04\n\t'" +
		    table.src_arr[line].op3 + "'" +
		    " Operand #3 (DR) Out of Range [0 : 7]";
	    }
	    // op3 already in position
	    break;
	case BR:
	case BRP:
	case BRZ:
	case BRZP:
	case BRN:
	case BRNP:
	case BRNZ:
	case BRNZP:
	    instruction >>= 3;
	    if(((table.src_arr[line].address + 1) | 0x1FF) !=
	       (op1 | 0x1FF)) // op1 (pgoffset9) on wrong page
	    {
		throw "ERROR: #ENC05\n\t'" +
		    table.src_arr[line].op1 + "'" +
		    " Operand #1 (pgoffset9) Crosses Page Boundary";
	    }
	    op1 &= 0x1FF; // mask off top bits to get pgoffset9
	    break;
	case LD:
	    // operand is literal
	    if(table.src_arr[line].op2[0] == '=') 
	    {
		if(!Find(table, op2, op2, rel))
		{
		    throw "ERROR: #ENC17\n\tLiteral '"
			+ table.src_arr[line].op2
			+ "' Not in Table";
		}
		table.src_arr[line].rel = rel;
	    }
	    // no 'break' intentional
	    // LD case must include statements in LDI-STI case below
	case LDI:
	case LEA:
	case ST:
	case STI:
	    // check relativity of operands
	    if(r1) throw "ERROR: #ENC16\n\tInvalid Operand '" +
		       table.src_arr[line].op1 +
		       "'. Relocatable Symbol used in Invalid Location";
	    if(op1 > 7) // op1 (register) out of range
	    {
		throw "ERROR: #ENC06\n\t'" +
		    table.src_arr[line].op1 + "'" +
		    " Operand #1 (DR/SR) Out of Range [0 : 7]";
	    }
	    if(((table.src_arr[line].address + 1) | 0x1FF) !=
	       (op2 | 0x1FF)) // op2 (pgoffset9) on wrong page
	    {
		throw "ERROR: #ENC07\n\t'" +
		    table.src_arr[line].op2 + "'" +
		    " Operand #2 (pgoffset9) Crosses Page Boundary";
	    }
	    op1 <<= 9; // adjust op1 into position
	    op2 &= 0x1FF; // mask off top bits to get pgoffset9
	    // op2 already in position, op3 is zero
	    break;
	case JMP:
	case JSR:	
	    if(((table.src_arr[line].address + 1) | 0x1FF) !=
	       (op1 | 0x1FF)) // op1 (pgoffset9) on wrong page
	    {
		throw "ERROR: #ENC08\n\t'" + table.src_arr[line].op1 + "'" +
		    " Operand #1 (pgoffset9) Crosses Page Boundary";
	    }
	    op1 &= 0x1FF; // mask off top bits to get pgoffset9
	    if(table.src_arr[line].opcode == JSR)
	    {
		op2 |= 0x800; // put LINK bit into op2
	    }
	    // op2 already in position, op3 is zero
	    break;
	case JMPR:
	case JSRR:
	    // check relativity of operands
	    if(r1) throw "ERROR: #ENC16\n\tInvalid Operand '" +
		       table.src_arr[line].op1 +
		       "'. Relocatable Symbol used in Invalid Location";
	    if(r2) throw "ERROR: #ENC16\n\tInvalid Operand '" +
		       table.src_arr[line].op2 +
		       "'. Relocatable Symbol used in Invalid Location";
	    if(op1 > 7) // op1 (BR) out of range
	    {
		throw "ERROR: #ENC09\n\t'" + table.src_arr[line].op1 + "'" +
		    " Operand #1 (BR) Out of Range [0 : 7]";
	    }
	    if(op2 > 0x1F and op2 < 0xFFE0) // op2 (index6) out of range
	    {
		throw "ERROR: #ENC10\n\t'" + table.src_arr[line].op2 + "'"
		    " Operand #2 (index6) Out of Range [-64 : 63]";
	    }
	    op1 <<= 6; // adjust op1 into position
	    if(table.src_arr[line].opcode == JSRR)
	    {
		op2 |= 0x800; // put LINK bit into op2
	    }
	    // op2 already in position, op3 is zero
	    break;
	case LDR:
	case NOT:
	case STR:
	    // check relativity of operands
	    if(r1) throw "ERROR: #ENC16\n\tInvalid Operand '" +
		       table.src_arr[line].op1 +
		       "'. Relocatable Symbol used in Invalid Location";
	    if(r2) throw "ERROR: #ENC16\n\tInvalid Operand '" +
		       table.src_arr[line].op2 +
		       "'. Relocatable Symbol used in Invalid Location";
	    if(op1 > 7) // op1 (register) out of range
	    {
		throw "ERROR: #ENC11\n\t'" + table.src_arr[line].op1 + "'" +
		    " Operand #1 (DR/SR) Out of Range [0 : 7]";
	    }
	    if(op2 > 7) // op2 (register) out of range
	    {
		throw "ERROR: #ENC12\n\t'" + table.src_arr[line].op2 + "'" +
		    " Operand #2 (BR/SR) Out of Range [0 : 7]";
	    }
	    if(op3 > 0x1F and op3 < 0xFFE0) // op3 (index6) out of range
	    {
		throw "ERROR: #ENC13\n\t'" + table.src_arr[line].op3 + "'" +
		    " Operand #3 (index6) Out of Range [-64 : 63]";
	    }
	    op1 <<= 9; // adjust op1 into position
	    op2 <<= 6; // adjust op2 into position
	    // op3 already in position
	    break;
	case TRAP:
	    // check relativity of operands
	    if(r1) throw "ERROR: #ENC16\n\tInvalid Operand '" +
		       table.src_arr[line].op1 +
		       "'. Relocatable Symbol used in Invalid Location";
	    if(op1 > 0xFF and op1 < 0xFF10) // op1 (trapvect8) out of range
	    {
		throw "ERROR: #ENC14\n\t'" + table.src_arr[line].op1 + "'" +
		    " Operand #1 (trapvect8) Out of Range [0x00 : 0xFF]";
	    }
	    // check for valid trap vector
	    switch (op1)
	    {
	    case 0x21:
	    case 0x22:
	    case 0x23:
	    case 0x25:
	    case 0x31:
	    case 0x33:
	    case 0x43:
		break;
	    default:
		throw "WARNING: #ENC15\n\t'" + table.src_arr[line].op1 + "'" +
		    " Invalid Trap Vector";
	    }
	    // op1 already in position, op2 & op3 are zero
	    break;
	case FILL:
	    instruction = 0;
	    // .FILL operand is symbol
	    if(table.src_arr[line].op1[0] != 'x' and
	       table.src_arr[line].op1[0] != '#')
	    {
		unsigned short dummy;
		// look it up in table
		bool rel;
		(void)Find(table,table.src_arr[line].op1,
			   dummy,rel);
		table.src_arr[line].rel = rel;
	    }
	    
	    // clear opcode value
	    // op1 contains full value already in position
	    // op2 & op3 are zero
	    break;
	case ENT:  // writer handles this
	case EXT:  // writer handles this
	case END:  // relativity checked by parser
	case ORIG: // start and length explicit in table
	case EQU:  // no associated object instruction
	case STRZ: // character string is stored when writing files
	case BLKW: // no values stored
	case DBUG: // op1, op2, & op3 are zero
	case RET:  // op1, op2, & op3 are zero
	case CMNT: // instruction value is irrelevant
	default:
	    break;
	}
    }
    catch(string e)
    {
	cerr << "(" << line << ") " << e << endl;
	return false;
    }
    
    // create final encoded instruction
    instruction = instruction | op1 | op2 | op3;
//    cout << instruction << endl;
    return true;
}

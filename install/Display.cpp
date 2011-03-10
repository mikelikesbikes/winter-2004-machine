#include "Display.h"

// displays a table with the contents of general purpose registers
void Display_Reg(W05& state)
{
    cout << hex << uppercase;
    cout << "\nREGISTERS\n";
    cout << "-------------------------------------------------------------\n";
    for (int i=0; i <=7; i++)
    {
	cout << "R" << i <<": " << state.reg_arr[i] << "   ";
    }
    cout << "\n\n";
    cout << "PC: " << state.pc;
    switch(state.ccr)
    {	    
    case 1:
	cout << "   N: 0    Z: 0    P: 1\n\n";
	break;
	
    case 2:
	cout << "   N: 0    Z: 1    P: 0\n\n";
	break;
	
    case 4:
	cout << "   N: 1    Z: 0    P: 0\n\n";
	break;
    }
}

// displays a table showing the contents of memory starting with the
// address specified by the user
void Display_Mem(W05& state, int& startMem)
{
    // cannot display memory past 0xFFFF
    if (startMem > 0xFFFF)
    {
	startMem = 0xFFFF;
    }
    startMem = (startMem & 0xFFF0);
    int tempstartMem = startMem;
    cout << "\nMEMORY\n------\n\nADDR"
	 << "   +0   +1   +2   +3   +4   +5   +6   +7"
	 << "   +8   +9   +A   +B   +C   +D   +E   +F\n"
	 << "----------------------------------------"
	 << "---------------------------------------------\n";
    for (int i=0; i<8; i++)
    {
	cout << setw(4) << setfill('0') << hex  << uppercase << startMem << "  ";
	for (int j=0; j<16; j++)
	{
	    cout << hex << setfill('0') << setw(4)
		 << uppercase << state.mem_arr[startMem] << " ";
	    startMem++;

	    // if the end of memory is reached, stop the display where
	    // it is and inform the user
	    if (startMem > 65535)
	    {
		break;
	    }
	}
	cout << "  ";
	for (int j=0; j<16; j++)
	{
	    if(state.mem_arr[tempstartMem] >= 0x20 and state.mem_arr[tempstartMem] <= 0x7E)
	    {
		cout << (char)(state.mem_arr[tempstartMem] & 0x00FF);
	    }
	    else
	    {
		cout << ".";
	    }
	    tempstartMem++;
	    if(tempstartMem > 0xFFFF)
	    {
		break;
	    }
	}
	cout << "\n";
	if (startMem > 65535)
	{
	    cout << "\nEND OF MEMORY\n";
	    break;
	}
    }
    cout << "\n";
}

void Display_Help(bool done)
{
    cout << "COMMANDS\n"
	 << "--------\n\n";
    if(!done)
    {
	cout << "R - Quiet Run Mode\n"
	     << "T - Trace Mode\n"
	     << "S - Step Mode\n"
	     << "X - StepX Mode\n";
    }
    cout << "M - Dump Memory\n"
	 << "G - Display Registers\n"
	 << "I - Display Next Instruction\n"
	 << "H - Help Display\n";
    if(!done)
    {
	cout << "Q - Quit Loaded File\n\n";
    }
    else
    {
	cout << "Q - Quit IGYW\n\n";
    }
}

void Display_Next_Inst(W05& state)
{
    unsigned short value=state.mem_arr[state.pc];
    unsigned short dr, sr1, sr2, imm5, index6, trapvect8, pgoffset9;
    unsigned short oppbits, Lbit, Nbit, Zbit, Pbit, bit5;

    // mask the instruction value to get particular bits
    dr = value << 4;
    dr = dr >> 13;
    sr1 = value << 7;
    sr1 = sr1 >> 13;
    sr2 = value & 0x0007;
    imm5 = value & 0x001F;
    index6 = value & 0x003F;
    trapvect8 = value & 0x00FF;
    pgoffset9 = value & 0x01FF;

    oppbits = value & 0xF000;
    Nbit = value & 0x0800;
    Zbit = value & 0x0400;
    Pbit = value & 0x0200;
    bit5 = value & 0x0020;
    Lbit = value & 0x0800;
    
    cout << "\nNext Instruction:   ";

    switch(oppbits)
    {
    case 0x0000:
	cout << "BR ";
	if(Nbit != 0)
	{
	    cout << 'N';
	}
	if(Zbit != 0)
	{
	    cout << 'Z';
	}
	if(Pbit != 0)
	{
	    cout << 'P';
	}

	cout << hex << showbase << ' ' << pgoffset9;
	
	break;
    case 0x1000:
	cout << dec << noshowbase << "ADD %r" << dr << " %r" << sr1 << ' ';
	if(bit5 != 0)
	{
	    cout << hex << showbase << imm5;
	}
	else
	{
	    cout << dec << noshowbase << "%r" << sr2;
	}
	break;
    case 0x2000:
	cout << "LD %r" << dr << ' ' << hex << showbase << pgoffset9;
	break;
    case 0x3000:
	cout << "ST %r" << dr << ' ' << hex << showbase << pgoffset9;
	break;
    case 0x4000:
	if(Lbit != 0)
	{
	    cout << "JSR ";
	}
	else
	{
	    cout << "JMP ";
	}
	cout << hex << showbase << pgoffset9;
	break;
    case 0x5000:
	cout << dec << noshowbase << "AND %r" << dr << " %r" << sr1 << ' ';
	if(bit5 != 0)
	{
	    cout << hex << showbase << imm5;
	}
	else
	{
	    cout << dec << noshowbase << "%r" << sr2;
	}
	break;
    case 0x6000:
	cout << dec << noshowbase << "LDR %r" << dr << " %r" << sr1 << ' ';
	cout << hex << showbase << index6;
	break;
    case 0x7000:
	cout << dec << noshowbase << "STR %r" << dr << " %r" << sr1 << ' ';
	cout << hex << showbase << index6;
	break;
    case 0x8000:
	cout << "DBUG";
	break;
    case 0x9000:
	cout << dec << noshowbase << "NOT %r" << dr << " %r" << sr1;
	break;
    case 0xA000:
	cout << dec << noshowbase << "LDI %r" << dr << ' ';
	cout << hex << showbase << pgoffset9;
	break;
    case 0xB000:
	cout << dec << noshowbase << "STI %r" << dr << ' ';
	cout << hex << showbase << pgoffset9;
	break;
    case 0xC000:
	if(Lbit != 0)
	{
	    cout << "JSRR ";
	}
	else
	{
	    cout << "JMPR ";
	}
	cout << dec << noshowbase << dr << " %r" << sr1 << ' ';
	cout << hex << showbase << index6;
	break;
    case 0xD000:
	cout << "RET";
	break;
    case 0xE000:
	cout << dec << noshowbase << "LEA %r" << dr << ' ';
	cout << hex << showbase << pgoffset9;
	break;
    case 0xF000:
	cout << hex << showbase << "TRAP " << trapvect8;
	break;
    }   
    cout << "\n\n" << hex << noshowbase;
}

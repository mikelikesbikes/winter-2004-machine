#include "Tokenizer.h"

void Insert(
    Tokenizing_Machine& m,
    char& ch
    )
{
    switch (m.bs)
    {
    case EMPTY_BS:
    {
	m.bs = Buffer_Type(ch);
    }
    break;
    
    case NEWLINE_BS:
    {
	m.token_ready = true;
    }
    break;
	
    case OP_OR_SYMBOL_OR_REGISTER_BS:
    {
	if(m.buffer[0] == 'R')
	{
// 	    if(!Is_Digit_Char(ch))
// 	    {
// 		if(Size(m) == 1 and ch != 'E')
// 		{
// 		    m.token_ready = true;
// 		}
// 		else if(Size(m) == 2 and ch != 'T')
// 		{
// 		    m.token_ready = true;
// 		}
// 		else if(Size(m) > 2)
// 		{
// 		    m.token_ready = true;
// 		}
// 	    }

	    if (Size(m) == 1 and ch!= 'E' and !Is_Digit_Char(ch))
	    {
		m.token_ready = true;
	    }

	    else if (Size(m) == 2)
	    {
		if (m.buffer[1] == 'E')
		{
		    if (ch != 'T')
		    {
			m.token_ready = true;
		    }
		}
		else
		{
		    if (!Is_Digit_Char(ch))
		    {
			m.token_ready = true;
		    }
		}
	    }
	    else if (Size(m) == 3)
	    {
		if (m.buffer[2] == 'T')
		{
		    m.token_ready = true;
		}
		else
		{
		    if (!Is_Digit_Char(ch))
		    {
			m.token_ready = true;
		    }
		}
	    }
	    else if (Size(m) >= 4)
	    {
		if (!Is_Digit_Char (ch))
		{
		    m.token_ready = true;
		}
	    }

	}
	else if(! Is_Symbol_Char(ch))
	{
	    m.token_ready = true;
	}
    }
    break;

    case LITERAL_BS:
    {
	if((m.buffer.length() == 1) and !(ch == 'x' or ch == '#'))
	{
	    m.token_ready = true;
	}
	else if(m.buffer.length() == 2)
	{
	    if(m.buffer[1] == '#' and ! Is_Digit_Char(ch))
	    {
		if(!(ch =='-' or ch == '+'))
		{
		    m.token_ready = true;
		}
	    }
	    else if(m.buffer[1] == 'x' and !Is_Hex_Digit_Char(ch))
	    {
		m.token_ready = true;
	    }
	}

	else if(Size(m) > 2)
	{
	    if(m.buffer[1] == '#' and !Is_Digit_Char(ch))
	    {
		m.token_ready = true;
	    }
	    else if(m.buffer[1] == 'x' and !Is_Hex_Digit_Char(ch))
	    {
		m.token_ready = true;
	    }
	}  
    }
    break;

    case NUMBER_BS:
    {
	if(m.buffer[0] == '#' and ! Is_Digit_Char(ch))
	{
	    if(!((ch =='-' or ch == '+') and Size(m) == 1))
	    {
		m.token_ready = true;
	    }
	}
	else if(m.buffer[0] == 'x' and !Is_Hex_Digit_Char(ch))
	{
	    m.token_ready = true;
	}
    }
    break;

    case WHITE_SPACE_BS:
    {
	if(! Is_Whitespace_Char(ch))
	{
	    m.token_ready = true;
	}
    }
    break;

    case STRING_BS:
    {
	if(Size(m)>1 and m.buffer[Size(m)-1] == '\"')
	{
	    m.token_ready = true;
	}
    }
    break;
    
    case SEPARATOR_BS:
    {
	if(! Is_Whitespace_Char(ch))
	{
	    m.token_ready = true;
	}   
    }
    break;

    case COMMENT_BS:
    {
	if(ch == '\n')
	{
	    m.token_ready = true;
	}
    }
    break;
    
    case ERROR_BS:
    {
	if(Can_Start_Token(ch))
	{
	    m.token_ready = true;
	}
    }
    break;
    }
    m.buffer = m.buffer + ch;
}

void Dispense(
    Tokenizing_Machine& m,
    string& token_text,
    int& token_kind
    )
{
    char ch;
    // take the last character off the buffer and put it into ch
    
    Remove(m.buffer, ch, Size(m)-1);
    
    // token_text = buffer
    token_text = m.buffer;

    token_kind = Token_Kind(m.bs, token_text);

    m.bs = Buffer_Type(ch);

    m.buffer = ch;

    m.token_ready = false;
}

void Flush_A_Token(
    Tokenizing_Machine& m,
    string& token_text,
    int& token_kind
    )
{    

    
    // token text gets the entire buffer
    if(Size(m) > 0)
    {
	char ch;
	Remove(m.buffer, ch, Size(m)-1);
    }
    token_text = m.buffer;
    token_kind = Token_Kind(m.bs, m.buffer);	

    // reset machine
    Initialize_Machine(m);
}

bool Is_Ready_To_Dispense(
    Tokenizing_Machine& m
    )
{
    return m.token_ready;
}
    
int Size(
    Tokenizing_Machine& m
    )
{
    // return the length of the buffer
    return m.buffer.length();
}

bool Is_Whitespace_Char(
    char ch
    )
{
    return (ch == '\t' or ch == ' ');
}

bool Is_Digit_Char(
    char ch
    )
{
    return (ch>= '0' and ch <= '9');
}

bool Is_Hex_Digit_Char(
    char ch
    )
{
    return Is_Digit_Char(ch) or (ch >= 'a' and ch <= 'f') or (ch >= 'A' and ch <= 'F');
}

bool Is_Alphabetic_Char(
    char ch
    )
{
    return ((ch >= 'A' and ch <= 'Z') or (ch >= 'a' and ch <= 'z'));
}

bool Is_Symbol_Char(
    char ch
    )
{
    return (Is_Alphabetic_Char(ch) or Is_Digit_Char(ch));
}

bool Can_Start_Token(
    char ch
    )
{
    return (Is_Alphabetic_Char(ch) or Is_Whitespace_Char(ch) or ch == '#'
	    or ch == ',' or ch == '=' or ch == '.' or ch == ';' or ch == '\"');
}

bool Is_Branch_Op(
    string& t
    )
{
    return ((t == "BR") or (t == "BRN") or (t == "BRZ") or (t == "BRP")
	    or (t == "BRNZ") or (t == "BRZP") or (t == "BRNP")
	    or (t == "BRNZP"));
}

int Op_Or_Symbol_Or_Register(
    string& t
    )
{
    int result;
    if ((t == "ADD") or (t == "AND") or (t == "DBUG") or (t == "JSR")
	or (t == "JSRR") or (t == "LD") or (t == "LDI") or (t == "LDR")
	or (t == "LEA") or (t == "NOT") or (t == "RET") or (t == "ST")
	or (t == "STI") or (t == "STR") or (t == "TRAP") or (t == "JMP")
	or (t == "JMPR") or (t == ".ORIG") or (t == ".END") or (t == ".EQU")
	or (t == ".FILL") or (t == ".STRZ") or (t == ".BLKW") or (t == ".ENT")
	or (t == ".EXT") or Is_Branch_Op(t))
    {
	result = OP;
    }
    else if (t[0] == 'R')
    {
	if ((t == "R") or (t == "RE"))
	{
	    result = ERROR;
	}
	else
	{
	    result = REGISTER;
	}
    }
    else
    {
	if(t[0] == '.')
	{
	    result = ERROR;
	}
	else
	{
	    result = SYMBOL;
	}
    }
    return result;
}

int Buffer_Type(
    char ch
    )
{
    int result;
    if((Is_Alphabetic_Char(ch) and !(ch == 'x')) or ch == '.')
    {
	result = OP_OR_SYMBOL_OR_REGISTER_BS;
    }
    else if(ch == 'x' or ch == '#')
    {
	result = NUMBER_BS;
    }
    else if(ch == '=')
    {
        result = LITERAL_BS;
    }
    else if(ch == '\"')
    {
	result = STRING_BS;
    }
    else if(Is_Whitespace_Char(ch))
    {
	result = WHITE_SPACE_BS;
    }
    else if(ch == ',')
    {
	result = SEPARATOR_BS;
    }
    else if(ch == ';')
    {
	result = COMMENT_BS;
    }
    else if(ch == '\n')
    {
	result = NEWLINE_BS;
    }
    else
    {
	result = ERROR_BS;
    }
    return result;
}

int Token_Kind(
    int bs,
    string& str
    )
{
    int result;
    if((bs == EMPTY_BS) or (bs == ERROR_BS))
    {
	result = ERROR;
    }
    else if(bs == OP_OR_SYMBOL_OR_REGISTER_BS)
    {
	result = Op_Or_Symbol_Or_Register(str);
    }
    else if(bs == SEPARATOR_BS)
    {
	result = SEPARATOR;
    }
    else if(bs == WHITE_SPACE_BS)
    {
	result = WHITE_SPACE;
    }
    else if(bs == LITERAL_BS)
    {
	if(str.length()<3)
	{
	    result = ERROR;
	}
	else if(str.length() == 3 and (str[2] == '-' or str[2] == '+'))
	{
	    result = ERROR;
	}
	else
	{
	    result = LITERAL;
	}
	    
    }
    else if(bs == NUMBER_BS)
    {
	if(str[0] == 'x' and str.length() > 1)
	{
	    result = NUMBER;
	}
	else if(str[0] == '#' and str.length() > 1)
	{
	    if((str[1] == '-' or str[1] == '+') and str.length() == 2)
	    {
		result = ERROR;
	    }
	    else
	    {
		result = NUMBER;
	    }
	}
	else
	{
	    return ERROR;
	}
    }
    else if(bs == COMMENT_BS)
    {
	result = COMMENT;
    }
    else if(bs == NEWLINE_BS)
    {
	result = NEWLINE;
    }
    else if(bs == STRING_BS)
    {
	if(str.length() > 1 and str[str.length()-1] == '"')
	{
	    result = STRING;
	}
	else
	{
	    result = ERROR;
	}
    }
    return result;
}

void Initialize_Machine (
    Tokenizing_Machine& m
    )
{
    m.bs = EMPTY_BS;
    m.buffer = "";
    m.token_ready = false;
}

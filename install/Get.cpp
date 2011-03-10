#include "Get.h"

void Get_Next_Token(
    Tokenizing_Machine& m,
    ifstream& file,
    string& token_text,
    int& token_kind
    )
{
    
    while(!file.eof() and !Is_Ready_To_Dispense(m))
    {
	char ch;
	file.get(ch);
	
	Insert(m, ch);
    }
       
    if(m.token_ready)
    {
	Dispense(m, token_text, token_kind);
    }
    else
    {
	Flush_A_Token(m, token_text, token_kind);
    }
}

void Get_Next_Non_Whitespace_Token(
    Tokenizing_Machine& m,
    ifstream& file,
    string& token_text,
    int& token_kind
    )
{
    // get the next token on the line
    Get_Next_Token(m, file, token_text, token_kind);

    // while the token is a separator/whitespace/comment token get the
    // next token
    if( token_kind == WHITE_SPACE )
    {
	Get_Next_Token(m, file, token_text, token_kind);
    }

//     Display_Token_Kind(token_kind);
//     cout << "\t token_text = " << token_text << endl;
    
}

void Display_Token_Kind(
    int token_kind
    )
{
    cout << "token_kind = ";
    switch(token_kind)
    {
    case ERROR:
    {
	cout << "ERROR";
    }
    break;
    case OP:
    {
	cout << "OP\t";
    }
    break;
    case SYMBOL:
    {
	cout << "SYMBOL";
    }
    break;
    case REGISTER:
    {
	cout << "REGISTER";
    }
    break;
    case LITERAL:
    {
	cout << "LITERAL";
    }
    break;
    case NUMBER:
    {
	cout << "NUMBER";
    }
    break;
    case STRING:
    {
	cout << "STRING";
    }
    break;
    case SEPARATOR:
    {
	cout << "SEPARATOR";
    }
    break;
    case WHITE_SPACE:
    {
	cout << "WHITE_SPACE";
    }
    break;
    case COMMENT:
    {
	cout << "COMMENT";
    }
    break;
    case NEWLINE:
    {
	cout << "NEWLINE";
    }
    break;
    }
}

void Display_Buffer_State(
    Tokenizing_Machine& m
    )
{
    cout << "\nBuffer_State = ";
    switch(m.bs)
    {
    case EMPTY_BS:
    {
	cout << "EMPTY_BS";
    }
    break;
    case OP_OR_SYMBOL_OR_REGISTER_BS:
    {
	cout << "OP_OR_SYMBOL_OR_REGISTER_BS";
    }
    break;
    case NUMBER_BS:
    {
	cout << "NUMBER_BS";
    }
    break;
    case LITERAL_BS:
    {
	cout << "LITERAL_BS";
    }
    break;
    case STRING_BS:
    {
	cout << "STRING_BS";
    }
    break;
    case WHITE_SPACE_BS:
    {
	cout << "WHITESPACE_BS";
    }
    break;
    case SEPARATOR_BS:
    {
	cout << "SEPARATOR_BS";
    }
    break;
    case COMMENT_BS:
    {
	cout << "COMMENT_BS";
    }
    break;
    case NEWLINE_BS:
    {
	cout << "NEWLINE_BS";
    }
    break;
    case ERROR_BS:
    {
	cout << "ERROR_BS";
    }
    break;
    }
    cout << endl;
}

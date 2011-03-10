#include "Utils.h"

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
	  if('0' <= op[k] & op[k] <= '9')
	    {
	      integer = (long)(op[k] - 48);
	    }//fi
	  // convert uppercase hex letters
	  else if('A' <= toupper(op[k]) & 'F' >= toupper(op[k]))
	    {
	      integer = (long)(op[k] - 55);
	    }//esle
	  else
	    {
	      err="ERROR: #2\n\tString contains non-hex characters.";
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

bool Defined(Total& table, char value[], int counter)
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
	      string err ="ERROR: #1\n\tCharacter expected, newline received.";
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


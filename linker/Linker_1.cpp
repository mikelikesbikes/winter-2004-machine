#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>

using namespace std;

//Data Structures

struct Symbol {
  char name[7];
  unsigned short value;
};

struct Line_rec {
  char type;
  unsigned short address;
  unsigned short value;
  char symbol[7];
};

struct File_rec {	
  Line_rec line[518];
  unsigned short seg_len;
  unsigned short PLA;
};

struct Total{
  File_rec file[512];
  int file_num;
  int file_main;
  Symbol external[2560];
  int N_length;
  unsigned short prog_len;
  unsigned short IPLA;
  int file_start;
};



//functions

// bool Input_R
// bool Input_N
// bool Input_SLT
// bool Input_XY
// bool Input_Comment
// bool Input_E
// bool Input_H
// bool Input_Newline
// bool INput_Error


unsigned short REPLACE_L(string value, unsigned short offset);

unsigned short REPLACE_S(string value, unsigned short offset);

unsigned short REPLACE_X(string symbol, string value, unsigned short offset);

unsigned short REPLACE_X(string symbol, string value, unsigned short offset);

void Get(char arr[], int num, ifstream &input, int line, char file[]);

bool Convert_Hex(unsigned short& ret, char op[], int line, char file[]);

bool Defined(Total& table, char value[]);

bool linker(ifstream inny[], ofstream& outy, int length);

int main()
{  
  int length=3;
  bool hi;
  ifstream arr[7], input;
  ofstream outy("output");
  arr[0].open("jen.txt");
  arr[1].open("mary.txt");
  arr[2].open("nappier.txt");
  arr[3].open("you");
  arr[4].open("us");
  arr[5].open("what'sup");
  
  hi=linker( arr, outy, length);
}

bool linker(ifstream inny[], ofstream& outy, int length)
{
  
  
  
  //===========================================
  //initialization section begins
  //=========================================
  
  Total tot;
  
  tot.IPLA=0;
  tot.N_length=0;
  int i=0, count=0, chars=0, Nnum=0, j=1;
  unsigned short len;
  char ch, arr[5];
  string st, err;
  bool Ninsert, go, Rinsert, Einsert;
  tot.file_num=length;
  char sym_arr[7];
  
  
  //===========================================
  //initialization section ends
  //=========================================
  
  //===========================================
  //pass 1 begins
  //=========================================
  

  while(count<tot.file_num)
    {
   
      i=0;
      j=1;
      Ninsert=false;
      Rinsert=true;
      Einsert=true;
      Nnum=0;
      tot.file[count].seg_len=0;
      if(count==0)
	{
	  tot.file[count].PLA=tot.IPLA;
	  cout<<"PLA"<<tot.file[count].PLA<<endl;
	}
      else
	{
	  tot.file[count].PLA=tot.file[count-1].PLA + tot.file[count-1].line[0].value;
	  cout<<"PLA"<<tot.file[count].PLA<<endl;
	}
      while(inny[count].get(tot.file[count].line[i].type))
	{
	  //external symbols have to be declared first
	  if(tot.file[count].line[i].type!='N' & Ninsert==true)
	    {
	      Ninsert=false;
	    }
 try
  { 
	  switch(tot.file[count].line[i].type)
	    { 
	    case 'R':
	      Ninsert=true;
	      if(Rinsert==true)
		{
		  Rinsert=false;
		  Get(tot.file[count].line[i].symbol, 6, inny[count], j, tot.file[count].line[i].symbol);

		  //check to see if the file is main
		  //check to see if main is defined
		  //if it's not, define it, otherwise throw an error
		  if(strcmp(tot.file[count].line[i].symbol, "MAIN  ")==0)
		    {
		      if(!Defined(tot, tot.file[count].line[i].symbol))		     
			{
			  tot.file_main=count;
			   strcpy(tot.external[tot.N_length].name, tot.file[count].line[i].symbol);
			   tot.N_length++;
			  cout<<"found main"<<endl;
			}//end if
		      else//throw error: main already defined
			{
			  getline(inny[count], st);
			 err="ERROR:#3\n\tSecond declaration of main";
			 throw err;
			}//end else
		    }//end if
		  
		  //get address	
		  Get(arr, 4, inny[count], j, tot.file[count].line[i].symbol);
		  go=Convert_Hex(tot.file[count].line[i].address, arr, j, tot.file[count].line[i].symbol);
		  if(tot.file[count].line[i].address!=0)
		    {
		      err="ERROR: #4\n\trelocatable program start address must be zero";
		  getline(inny[count], st);
		      throw err;
		    }//end if
		  
		  //get segment length					
		  
		  Get(arr, 4, inny[count], j, tot.file[count].line[i].symbol);
		  go=Convert_Hex(tot.file[count].line[i].value ,arr, j, tot.file[count].line[i].symbol);
		  //check to see that program fits on 1 page	
		  if(tot.file[count].line[i].value>512)
		    {
		      err="ERROR: #5\n\tprogram length greater than one page";
		  getline(inny[count], st);
		      throw err;
		    }//end if
		  getline(inny[count], st);
		  cout<<tot.file[count].line[i].type<<tot.file[count].line[i].symbol;
		  cout<<hex<<tot.file[count].line[i].value<<"i is"<<i<<endl;
		}
	      else
		{
		  i--;
		  getline(inny[count], st);
		  err="ERROR: #6\n\tmultiple header records";
		  throw err;
		}//end else
	      break;
	    case 'N':
	      //clear the symbol array
	      memset(sym_arr, '\0', 7);
	      chars=0;
	      
	      //check for maximum allowed symbols<5
	      //check EXT at beginning
	      if(Nnum<5 & Ninsert==true)
		{
		  do
		    {
		      inny[count].get(ch);
		      if(ch!='=')
			{
			  sym_arr[chars]=ch;
			  
			}//end if
		      chars++;
		    }while(ch != '=');
		  Get(arr, 4, inny[count], j, tot.file[count].line[i].symbol);
		  getline(inny[count], st);
		  if(!Defined(tot, sym_arr))
		    {
		      strcpy(tot.external[tot.N_length].name, sym_arr);
		      go=Convert_Hex(tot.external[tot.N_length].value , arr, j, tot.file[count].line[i].symbol);
		      tot.N_length++;
		      Nnum++;
		      cout<<tot.file[count].line[i].type<<tot.external[tot.N_length-1].name<<hex<<tot.external[tot.N_length-1].value<<endl;
		    }
		  else
		    {
		      err="ERROR: #7\n\tduplicate external symbols defined";
		      throw err;
		    }
		  
		}//end if
	      else//max symbols<5(header + 4 ENTs) throw error or EXT not at beginning
		{	
		  if(Ninsert==true)
		    {
		      err="ERROR: #8\n\tGreater than 4 external symbols";
		      throw err;
		    }
		else
		  {
		    err="ERROR: #9\n\texternal symbols must be declared prior to program start";
		    throw err;
		  }
		}//end else
	      break;
	    case 'S':
	    case 'L':
	    case 'T':
	      Get(arr, 4, inny[count], j, tot.file[count].line[i].symbol);
	      go=Convert_Hex(tot.file[count].line[i].address, arr, j, tot.file[count].line[i].symbol);		
	      Get(arr, 4, inny[count], j, tot.file[count].line[i].symbol);
	      go=Convert_Hex(tot.file[count].line[i].value, arr, j, tot.file[count].line[i].symbol);			
	      getline(inny[count], st);
	      cout<<tot.file[count].line[i].type<<hex<<tot.file[count].line[i].address;
	      cout<<hex<<tot.file[count].line[i].value<<endl;
	      tot.file[count].seg_len++;
	      break;
	    case 'X':
	    case 'Y':
	      Get(arr, 4, inny[count], j, tot.file[count].line[i].symbol);
	      go=Convert_Hex(tot.file[count].line[i].address, arr, j, tot.file[count].line[i].symbol);
	      Get(arr, 4, inny[count], j, tot.file[count].line[i].symbol);
	      go=Convert_Hex(tot.file[count].line[i].value, arr, j, tot.file[count].line[i].symbol);
	      chars=0;	
	      do
		{
		  inny[count].get(ch);
		  if(ch!=' ' & ch!='\n' & ch!='\t')
		    {
		      tot.file[count].line[i].symbol[chars]=ch;
		    }//end if
		  chars++;
		}while(ch!=' ' & ch!='\n' & ch!='\t');
	      if(ch!='\n')
		{
		  getline(inny[count], st);
		}
	      //throw error if chars>=6
	      if(chars>7)
		{
		  err="ERROR: #10\n\texternal symbols can only be 6 characters long";
		  throw err;
		}
	      if(!Defined(tot, tot.file[count].line[i].symbol))
		{
		  err="ERROR: #15\n\texternal symbol undefined";
		  throw err;
		}
	      cout<<tot.file[count].line[i].type<<hex<<tot.file[count].line[i].address;
	      cout<<hex<<tot.file[count].line[i].value<<tot.file[count].line[i].symbol<<endl;
	      tot.file[count].seg_len++;
	      break;
	      
	    case ';':
	      getline(inny[count], st);
	      cout<<"comment"<<endl;
	      i--;			
	      break;	
	    case 'E':
	      if(Einsert==true)
		{
		  Einsert=false;
		  Get(arr, 4, inny[count], j, tot.file[count].line[i].symbol);
		  getline(inny[count], st);
		  go=Convert_Hex(tot.file[count].line[i].address, arr, j, tot.file[count].line[i].symbol);
		  if(count==tot.file_main)
		  {
		    tot.file_start=tot.file[count].line[i].address;
  }
		  cout<<tot.file[count].line[i].type<<hex<<tot.file[count].line[i].address<<endl;
		}//end if Einsert==true
	      else
		{
		  err="ERROR: #11\n\tEND record already declared";
		  throw err;
		}
	      break;
	    case 'H':
	      getline(inny[count], st);
	      Rinsert=false;
	      Ninsert=true;
	      err="ERROR: #12\n\tprogram is not relocatable";
	      throw err;
	 
	      break;
	    case '\n':
	      cout<<"newline"<<endl;
	      i--;
	      break;
	    case '\t':
	    case ' ': 
	       getline(inny[count], st);
	       i--;
	       break;
	    default:
	      getline(inny[count], st);
	      i--;
	      err="ERROR: #13\n\tinvalid character at line start";
	      throw err;

	      break;
	      
	      
	      
	      
	      cout<<j<<endl;
	    }//end switch
  }
 catch(string e)
  { 
    
    cerr<<e<<endl<<"\tFILE: "<<tot.file[count].line[0].symbol<<"\tLINE: "<<j<<endl;
  }
 
   	  
 i++;
 j++;
	}//end while
      try
	{
	  if(tot.file[count].seg_len!=tot.file[count].line[0].value)
	{
	  err="ERROR: #14\n\tthe segment length given in the header\n\tand the actual segment length do not match";;
	  throw err;
	}
	}
      catch(string e)
	{
	  cerr<<e<<endl<<"\tFILE: "<<tot.file[count].line[0].symbol<<tot.file[count].seg_len<<endl;
	}
      count++;
      
    }//end while

  //============================================================
  //pass 1 ends
  //============================================================
  count=0;
  tot.prog_len=0;
  while(count<tot.file_num)
    {
      cout<<tot.file[count].PLA<<endl;;
      tot.prog_len=tot.prog_len+tot.file[count].PLA;
      count++;
    }
  tot.prog_len=tot.prog_len+tot.file[tot.file_num].line[0].value;
  try
    {
      if(tot.prog_len>512)
	{
	  err="ERROR: #16\n\t linked programs will not all fit on one page";
	  throw err;
	}
    }
  catch(string e)
    {
      cerr<<e<<endl<<"\ttotal length is: "<<tot.prog_len<<endl;;
    }
  //============================================================
  //pass 2 begins
  //============================================================ 
  count=0;
  while(count<tot.file_num)
    {
      i=0;
      do
	{
	  switch(tot.file[count].line[i].type)
	    {	  
	    case 'R':
	      if(count==0)
		{
		  outy<<"R"<<tot.file[0].line[0].symbol<<"0000"<<setw(4)<<setfill('0')<<hex<<uppercase<<tot.prog_len<<endl;
		}
	      break;
	    case 'N':
	      break;
	    case 'S':
	      outy<<"S"<<endl;
	      break;
	    case 'L':
	      outy<<"L"<<endl;
	      break;
	    case 'X':
	      outy<<"S"<<endl;
	      break;
	    case 'Y':
	      outy<<"L"<<endl;
	      break;
	    case 'T':
	      outy<<"T"<<setw(4)<<setfill('0')<<hex<<uppercase<<tot.file[count].line[i].address+tot.file[count].PLA<<setw(4)<<setfill('0')<<hex<<uppercase<<tot.file[count].line[i].value<<endl;
	      break;
	    case 'E':
	      if(count==tot.file_num-1)
		{
		  outy<<"E"<<setw(4)<<setfill('0')<<hex<<uppercase<<tot.file[tot.file_main].line[tot.file[tot.file_main].line[0].value-1].address+tot.file[tot.file_main].PLA;
		}
	      break;
	    }//end switch
	  i++;
	}while(tot.file[count].line[i].type!='E');
	count++;
    }//end while
outy<<"E"<<setw(4)<<setfill('0')<<hex<<uppercase<<tot.file_start+tot.file[tot.file_main].PLA;





  //============================================================
  //pass 2 ends
  //============================================================



}//end program


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
	    }
	  // convert uppercase hex letters
	  else if('A' <= toupper(op[k]) & 'F' >= toupper(op[k]))
	    {
	      integer = (long)(op[k] - 55);
	    }
	  else
	    {
	      err="ERROR: #2\n\tstring contains non-hex characters";
	      throw err;
	    }
	  //add all the converted characters up
	  sum=sum*16 + integer;
	}//end for
      
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

bool Defined(Total& table, char value[])
{
  for(int k=0; k<table.N_length;k++)// loop through all defined external symbols
    {
      if (strcmp(table.external[k].name, value)==0)// if external symbol is found
	{
	  return true;
	}
    }
  return false;// specified symbol not found
}

void Get(char arr[], int num, ifstream &input, int line, char file[])
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
	    }
	  else
	    {
	      input.unget();
	      string err ="ERROR: #1\n\tCharacter expected, newline received";
	      throw err;
	      
	    }
	  
	  counter++;
	}while(counter<num & ch!='\n');	
    }
  catch(string e)
    {
      cerr<<e<<endl<<"\tFILE: "<<file<<"\tLINE: "<<line<<endl;
    }
}

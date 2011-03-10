#include"Linker.h"

bool linker(ifstream inn[], ofstream& out, int length)
{
  //===========================================
  //initialization section begins
  //=========================================
  
  Total tot;
  
  tot.IPLA=0;
  tot.N_length=0;
  int i=0, count=0, chars=0, Nnum=0, j=1, errors=0, warnings=0, k;
  unsigned short seg_len;
  char ch, arr[5];
  string st, err;
  bool Ninsert, go=true, Rinsert, Einsert;
  tot.file_num=length;
  char sym_arr[7];
    
  //===========================================
  //initialization section ends
  //=========================================
  
  //===========================================
  //pass 1 begins
  //=========================================
  while(count<tot.file_num & errors<20 & go==true)
    {
      i=0;
      j=1;
      Ninsert=false;
      Rinsert=true;
      Einsert=true;
      Nnum=0;
      tot.file[count].seg_len=0;
      
      //==================================================================
      // set the IPLA & PLAs if the linked programs were loaded at 0x0000
      //==================================================================
      if(count==0)
	{
	  tot.file[count].PLA=tot.IPLA;
	}//fi
      else
	{
	  tot.file[count].PLA=tot.file[count-1].PLA + tot.file[count-1].line[0].value;
	}//esle
      
      
      //input each file
      while(inn[count].get(tot.file[count].line[i].type) )
	{
	  //external symbols have to be declared first
	  if(tot.file[count].line[i].type!='N' & Ninsert==true)
	    {
	      Ninsert=false;
	    }//fi
	  try
	    { 
	      switch(tot.file[count].line[i].type)
		{ 
		  //=================================================================
		  //HEADER
		  //=================================================================
		case 'R':
		  Ninsert=true; //entry points must be declared directly below the header
		  if(Rinsert==true)//there can only be 1 header
		    {
		      Rinsert=false;
		      if(!Get(tot.file[count].line[i].symbol, 6, inn[count], j, tot.file[count].line[0].symbol))
			{
			  errors++;
			}//fi

		      //check to see if the segment name is defined
		      //if it's not, define it, otherwise throw an error
		     
		      if(!Defined(tot, tot.file[count].line[i].symbol, k))	
			{
			  //define the file as main if it is MAIN
			  if(strcmp(tot.file[count].line[i].symbol, "MAIN  ")==0)
			    {
			      tot.file_main=count;
			    }//fi	     
			  tot.external[tot.N_length].name=tot.file[count].line[0].symbol;
			  tot.external[tot.N_length].value=0;
			  tot.N_length++;
			}//fi
		      else//throw error: main already defined
			{
			  getline(inn[count], st);
			  errors++;
			  err="ERROR:#3\n\tSecond segment name declaration.  This segment will not be used.";
			  throw err;
			}//esle
		      
		      //get address	
		      if(!Get(arr, 4, inn[count], j, tot.file[count].line[0].symbol))
			{
			  errors++;
			}//fi
		      if(!Convert_Hex(tot.file[count].line[i].address, arr, j, tot.file[count].line[0].symbol))
			{
			  errors++;
			}//fi
		      //address of a relocatable program must be zero
		      if(tot.file[count].line[i].address!=0)
			{
			  errors++;
			  err="ERROR: #4\n\tRelocatable program start address must be zero.";
			  getline(inn[count], st);
			  throw err;
			}//fi
		      
		      //get segment length					
		      
		      if(!Get(arr, 4, inn[count], j, tot.file[count].line[0].symbol))
			{
			  errors++;
			}//fi
		      if(!Convert_Hex(tot.file[count].line[i].value ,arr, j, tot.file[count].line[0].symbol))
			{
			  errors++;
			}//fi
		      //seg_len is the total length of the Text records per program
		      seg_len=tot.file[count].line[i].value-1;

		      //check to see that program fits on 1 page	
		      if(tot.file[count].line[i].value>512)
			{
			  errors++;
			  go= false;
			  err="ERROR: #5\n\tProgram length greater than one page.";
			  getline(inn[count], st);
			  throw err;
			}//fi
		      
		      //get the rest of the line and throw it away
		      getline(inn[count], st);
		      //  cout<<tot.file[count].line[i].type<<tot.file[count].line[i].symbol;
		      // cout<<hex<<tot.file[count].line[i].value<<"i is"<<i<<endl;
		    }//fi
		  //a relocatable header record has already been found
		  else
		    {
		      i--;
		      getline(inn[count], st);
		      errors++;
		      err="ERROR: #6\n\tMultiple header records.  This line will be skipped.";
		      throw err;
		    }//esle
		  break;

		case 'N':
		  //========================================================================
		  //ENTRY POINTS
		  //========================================================================

		  //clear the symbol array, reset chars
		  memset(sym_arr, '\0', 7);
		  chars=0;
		  
		  //check for maximum allowed symbols<5
		  //check EXT at beginning of segment
		  //get the symbol name
		  if(Nnum<5 & Ninsert==true)
		    {
		      do
			{
			  inn[count].get(ch);
			  if(ch!='=')
			    {
			      sym_arr[chars]=ch;
			      
			    }//fi
			  chars++;
			}while(ch != '=');//elihw

		      //get the symbol value
		      if(!Get(arr, 4, inn[count], j, tot.file[count].line[0].symbol))
			{
			  errors++;
			}//fi
		      //get the rest of the line and throw it away
		      getline(inn[count], st);
		      //if the symbol isn't in the table, add it, and define the value
		      if(!Defined(tot, sym_arr, k))
			{
			  strcpy(tot.external[tot.N_length].name, sym_arr);
			  if(!Convert_Hex(tot.external[tot.N_length].value , arr, j, tot.file[count].line[i].symbol))
			    {
			      errors++;
			    }//fi
			  //external symbol value must be within the footprint
			  if(tot.external[tot.N_length].value>seg_len)
			    {
			      errors++;
			      err="ERROR: #18\n\tEntry point value not between [segment load address] and [segment load address + segment length].";
			      throw err;
			    }//fi
			  //increment the total number of symbols
			  tot.N_length++;
			  //increment the number of symbols in this program
			  Nnum++;
			  //	  cout<<tot.file[count].line[i].type<<tot.external[tot.N_length-1].name<<hex<<tot.external[tot.N_length-1].value<<endl;
			}//fi
		      //the symbol is already in the table
		      else
			{
			  errors++;
			  err="ERROR: #7\n\tDuplicate entry points defined.  Value of first entry point taken.";
			  throw err;
			}//esle
		      
		    }//fi

		  else//max symbols<5(header + 4 ENTs) throw error or EXT not at beginning
		    {
		      //max symbols<5	
		      if(Ninsert==true)
			{
			  errors++;
			  err="ERROR: #8\n\tGreater than 4 entry points. Entry point will not used. ";
			  throw err;
			}//fi
		      //EXT have to be at the beginning of the program
		      else
			{
			  errors++;
			  err="ERROR: #9\n\tEntry points must be declared prior to all text records.";
			  throw err;
			}//esle
		    }//esle
		  break;

		case 'S':
		case 'L':
		case 'T':
		  //========================================================================
		  //SHORT/LONG relocatables & TEXT
		  //========================================================================
		  
		  //get the address
		  if(!Get(arr, 4, inn[count], j, tot.file[count].line[0].symbol))
		    {
		      errors++;
		    }//fi
		  if(!Convert_Hex(tot.file[count].line[i].address, arr, j, tot.file[count].line[0].symbol))
		    {
		      errors++;
		    }//fi
		  //make sure address is within footprint
		  if(tot.file[count].line[i].address>seg_len)
		    {
		      errors++;
		      err="ERROR: #19\n\tAddress not between [segment load address] and [segment load address + segment length].";
		      getline(inn[count], st);
		      throw err;
		    }//fi
		  //get the value
		  if(!Get(arr, 4, inn[count], j, tot.file[count].line[0].symbol))
		    {
		      errors++;
		    }//fi
		  if(!Convert_Hex(tot.file[count].line[i].value, arr, j, tot.file[count].line[0].symbol))
		    {
		      errors++;
		    }//fi
		  //get the rest of the line and throw it away
		  getline(inn[count], st);

		  //increment the segment length 
		  tot.file[count].seg_len++;
		  break;

		case 'X':
		case 'Y':
		  //========================================================================
		  //SHORT(X)/LONG(Y) relocatables with external symbols
		  //========================================================================
		 
		  //get the address
		  if(!Get(arr, 4, inn[count], j, tot.file[count].line[0].symbol))
		    {
		      errors++;
		    }//fi
		  if(!Convert_Hex(tot.file[count].line[i].address, arr, j, tot.file[count].line[0].symbol))
		    {
		      errors++;
		    }//fi
		  //address must be within the footprint
		  if(tot.file[count].line[i].address>seg_len)
		    {
		      errors++;
		      err="ERROR: #19\n\tAddress not between [segment load address] and [segment load address + segment length].";
		      getline(inn[count], st);
		      throw err;
		    }//fi
		  //get the value
		  if(!Get(arr, 4, inn[count], j, tot.file[count].line[0].symbol))
		    {
		      errors++;
		    }
		  if(!Convert_Hex(tot.file[count].line[i].value, arr, j, tot.file[count].line[0].symbol))
		    {
		      errors++;
		    }
		  //get the symbol
		  chars=0;	
		  do
		    {
		      inn[count].get(ch);
		      if(ch!=' ' & ch!='\n' & ch!='\t')
			{
			  tot.file[count].line[i].symbol[chars]=ch;
			  chars++;
			}//fi
		   
		    }while(ch!=' ' & ch!='\n' & ch!='\t');
		  if(ch!='\n')
		    {
		      getline(inn[count], st);
		    }

		  //throw error if chars>=6, since index starts at 0
		  if(chars>6)
		    {
		      errors++;
		      err="ERROR: #10\n\tExternal symbol '"+(string)tot.file[count].line[i].symbol+"' greater than 6 characters long.";
		      throw err;
		    }//fi

		  tot.file[count].seg_len++;
		  break;
		  
		case ';':
		  //========================================================================
		  //COMMENT
		  //========================================================================
		  getline(inn[count], st);
		  i--;			
		  break;	
		case 'E':
		  //========================================================================
		  //END
		  //========================================================================
		  if(Einsert==true)
		    {
		      Einsert=false;
		      Get(arr, 4, inn[count], j, tot.file[count].line[0].symbol);
		      getline(inn[count], st);
		      if(!Convert_Hex(tot.file[count].line[i].address, arr, j, tot.file[count].line[0].symbol))
			{
			  errors++;
			}//fi
		      if(tot.file[count].line[i].address>seg_len)
			{
			  errors++;
			  err="ERROR: #20\n\tStart address not between [segment load address] and [segment load address + segment length].";
			  throw err;
			}//fi

		      if(count==tot.file_main)
			{
			  tot.file_start=tot.file[count].line[i].address;
			}//fi
		    }//fi
		  else
		    {
		      errors++;
		      err="ERROR: #11\n\tEND record already declared.";
		      throw err;
		    }
		  break;
		case 'H':
		  //========================================================================
		  //invalid header
		  //========================================================================
		  getline(inn[count], st);
		  Rinsert=false;
		  Ninsert=true;
		  errors++;
		  go=false;
		  err="ERROR: #12\n\tProgram is not relocatable.";
		  throw err;
		  
		  break;
		  //========================================================================
		  //NEWLINE/TAB/SPACE
		  //========================================================================
		case '\n':
		  i--;
		  break;
		case '\t':
		case ' ': 
		  getline(inn[count], st);
		  i--;
		  break;
		default:
		  //========================================================================
		  //INVALID INPUT
		  //========================================================================
		  getline(inn[count], st);
		  i--;
		  errors++;
		  err="ERROR: #13\n\tInvalid character at line start.";
		  throw err;
		  
		  break;
		}//hctiws
	    }
	  catch(string e)
	    { 
	      cerr<<e<<endl<<"\tFILE: "<<tot.file[count].line[0].symbol<<"\tLINE: "<<j<<endl;
	    }
	  
   	  
	  i++;//i is the index of the array
	  j++;//j is the actual line counter
	}//elihw
      try
	{
	  if(tot.file[count].seg_len>tot.file[count].line[0].value)
	    {
	      errors++;
	      err="ERROR: #14\n\tText records do not fit in the footprint.";;
	      throw err;
	      
	    }
	}//
      catch(string e)
	{
	  cerr<<e<<endl<<"\tFILE: "<<tot.file[count].line[0].symbol<<endl;
	}
      count++;
      
    }//elihw

  //=====================================================================================
  //pass 1 ends
  //=====================================================================================
  
  //main has to be defined
  try
    {
      if(!Defined(tot, "MAIN  ", k))
	{
	  errors++;
	  err="ERROR: #17\n\tMAIN undefined.";
	  throw err;
	}
    }
  catch(string e)
    {
      cerr<<e<<endl;
    }

  count=0;
  tot.prog_len=0;
  //calculate the total program length
  while(count<tot.file_num)
    {
      cout<<tot.file[count].PLA<<endl;;
      tot.prog_len=tot.prog_len+tot.file[count].line[0].value;
      count++;
    }

  //program length cannot be greater than 512
  try
    {
      if(tot.prog_len>512)
	{
	  errors++;
	  err="ERROR: #16\n\tLinked programs do not fit on one page.";
	  throw err;
	}
    }
  catch(string e)
    {
      cerr<<e<<endl<<"\ttotal length is: "<<tot.prog_len<<endl;
    }

  //don't continue if there are errors
  if(errors>0)
    {
      cout<<"..."<<dec<<errors<<" Errors in input files... "<<endl<<"...Output file will not be generated..."<<endl<<endl;
      go=false;
    }
  //prompt for the relocation address
  else
    {
      cout<<"...No errors in input files..."<<endl<<endl;
      go=false;
      while(go==false)
	{
	  cout<<"Please enter the relocation start address"<<endl<<"valid range: [0000, "
	      <<setw(4)<<hex<<uppercase<<setfill('0')<<0x01FF-tot.prog_len+0x0001<<"]"<<endl;  
	  cin>>hex>>tot.IPLA;
	  if(tot.IPLA<=(0x01ff-tot.prog_len+1))
	    {
	      go=true;
	    }//fi
	  else    
	    {
	      cout<<"you entered an invald address: "<<hex<<tot.IPLA<<endl;
	    }//esle
	}//elihw
    }//esle
  //============================================================
  //pass 2 begins
  //============================================================ 
  count=0;
  errors=0;
  while(count<tot.file_num & go==true)
    {
      i=0;
      do
	{
	  try
	    {
	      switch(tot.file[count].line[i].type)
		{	  
		case 'R':
		  if(count==0)
		    {
		      out<<"HMAIN  "<<setw(4)<<uppercase<<hex<<setfill('0')<<tot.IPLA<<setw(4)<<setfill('0')<<hex<<uppercase<<tot.prog_len<<endl;
		    }//fi
		  break;
		case 'N':
		  break;
		case 'S':
		  out<<"T"<<setw(4)<<setfill('0')<<hex<<uppercase<<tot.file[count].line[i].address+tot.file[count].PLA+tot.IPLA;
		  out<<setw(4)<<setfill('0')<<hex<<uppercase<<(tot.file[count].line[i].value & 0xFE00) + tot.IPLA+ tot.file[count].PLA<<endl;
		  //bits 0-8 should be zero
		  if(tot.file[count].line[i].value & 0x01FF!=0)
		    {
		      warnings++;
		      string err="WARNING: #1\n\tBits 0-8 of short relocatable record not zero.\n\tThey will be set to zero before relocation.";
		      throw err;
		    }//fi
		  break;
		case 'L':
		  out<<"T"<<setw(4)<<setfill('0')<<hex<<uppercase<<tot.file[count].line[i].address+tot.file[count].PLA+tot.IPLA
		     <<setw(4)<<setfill('0')<<hex<<uppercase<<tot.IPLA+tot.file[count].PLA<<endl;
		  //bits 0-15 should be zero
		  if(tot.file[count].line[i].value!=0)
		    {
		      warnings++;
		      string err="WARNING: #2\n\tBits 0-15 of long relocatable record not zero.\n\tThey will be set to zero before relocation.";
		      throw err;
		    }//fi
		  break;
		case 'X':
		  out<<"T"<<setw(4)<<setfill('0')<<hex<<uppercase<<tot.file[count].line[i].address+tot.file[count].PLA+tot.IPLA;
		  //output the relocated value if the external symbol is defined
		  if(!Defined(tot, tot.file[count].line[i].symbol, k))
		    {
		      errors++;
		      out<<setw(4)<<setfill('0')<<hex<<uppercase
			 <<tot.external[k].value+tot.file[count].PLA+tot.IPLA+(tot.file[count].line[i].value & 0xFE00)<<endl;
		      err= "ERROR: #15\n\tExternal symbol '" + (string)tot.file[count].line[i].symbol +"' undefined.";
		      throw err;
		    }//fi
		  else
		    {
		      out<<setw(4)<<setfill('0')<<hex<<uppercase
			     <<tot.external[k].value+tot.file[count].PLA+tot.IPLA+(tot.file[count].line[i].value & 0xFE00)<<endl;	       	
		    }//esle
		  //bits 0-8 should be zero
		  if(tot.file[count].line[i].value & 0x01FF!=0)
		    {
		      warnings++;
		      err="WARNING: #1\n\tBits 0-8 of short relocatable record not zero.\n\tThey will be set to zero before relocation.";
		      throw err;
		    }//fi
		  break;
		case 'Y':
		  out<<"T"<<setw(4)<<setfill('0')<<hex<<uppercase<<tot.file[count].line[i].address+tot.file[count].PLA+tot.IPLA;
		  //output the relocated value if the external symbol is defined
		  if(!Defined(tot, tot.file[count].line[i].symbol, k))
		    {
		      errors++;
		      out<<setw(4)<<setfill('0')<<hex<<uppercase<<tot.IPLA+tot.file[count].PLA+tot.external[k].value<<endl;
		      err="ERROR: #15\n\tExternal symbol '" + (string)tot.file[count].line[i].symbol +"' undefined.";
		      throw err;
		    }//fi
		  else
		    {
		      out<<setw(4)<<setfill('0')<<hex<<uppercase<<tot.IPLA+tot.file[count].PLA+tot.external[k].value<<endl;	
		    }//esle
		  //bits 0-15 should be zero
		  if(tot.file[count].line[i].value!=0)
		    {
		      warnings++;
		      err= "WARNING: #2\n\tBits 0-15 of long relocatable record not zero.\n\tThey will be set to zero before relocation.";
		      throw err;
		    }//fi
		  
		  break;
		case 'T':
		  out<<"T"<<setw(4)<<setfill('0')<<hex<<uppercase<<tot.file[count].line[i].address+tot.file[count].PLA+tot.IPLA
		     <<setw(4)<<setfill('0')<<hex<<uppercase<<tot.file[count].line[i].value<<endl;
		  break;
		case 'E':
		default:	
		  break;
		}//hctiws
	    }//yrt
	  catch(string e)
	    {
	      cerr<<e<<endl<<"\tFILE: "<<tot.file[count].line[0].symbol<<"\tLINE: "<<i<<endl;
	    }
	  i++;
	}while(tot.file[count].line[i].type!='E');
      count++;
    }//elihw
  
  //output the END record
  (void)Defined(tot, tot.file[count].line[i].symbol, k);
  out<<"E"<<setw(4)<<setfill('0')<<hex<<uppercase<<tot.external[k].value+tot.file[tot.file_main].PLA+tot.IPLA;
  
  //output whether the file is correct or not
  if(errors>0)
    {
      cout<<"..."<<dec<<errors<<" Error(s) in generation of output files... "<<endl
	  <<"...Output file may not be correct..."<<endl<<endl;
    }//fi
  else if(go==true)
    {
      cout<<endl<<"...File created successfully..."<<endl<<endl;
    }//fi esle
  //============================================================
  //pass 2 ends
  //============================================================
}//margorp





//===========================================================
//===========================================================
//Program: Linker.cpp
//Author: Jennifer Nappier
//Description:  Links and relocates one or more programs.
//===========================================================
//===========================================================
#include"Linker.h"



bool Linker(ifstream inn[], ofstream& out, int length)
{
  
  
  //===========================================
  //initialization section begins
  //=========================================
  
  Total tot;
  
  tot.IPLA=0;
  tot.N_length=0;
  int i=0, count=0, chars=0, Nnum=0, j=1, errors=0, warnings=0, k, n;
  long address;
  unsigned short  header_length[513];
  char ch, arr[5], name[7];
  string st, err;
  bool Ninsert, go=true, Rinsert, Einsert;
  tot.file_num=length;
  char sym_arr[7], upper_arr[7];
  
  	      char dummy[7];
		      int z=0;
  //=========================================
  //initialization section ends
  //=========================================
  
  //=========================================
  //pass 1 begins
  //=========================================
  

  while(count<tot.file_num and errors<20 and go==true)
    {
      
      i=0;
      j=1;
      Ninsert=false;
      Rinsert=true;
      Einsert=true;
      Nnum=0;
      tot.file[count].seg_len=0;
        
      //==============================================
      // set the IPLA & PLAs as if the linked programs
      // were loaded at 0x0000
      //==============================================
      
      if(count==0)
	{
	  tot.file[count].PLA=tot.IPLA;
	}//fi
      else
	{
	  tot.file[count].PLA=tot.file[count-1].PLA + header_length[count-1];
	}//esle
      //input each file
      while(inn[count].get(tot.file[count].line[i].type) and
	    (errors<20) and (go==true)) 
	{
	  //external symbols have to be declared first
	  if(tot.file[count].line[i].type!='N' and Ninsert==true)
	    {
	      Ninsert=false;
	    }//fi
	  try
	    { 
	      switch(tot.file[count].line[i].type)
		{ 
		  //=============================================
		  //HEADER
		  //=============================================
		
		case 'R':

		  Ninsert=true; //entry points must be declared
		                //directly below the header
		  if(Rinsert==true)//there can only be 1 header
		    {
		      Rinsert=false;
		      if(!Get(tot.file[count].line[i].symbol, 6, inn[count],
			      j, tot.file[count].line[i].symbol))
			{
			  errors++;
			}//fi

		      //get rid of spaces in program name, if they exist
		      z=0;
		      //clear the symbol array, reset chars
		      memset(dummy, '\0', 7);
		      while(z<(strlen(tot.file[count].line[i].symbol)-1) and tot.file[count].line[i].symbol[z]!=' ')
			{
			  dummy[z]=tot.file[count].line[i].symbol[z];
			  z++;
			}
		      strcpy(tot.file[count].line[i].symbol, dummy);
		      name=tot.file[count].line[i].symbol;

		      //check to see if the segment name is defined
		      //if it's not, define it, otherwise throw an error
		      //define the file as main if it is MAIN
		      //Uppercase(tot.file[count].line[i].symbol,
		      //upper_arr);

		      for(n=0;n<6;n++)
			{
			  upper_arr[n]=toupper(tot.file[count].line[i].symbol[n]);
			}
		      if(strcmp(upper_arr, "MAIN")==0)
			{ 
			  if(!Defined(tot, "MAIN", k))	
			    {
			      
			      tot.external[tot.N_length].name=upper_arr;
			      tot.external[tot.N_length].value=0+tot.file[count].PLA;
			      tot.N_length++;
			      tot.file_main=count;
			    }//fi
			  else//throw error: main already defined
			    {
			      getline(inn[count], st);
			      errors++;
			      err="link ERROR:#21\n\tSecond declaration of main.  "
				  "It will not be used as an entry point.";
			      throw err;
			    }//esle
			}//fi
		      else //segment name is not "MAIN"
			{
			  if(!Defined(tot, tot.file[count].line[i].symbol, k))	
			    {			      
			      tot.external[tot.N_length].name=tot.file[count].line[i].symbol;
			      tot.external[tot.N_length].value=0+tot.file[count].PLA;
			      tot.N_length++;
			    }//fi
			  else//throw error: segment name  already defined
			    {
			      getline(inn[count], st);
			      errors++;
			      err="link ERROR:#3\n\tSecond segment name declaration.  "
				   "This segment will not be used as an entry point.";
			      throw err;
			    }//esle
			}//esle
		      
		      //get address	
		      if(!Get(arr, 4, inn[count], j,name))
			{
			  errors++;
			}//fi
		      if(!Convert_Hex(tot.file[count].line[i].address, arr, j, name))
			{
			  errors++;
			}//fi
		      //address of a relocatable program must be zero
		      if(tot.file[count].line[i].address!=0)
			{
			  errors++;
			  err="link ERROR: #4\n\tRelocatable program load address must be zero.";
			  getline(inn[count], st);
			  throw err;
			}//fi
		      
		      //get segment length					
		      
		      if(!Get(arr, 4, inn[count], j, tot.file[count].line[i].symbol))
			{
			  errors++;
			}//fi
		      if(!Convert_Hex(tot.file[count].line[i].value ,arr, j,
				      tot.file[count].line[i].symbol))
			{
			  errors++;
			}//fi
		      //seg_len is the total length of the Text
		      //records per program

		      header_length[count]=tot.file[count].line[i].value;
		      //check to see that program fits on 1 page	
		      if(tot.file[count].line[i].value>0x0200)
			{
			  errors++;
			  go= false;
			  err="link ERROR: #5\n\tProgram length greater than one page.";
			  getline(inn[count], st);
			  throw err;
			}//fi
		      
		      //get the rest of the line and throw it away
		      getline(inn[count], st);
		    }//fi
		  //a relocatable header record has already been found
		  else
		    {
		      getline(inn[count], st);
		      errors++;
		      err="link ERROR: #6\n\tMultiple header records.  This line will be skipped.";
		      throw err;
		    }//esle
		  break;

		case 'N':
		  //=================================================
		  //ENTRY POINTS
		  //=================================================
		    
		  if(Rinsert==true)
		    {
		      go=false;
		      errors++;
		      err="link ERROR: #24\n\tHeader record undefined.";
		      throw err;
		    }//fi
		  //clear the symbol array, reset chars
		  memset(sym_arr, '\0', 7);
		  chars=0;
		  
		  //check for maximum allowed symbols<4
		  //check EXT at beginning of segment
		  //get the symbol name
		  if(Nnum<4 and Ninsert==true)
		    {
		      do
			{
			  inn[count].get(ch);
			  if(ch!='=')
			    {
			      sym_arr[chars]=ch;
			      chars++;
			      
			    }//fi
			  if(chars>6)
			    {
			      errors++;
			      getline(inn[count], st);
			      err="link ERROR: #26\n\tEntry point symbol length greater than 6.";
			      throw err;
			    }
			}while(ch != '=');//elihw

		      //get the symbol value
		      if(!Get(arr, 4, inn[count], j, name))
			{
			  errors++;
			}//fi
		      //get the rest of the line and throw it away
		      getline(inn[count], st);
		      //if the symbol isn't in the table, add it, and define the value
		      if(!Defined(tot, sym_arr, k))
			{
			  strcpy(tot.external[tot.N_length].name, sym_arr);
			  if(!Convert_Hex(tot.external[tot.N_length].value , arr, j, name))
			    {
			      errors++;
			    }//fi
			  //increment the total number of symbols
			  tot.N_length++;
			  //increment the number of symbols in this program
			  Nnum++;
			  //external symbol value must be within the footprint
			  if(tot.external[tot.N_length-1].value>header_length[count]-1)
			    {
			      errors++;
			      err="link ERROR: #18\n\tEntry point value not between [segment "
				  "load address] and [segment load address + segment length].";
			      throw err;
			    }//fi
			  tot.external[tot.N_length-1].value+=tot.file[count].PLA;
			}//fi
		      //the symbol is already in the table
		      else
			{
			  errors++;
			  err="link ERROR: #7\n\tDuplicate entry points defined.  "
			      "Value of first entry point taken.";
			  throw err;
			}//esle
		      
		    }//fi

		  else//max symbols<5(header + 4 ENTs) throw error or
		      //EXT not at beginning
		    {
		      //max symbols<5	
		      if(Ninsert==true)
			{
			  errors++;
			  getline(inn[count], st);
			  err="link ERROR: #8\n\tGreater than 4 entry points. "
			      "Entry point will not used. ";
			  throw err;
			}//fi
		      //EXT have to be at the beginning of the program
		      else
			{
			  errors++;
			  getline(inn[count], st);
			  err="link ERROR: #9\n\tEntry points must be declared prior "
			      "to all text records.";
			  throw err;
			}//esle
		    }//esle
		  break;

		case 'S':
		case 'L':
		case 'T':
		  //================================================
		  //SHORT/LONG relocatables & TEXT
		  //================================================
		  if(Einsert==false)
		    {
		      go=false;
		      errors++;
		      err="link ERROR: #25\n\tText records after end record.";
		      throw err;
		    }//fi 
		  if(Rinsert==true)
		    {
		      go=false;
		      errors++;
		      err="link ERROR: #24\n\tHeader record undefined.";
		      throw err;
		    }//fi
		  //get the address
		  if(!Get(arr, 4, inn[count], j, name))
		    {
		      errors++;
		    }//fi
		  if(!Convert_Hex(tot.file[count].line[i].address, arr, j, name))
		    {
		      errors++;
		    }//fi
		  //make sure address is within footprint
		  if(tot.file[count].line[i].address>header_length[count]-1)
		    {
		      errors++;
		      err="link ERROR: #19\n\tAddress not between [segment load address] "
			  "and [segment load address + segment length].";
		      getline(inn[count], st);
		      throw err;
		    }//fi
		  //get the value
		  if(!Get(arr, 4, inn[count], j, name))
		    {
		      errors++;
		    }//fi
		  if(!Convert_Hex(tot.file[count].line[i].value, arr, j, name))
		    {
		      errors++;
		    }//fi
		  //get the rest of the line and throw it away
		  getline(inn[count], st);
		  if((tot.file[count].line[i].value & 0x1FF)>(header_length[count]-1)
		     and tot.file[count].line[i].type=='S')
		    {
		      errors++;
		      err="link ERROR #28\n\tBits 0-8 of 'S' record are not within the footprint.";
		      throw err;
		    }
		  if(tot.file[count].line[i].value>(header_length[count]-1)
		     and tot.file[count].line[i].type=='L')
		    {
		      errors++;
		      err="link ERROR #29\n\tBits 0-15 of 'L' record are not within the "
			  "footprint.";
		      throw err;
		    }
		  //increment the segment length 
		  tot.file[count].seg_len++;
		  break;

		case 'X':
		case 'Y':
		  //===================================================
		  //SHORT(X)/LONG(Y) relocatables with external symbols
		  //===================================================
		 
		  if(Rinsert==true)
		    {
		      go=false;
		      errors++;
		      err="link ERROR: #24\n\tHeader record undefined.";
		      throw err;
		    }//fi
		  if(Einsert==false)
		    {
		      go=false;
		      errors++;
		      err="link ERROR: #25\n\tText records after end record.";
		      throw err;
		    }//fi
		  //get the address
		  if(!Get(arr, 4, inn[count], j, name))
		    {
		      errors++;
		    }//fi
		  if(!Convert_Hex(tot.file[count].line[i].address, arr, j, name))
		    {
		      errors++;
		    }//fi
		  //address must be within the footprint
		  if(tot.file[count].line[i].address>header_length[count]-1)
		    {
		      errors++;
		      err="link ERROR: #19\n\tAddress not between [segment load address] "
			  "and [segment load address + segment length].";
		      getline(inn[count], st);
		      throw err;
		    }//fi
		  //get the value
		  if(!Get(arr, 4, inn[count], j, name))
		    {
		      errors++;
		    }
		  if(!Convert_Hex(tot.file[count].line[i].value, arr, j, name))
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
			}//end if
		   
		    }while(ch!=' ' & ch!='\n' & ch!='\t');
		  if(ch!='\n')
		    {
		      getline(inn[count], st);
		    }
		  if(chars==0)
		    {
		      errors++;
		      err="link ERROR: #22\n\tRelocatable record with symbol missing a symbol.";
		      throw err;
		    }//fi
		  //throw error if chars>=6, since index starts at 0
		  if(chars>6)
		    {
		      errors++;
		      err="link ERROR: #10\n\tExternal symbol '"
			  +(string)tot.file[count].line[i].symbol
			  +"' greater than 6 characters long.";
		      throw err;
		    }//fi

		  tot.file[count].seg_len++;
		  break;
		  
		case ';':
		  //===================================
		  //COMMENT
		  //===================================
		  getline(inn[count], st);
		  break;	
		case 'E':
		  //==========================================
		  //END
		  //==========================================
		  if(Rinsert==true)
		    {
		      go=false;
		      errors++;
		      err="link ERROR: #24\n\tHeader record undefined.";
		      throw err;
		    }//fi
		  if(Einsert==true)
		    {
		      Einsert=false;
		      Get(arr, 4, inn[count], j, name);
		      getline(inn[count], st);
		      if(!Convert_Hex(tot.file[count].line[i].address, arr, j, name))
			{
			  errors++;
			}//fi
		      if(tot.file[count].line[i].address>header_length[count]-1)
			{
			  errors++;
			  err="link ERROR: #20\n\tStart address not between [segment load "
			      "address] and [segment load address + segment length].";
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
		      getline(inn[count], st);
		      err="link ERROR: #11\n\tEND record already declared.";
		      throw err;
		    }
			 break;
			 case 'H':
			 //===========================================
			 //invalid header
			 //===========================================
			 getline(inn[count], st);
			 Rinsert=false;
			 Ninsert=true;
			 errors++;
			 go=false;
			 err="link ERROR: #12\n\tProgram is not relocatable.";
			 throw err;
			 
			 break;
			 //============================================
			 //NEWLINE/TAB/SPACE
			 //============================================
			 case '\n':
			 ;
			 break;
			 case '\t':
			 case ' ': 
			 getline(inn[count], st);
			 ;
			 break;
			 default:
			 //=============================================
			 //INVALID INPUT
			 //=============================================
			 getline(inn[count], st);
		  ;
		  errors++;
		  err="link ERROR: #13\n\tInvalid character at line start.";
		  throw err;
		  
		  break;
		}//end switch
	    }
	  catch(string e)
	    { 
	      cerr<<e<<endl<<"\tFILE: "<<name<<"\tLINE: "<<j<<endl;
	    }
	  
	  i++;//i is the index of the array
	  j++;//j is the actual line counter
	}//end while
      try
	{
	  if(tot.file[count].seg_len>header_length[count])
	    {
	      errors++;
	      err="link ERROR: #14\n\tText records do not fit in the footprint.";;
	      throw err;
	      
	    }
	}//

      catch(string e)
	{
	  cerr<<e<<endl<<"\tFILE: "<<name<<endl<<"\ttext record length is: "
	      <<tot.file[count].seg_len<<"\n\tfootprint length is: "
	      <<header_length[count]<<endl;
	}
     
      try
	{
	  if(Einsert==true)
	    {
	      errors++;
	      err="link ERROR: #23\n\tNo end record.";
	      throw err;
	    }//fi
	}//yrt
      catch(string e)
	{
	  cerr<<e<<endl<<"\tFILE: "<<name<<endl;
	}//hctac
      count++;
    }//end while
    //===================================================
  //pass 1 ends
  //=====================================================
  

  try
    {
      if(tot.file_num==1 and tot.file[0].seg_len==0)
	{
	  errors++;
	  err="link ERROR: #27\n\tOne relocatable program with no text records cannot "
	      "be relocated.";
	    throw err;
	}
    }
  catch(string e)
    {
      cerr<<e<<endl;
    }
  //main has to be defined
  try
    {
      if(!Defined(tot, "MAIN", k))
	{
	  errors++;
	  err="link ERROR: #17\n\tMAIN undefined.";
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
      tot.prog_len=tot.prog_len+header_length[count];
      count++;
    }

  //program length cannot be greater than 512
  try
    {
      if(tot.prog_len>512)
	{
	  errors++;
	  err="link ERROR: #16\n\tLinked programs do not fit on one page.";
	  throw err;
	}
    }
  catch(string e)
    {
      cerr<<e<<endl<<"\ttotal length is: "<<dec<<tot.prog_len<<endl;
    }

  //don't continue if there are errors
  if(errors>0)
    {
      cout<<"..."<<dec<<errors<<" Error(s) in input files... "<<endl
	  <<"...Output file will not be generated..."<<endl<<endl;
      return false;
    }
  //prompt for the relocation address
  else
    {
      cout<<"...No errors in input files..."<<endl<<endl;
      go=false;
      while(go==false)
	{
	  cout<<"Please enter the relocation start address (in hex)"<<endl
	      <<"valid hex range: [0000, "
	      <<setw(4)<<hex<<uppercase<<setfill('0')<<0x0200-tot.prog_len<<"]"<<endl;  
	  cin>>hex>>address;
	  if((address & 0x1FF)<=(0x0200-tot.prog_len) and  address>=0 and address<0xFFFF)
	    {
	      tot.IPLA=(unsigned short)address;
	      go=true;
	    }//fi
	  else    
	    {
	     getline(cin, st);
	      cout<<"you entered an invald address: "<<hex<<address<<endl;
	    }//esle
	}//elihw
    }//esle
  //============================================================
  //pass 2 begins
  //============================================================ 
  count=0;
  errors=0;
  while(count<tot.file_num and go==true and errors<20 and warnings<20)
    {
      i=0;
      do
	{
	  try
	    {
	      switch(tot.file[count].line[i].type)
		{	  
		case 'R':
		  name=tot.file[count].line[i].symbol;
		  if(count==0)
		    {
		      out<<"HMAIN  "<<setw(4)<<uppercase<<hex<<setfill('0')
			 <<tot.IPLA<<setw(4)<<setfill('0')<<hex<<uppercase
			 <<tot.prog_len<<endl;
		    }//fi
		  break;
		case 'N':
		  break;
		case 'S':
		  out<<"T"<<setw(4)<<setfill('0')<<hex<<uppercase
		     <<tot.file[count].line[i].address+tot.file[count].PLA+tot.IPLA;
		  out<<setw(4)<<setfill('0')<<hex<<uppercase
		     <<tot.file[count].line[i].value + tot.IPLA+ tot.file[count].PLA<<endl;

		  break;
		case 'L':
		  out<<"T"<<setw(4)<<setfill('0')<<hex<<uppercase
		     <<tot.file[count].line[i].address+tot.file[count].PLA+tot.IPLA
		     <<setw(4)<<setfill('0')<<hex<<uppercase
		     <<tot.file[count].line[i].value + tot.IPLA+tot.file[count].PLA<<endl;

		  break;
		case 'X':
		  out<<"T"<<setw(4)<<setfill('0')<<hex
		     <<uppercase<<tot.file[count].line[i].address+tot.file[count].PLA+tot.IPLA;
		  //output the relocated value if the external
		  //symbol is defined
		  if(!Defined(tot, tot.file[count].line[i].symbol, k))
		    {
		      errors++;
		      out<<setw(4)<<setfill('0')<<hex<<uppercase<<tot.IPLA<<endl;
		      err= "link ERROR: #15\n\tExternal symbol '"
			  + (string)tot.file[count].line[i].symbol +"' undefined.";
		      throw err;
		    }//fi
		  else
		    {
			  out<<setw(4)<<setfill('0')<<hex<<uppercase
			     <<tot.external[k].value+tot.IPLA+(tot.file[count].line[i].value & 0xFE00)<<endl;	       	
		    }//esle
		  //bits 0-8 should be zero
		  if((tot.file[count].line[i].value & 0x01FF)!=0)
		    {
		      warnings++;
		      err="link WARNING: #1\n\tBits 0-8 of 'X' record not zero.\n\tThey "
			  "will be set to zero before relocation.";
		      throw err;
		    }//fi
		  break;
		case 'Y':
		  out<<"T"<<setw(4)<<setfill('0')<<hex<<uppercase
		     <<tot.file[count].line[i].address+tot.file[count].PLA+tot.IPLA;
		  //output the relocated value if the external symbol is defined
		  if(!Defined(tot, tot.file[count].line[i].symbol, k))
		    {
		      errors++;
		      out<<setw(4)<<setfill('0')<<hex<<uppercase<<tot.IPLA <<endl;
		      err="link ERROR: #15\n\tExternal symbol '"
			  + (string)tot.file[count].line[i].symbol +"' undefined.";
		      throw err;
		    }//fi
		  else
		    {
		      out<<setw(4)<<setfill('0')<<hex<<uppercase<<tot.IPLA+tot.external[k].value<<endl;	
		    }//esle
		  //bits 0-15 should be zero
		  if(tot.file[count].line[i].value!=0)
		    {
		      warnings++;
		      err= "link WARNING: #2\n\tBits 0-15 of 'Y' record not zero.\n\tThey "
			  "will be set to zero before relocation.";
		      throw err;
		    }//fi
		  
		  break;
		case 'T':
		  out<<"T"<<setw(4)<<setfill('0')<<hex<<uppercase
		     <<tot.file[count].line[i].address+tot.file[count].PLA+tot.IPLA
		     <<setw(4)<<setfill('0')<<hex
		     <<uppercase<<tot.file[count].line[i].value<<endl;
		  break;
		case 'E':
		default:	
		  break;
		}//end switch
	    }//yrt
	  catch(string e)
	    {
	      cerr<<e<<endl<<"\tFILE: "<<name<<"\tLINE: "<<i+1<<endl;
	    }
	  i++;
	}while(tot.file[count].line[i].type!='E' and errors<20 and warnings<20);
      count++;
    }//elihw
  
  //output the END record
  out<<"E"<<setw(4)<<setfill('0')<<hex<<uppercase
     <<tot.file_start+tot.file[tot.file_main].PLA+tot.IPLA;
  
  //output whether the file is correct or not
  if(warnings>0)
    {
      cout<<"..."<<dec<<warnings<<" Warning(s) in generation of output file... "<<endl
	  <<"...Output file may not be correct.."<<endl<<endl;
    }//fi
  if(errors>0)
    {
      cout<<"..."<<dec<<errors<<" Error(s) in generation of output file... "<<endl
	  <<"...Output file may not be correct..."<<endl<<endl;
      return false;
    }//fi
  
  if(go==true)
    {
      cout<<endl<<"...File created successfully..."<<endl<<endl;
      return true;
    }//fi
  return false;
  //============================================================
  //pass 2 ends
  //============================================================
  
  

}//end program

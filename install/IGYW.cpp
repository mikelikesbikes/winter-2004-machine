// IGYW.cpp
// Author: Travis Nauman
// Date: March 3, 2005
// Class: CSE560

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include "Assembler.h"
#include "Linker.h"
#include "Simulator.h"
#include "defs.h"

using namespace std;

int main(int argc, char* argv[])
{   
    string str[argc];
    string err;
    string asm_err = "\"IGYW -asm [-o] [-l] <file(s)...>\"";
    string lnk_err = "\"IGYW -link <in_file(s)...> -o <out_file>\"";
    string run_err = "\"IGYW -run <file>\"";
    int k=1;
    
    for(int i=0;i<argc;i++)
    {
	str[i] = argv[i];
    }
    try
    {
	if (k < argc)
	{
	    if (str[k] == "-asm")
	    {
		string bas_filename, src_filename, obj_filename, lst_filename;
		bool o_open=false, l_open=false;
		k++;
		
		if (argc < 3)
		{
		    err = "\nIGYW Assembler Usage:\n\t" + asm_err;
		    throw err;
		}
		if (argc > 3)
		{
		    if (str[k] == "-o" or str[k+1] == "-o")
		    {
			o_open=true;
			k++;
			if (str[k] == "-l" or str[k-1] == "-l")
			{
			    l_open=true;
			    k++;
			}
		    }
		    else if (str[k] == "-l" or str[k-1] == "-l")
		    {
			l_open=true;
			k++;
			if (str[k] == "-o" or str[k-1] == "-o")
			{
			    o_open=true;
			    k++;
			}
		    }
		}
		if (k == argc)
		{
		    err = "\nIGYW Assembler Usage:\n\t" + asm_err;
		    throw err;
		}
		while (k < argc)
		{
		    ifstream source;
		    ofstream object, list;
		    
		    try
		    {
			src_filename = str[k];
			bas_filename = str[k];
			
			int j = bas_filename.rfind(".", bas_filename.length()-1);
			if (j>0) bas_filename.erase(j);
			
			src_filename = str[k];
			
			source.open(src_filename.c_str());
			
			if (o_open)
			{
			    obj_filename = bas_filename + ".o";
			    object.open(obj_filename.c_str());
			}
			if (l_open)
			{
			    lst_filename = bas_filename + ".lst";
			    list.open(lst_filename.c_str());
			}
			
			if (source.is_open())
			{
			    if(Assembler(object, list, source))
			    {
				cout << src_filename << " assembled." << endl;
				if (object.is_open()) object.close();
				if (list.is_open()) list.close();
				source.close();
			    }
			    else
			    {
				err = "Error(s) encountered while assembling " + src_filename;
				if (object.is_open()) object.close();
				if (list.is_open()) list.close();
				source.close();
				throw err;
			    }
			}
			else
			{
			    err = "Could not open source file: " + src_filename;
			    if (object.is_open()) object.close();
			    if (list.is_open()) list.close();
			    throw err;
			}			
		    }//end try
		    catch(string e)
		    {
			cout << e << endl;
		    }
		    k++;
		}// end while
	    }// end -asm
	    else if (str[k] == "-link")
	    {
		if(argc < 5)
		{
		    err = "\nIGYW Linker Usage:\n\t" + lnk_err;
		    throw err;
		}
		
		string link_file;
		ofstream out_file;
		ifstream in_files[PAGE];
		string link_names[PAGE];
		int lnk_ctr=0;
		int file_num=0;
		
		//look at next argument
		k++;
		
		while(str[k] != "-o")
		{
		    link_names[lnk_ctr] = str[k];
		    lnk_ctr++;
		    k++;
		    if (k >= (argc-1))
		    {
			err = "IGYW Linker Usage:\n\t" + lnk_err;
			throw err;
		    }
		}
		
		file_num = lnk_ctr;
		
		k++;
		lnk_ctr=0;
		link_file = str[k];
		
		while(lnk_ctr < file_num)
		{
		    in_files[lnk_ctr].open(link_names[lnk_ctr].c_str());
		    if (! in_files[lnk_ctr].is_open())
		    {
			//should we close streams here?
			err = "Could not open file: "
			    + link_names[lnk_ctr];			    
			throw err;
		    }
		    lnk_ctr++;   
		}
		out_file.open(link_file.c_str());
		if (out_file.is_open())
		{
		    (void) Linker(in_files, out_file, file_num);
		    if (out_file.is_open()) out_file.close();
		    lnk_ctr=0;
		    while (lnk_ctr < file_num)
		    {
			if (in_files[lnk_ctr].is_open()) in_files[lnk_ctr].close();
			lnk_ctr++;
		    }
		}
		else
		{
		    err = "Could not open file: "
			+ link_file;
		    throw err;
		}
 	    }//end -link
	    else if (str[k] == "-run")
	    {
		string run_file;
		ifstream run_stream;
		
		//look at next argument
		k++;
		
		if (k > argc-1)
		{
		    err = "\nIGYW Simulator Usage:\n\t" + run_err;
		    throw err;
		}
		else
		{
		    run_file = str[2];
		    run_stream.open(run_file.c_str());
		    if (run_stream.is_open())
		    {
			(void) Simulator(run_stream);
			run_stream.close();
		    }
		    else
		    {
			err = "Could not open file: " + run_file;
			throw err;
		    }
		}
	    }//end -run    
	    else
	    {
		err = "\nIGYW Usage:\n\t" + asm_err + "\n\t" + lnk_err + "\n\t" + run_err;
		throw (err);
	    }
	}
	else
	{
	    err = "\nIGYW Usage:\n\t" + asm_err + "\n\t" + lnk_err + "\n\t" + run_err;
	    throw (err);
	}
    }//end try
    catch (string e)
    {
	cerr << e << endl;
	return 1;
    }
    return 0;
}


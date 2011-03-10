#ifndef _UTILS_
#define _UTILS_

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


bool Get(char arr[], int num, ifstream &input, int line, char file[]);
bool Convert_Hex(unsigned short& ret, char op[], int line, char file[]);
bool Defined(Total& table, char value[], int counter);


#endif

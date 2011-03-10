#ifndef _W05_IGYW_DEFS_
#define _W05_IGYW_DEFS_

#include <map>
#include <string>

#define CURSOR "|IGYW>"
#define SRC_ARR_SZ 2000
#define SYM_ARR_SZ 100
#define LIT_ARR_SZ 50
#define EXT_ARR_SZ 4
#define ENT_ARR_SZ 5
#define MEM_ARR_SZ 65536
#define REG_ARR_SZ 8
#define PAGE 512

using namespace std;

struct Source{
    unsigned short address;
    string label;
    int opcode;
    string op1;
    string op2;
    string op3;
    string op4;
    bool rel;
};

struct Symbol{
    string name;
    unsigned short value;
    bool rel;
};

struct Literal{
    unsigned short value;
    unsigned short address;
    bool rel;
};

struct External{
    string name;
};

struct Entry{
    string name;
    unsigned short value;
    bool defined;
};

enum Operations {
    ADD=1,LD=2,ST=3,JSR=4,AND=5,LDR=6,STR=7,
    DBUG=8,NOT=9,LDI=10,STI=11,JSRR=12,RET=13,
    LEA=14,TRAP=15,ORIG=16,END=17,EQU=18,FILL=19,
    JMP=20,STRZ=21,BLKW=22,CMNT=23,EXT=24,ENT=25,JMPR=28,
    BR=80,BRP=81,BRZ=82,BRZP=83,BRN=84,BRNP=85,BRNZ=86,BRNZP=87
};

struct Tables{
    int src_len;
    Source src_arr[SRC_ARR_SZ+1];// array index=source line number
                                 // index 0 never used
    int sym_len;
    Symbol sym_arr[SYM_ARR_SZ];
    int lit_len;
    Literal lit_arr[LIT_ARR_SZ];
    int ext_len;
    External ext_arr[EXT_ARR_SZ];
    int ent_len;
    Entry ent_arr[ENT_ARR_SZ];
    Operations op; // enum
    map<string,Operations> ops_map;
    unsigned short start; // footprint start address
    unsigned short end; // footprint end address
    unsigned short exec; // start of execution address
};

enum mode{quietMode, traceMode, stepMode, stepxMode};

struct W05 {
    unsigned short mem_arr[MEM_ARR_SZ];
    unsigned short reg_arr[REG_ARR_SZ];
    unsigned short pc;
    unsigned short ccr;
    mode run_mode;
};


struct Xsymbol {
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
  Line_rec line[1000];
  long seg_len;
  unsigned short PLA;
};

struct Total{
  File_rec file[512];
  int file_num;
  int file_main;
  Xsymbol external[2560];
  int N_length;
  unsigned short prog_len;
  unsigned short IPLA;
  int file_start;
};

#endif

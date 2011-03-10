main     .ORIG
         .EXT    one,two,three,four
msg      .STRZ   "Test Passed"
PUTS     .EQU    x22
HALT     .EQU    x25
OUTN     .EQU    x31
Begin    LD      R0,one
         TRAP    OUTN
         LD      R0,two
         TRAP    OUTN
         LD      R0,three
         TRAP    OUTN
         LD      R0,four
         TRAP    OUTN
         LEA     R0,msg
         TRAP    PUTS
         TRAP    HALT
         .END    Begin

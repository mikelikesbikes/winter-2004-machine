;Filename: stupid.asm
;Testing: normal Linking

MAIN     .ORIG
         .EXT    one,two,negone
onestr   .STRZ   "one = "
twostr   .STRZ   "two = "
negstr   .STRZ   "neg = "
comp     .STRZ   "complete"
Begin    LEA     R0,onestr
         TRAP    x22
         LD      R0,one
         TRAP    x31
         LEA     R0,twostr
         TRAP    x22
         LD      R0,two
         TRAP    x31
         LEA     R0,negstr
         TRAP    x22
         LD      R0,negone
         TRAP    x31
         LEA     R0,comp
         TRAP    x22
END      TRAP    x25
         .END    Begin


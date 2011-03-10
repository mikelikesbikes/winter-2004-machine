;Filename: parseerr2.asm
;Testing: Generation of error message #41

test .ORIG

     .ENT ent1

begin ADD R1, R1, R7
ent1  .EQU begin	;should produce no error
ent1  NOT R1, R3	; should produce error
      TRAP x25

      .END

;Filename: parseerr3.asm
;Testing: Generation of error message #42

test .ORIG

     .EXT ext1

     LD R1, ext1	;should produce no error
ext1 ADD R1, R2, #12	; should produce error

     TRAP x25
     .END test

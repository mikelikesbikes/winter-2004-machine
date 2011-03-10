;Filename: parseerr1.asm
;Testing: Generation of error message #48 by both .ENT and .EXT

test .ORIG

     .ENT ent1, ent2, ent2
     .EXT test, ext1

ent1 LD R1, =#12
ent2 .EQU ent1
     TRAP x25

     .END

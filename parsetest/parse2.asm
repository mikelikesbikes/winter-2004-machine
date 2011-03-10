;Filename: parse2.asm
;Testing: EXT/ENT

t2 .ORIG

;comments before ENT/EXT accedpted?

;accepts .EXT before .ENT?
.EXT ext1     ;1 operand of .EXT
.ENT ent1, ent2	 ; 2 operands accepted?

BEGIN ADD R1, R1, R1
      DBUG

NOT   R3, R3

      LD R1, ext1
ent1 .EQU BEGIN
ent2 JSR ent2

     TRAP x25

.END t2	  ;program name as operand of .END

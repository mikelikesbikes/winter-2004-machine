;Filename: parseerr5.asm
;Testing: Generation of error message #44

test	  .ORIG

	  .ENT ent1, ent2, ent3

Begin	  ADD R1, R2, R3
	  NOT R1, R1
ent2	  .EQU Begin		;define ent2
	  TRAP x25
	  .END

;should produce messages stating that ent1 and ent3 were never defined

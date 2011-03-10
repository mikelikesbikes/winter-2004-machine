;Filename: parseerr6.asm
;Testing: Generation of error message #45

test	  .ORIG x0000000000

	  .ENT ent2, ent6	;should produce error
	  .EXT ext1		;should produce error

Begin	  ADD R1, R4, R3
	  TRAP x25
	  .END	Begin

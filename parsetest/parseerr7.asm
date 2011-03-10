;Filename: parseerr7.asm
;Testing: Generation of error message #46

test	  .ORIG

	  .ENT ent1, ent2
	  .EXT ext1
	  .EXT ext2		;should produce error
	  .ENT newent		;should produce error

	  AND R3, R4, R5
	  TRAP x25
	  .END test

;Filename: parseerr9.asm
;Testing: Generation of error message #49

test	  .ORIG

	  .ENT ent1, ent2, ent3, x1F	;should produce error
	  .EXT ext1, ext2, ext3, R7	;should produce error

	  AND R1, R2,R3
	  TRAP x25
	  .END

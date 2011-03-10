;Filename: parseerr8.asm
;Testing: Generation of error message #47

test	  .ORIG

; begin instructions
Begin	ADD R1, R2, R3
	.ENT ent1		;should produce error
	NOT R7, R0
	.EXT ext2		;should produce error
	TRAP x25
	.END

;Filename: parseerr11.asm
;Testing: Generation of error message #51

test	  .ORIG

ents	  .ENT ent1		;error!!
	  .EXT extone		; no error

	  TRAP x25
	  .END 

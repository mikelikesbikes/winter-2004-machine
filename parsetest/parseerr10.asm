;Filename: parseerr10.asm
;Testing: Generation of error message #50

test	  .ORIG

	  .ENT one, two, three, four	;no error
	  .EXT won, too, threee, for, five  ;should have error

one	  ADD R5, R6, R7
two	  .EQU	  one
three	  .EQU	  two
four	  .EQU	  one
	  TRAP x25
	  .END

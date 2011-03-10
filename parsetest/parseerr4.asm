;Filename: parseerr4.asm
;Testing: Generation of error message #43

test	  .ORIG

	  .ENT ent1, ent2, ent3, ent4

ent1	  ADD R1, R1, R1	; should produce no error
label	  .FILL xFFFF		
ent2	  .EQU label		;should produce no error
abs	  .EQU #34
ent3	  .EQU abs		;should produce error
ent4	  .EQU x1F		;should produce error
	  TRAP x25
	  .END x0

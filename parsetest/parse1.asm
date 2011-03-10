;Filename: parse1.asm
;Testing: .ENT/.EXT

t1 .ORIG
; .ENT declarations
   .ENT ent1, ent2, ent3, ent4	;accepts 4 operands?
; .EXT declarations
  .EXT ext1, ext2, ext3	;accepts less than 4 operands?

; body of program

Begin  ADD R1, R2, R3
ent1    .EQU Begin  ;accepts explicit definition of entrypoint?
	AND R1, R2,#4
	
ent3	.EQU t1	;use of program name as a symbol
		; entry point defined to program name
		; entry points defined in differen order than declared
ent4	.FILL ent4	;accepts implicit definition of entry point?
			;accepts entry point as operand of .FILL
	LD R1, ent2	; forward reference to entry point
			; entry point as operand of LD
        LD R1, ext3	; external symbol as operand of LD
        LD R3, =#-12

	.FILL ext3	; external symbol used as operand of .FILL

ent2	.EQU  ent4	;entry point defined to another entry point

	BRNZP ent4	;entry point as operand of BR
	BRZ   ext2	; ext symbol used as operand of BR
	JSR   ent1	; ent as operand of JSR
	JSR  ext2	; ext as operand of JSR
	JMP  ent3	;ent as operand of JMP
	JMP  ext2	; ext as operand of JMP
	LDI R7, ext3	; ext as opernad of LDI
	LDI R0, ent4	; ent as operand of LDI
	LDI R3, t1	; program name used as relative symbol
	LEA R4, ent4	; ent as operand of LEA
	LEA R5, ext2	; ext as operand of LEA
	ST  R2, ent4	; ent as operand of ST
	ST  R6, ext3	; ext as operand of ST
	STI R5, ext3	; ext as operand of STI
	STI R3, ent1	; ent as operand of STI
	STI R7, t1	; program name use
	TRAP x25	; halt

.END ent1    ; ent used as operand of .END

;uses some but not all declared external symbols
; ext1 not used


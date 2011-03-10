;Main for Array
Main	.ORIG	
	.EXT	array,subrst,INC
INTOUT	.EQU	x31
Addr	.BLKW	#1
Sum	.BLKW	#1
Start	LD	R1,=#-14
Loop1	LD	R0,array
	TRAP	INTOUT
	JSR	INC
	BRN	Loop1
	LD	R1,=#-14
	LD	R2,=#0
	LEA	R4,array
	ST	R4,Addr
	LD	R3,Addr
Loop2	ADD	R2,R2,R3
	ADD	R4,R4,#1
	ST	R4,Addr
	LD	R3,Addr
	ST	R2,Sum
	LD	R0,Sum
	TRAP	INTOUT
	BRN	Loop2
	TRAP	x25
	.END	Start
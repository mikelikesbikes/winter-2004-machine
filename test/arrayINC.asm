;increment
Incrmt	.ORIG
	.ENT	INC
HALT	.EQU	x25
One	.EQU	#1
INC	ADD	R1,R1,One
	RET
	.END

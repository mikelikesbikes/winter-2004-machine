MAIN	.ORIG
	.EXT	inc
X	.FILL   #4
Begin   LD	R0,X
loop    ADD     R1,R0,#-10
        BRZP    Done
        JSR     inc
        BRNZP   loop
Done    TRAP    x31
        TRAP    x25
        .END    Begin
        
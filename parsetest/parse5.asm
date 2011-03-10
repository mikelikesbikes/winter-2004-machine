;Filename: parse5.asm
;Testing: program name as an absolute symbol

t5 .ORIG x00001

BEGIN .EQU t5
ADD t5, t5, t5
LD R3, t5
temp .FILL t5
.BLKW t5

TRAP x25

.END t5




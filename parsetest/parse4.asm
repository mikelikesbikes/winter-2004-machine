;Filename: parse4.asm
;Testing: .EXT

t4 .ORIG
; just .EXT
.EXT externalsymbol
BEGIN LD R1, externalsymbol
TRAP x25

.END

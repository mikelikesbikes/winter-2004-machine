;Filename: parse3.asm
;Testing: .ENT

t3 .ORIG

; just .ENT
.ENT ent1

ent1 ADD R1, R2, R3
TRAP x25

.END

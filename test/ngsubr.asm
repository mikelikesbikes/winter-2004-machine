;Subroutine for displaying a series of lines of text
;  The lines of text display a count-down, from X to 0
;Calling convention: register 3 contains return address
;
Mesg     .ORIG
         .ENT    Displ,X
;
Txt      .STRZ   "Value= "
X        .FILL   #6
SavR0    .BLKW   #1
SavR1    .BLKW   #1
;
Displ    ADD     R4,R7,#0       ; *** SAVE R7! our return address!
         ST      R0,SavR0
         ST      R1,SavR1       ;M[SavR1] <- r1
         LD      R1,X           ;r1 <- M[X]
         BRN     Done        ;if (r1 < 0) goto Done
Loop     LEA     R0,Txt
         TRAP    x22            ;Display text "Value=  "
         LD      R0,X
         TRAP    x31            ;Display value in M[X]
         ADD     R0,R0,#-1
         ST      R0,X          ;M[X] <- r0
         BRN     Done           ;if (r0 < 0) goto Done
         JMP     Loop          ;goto Loop
Done     LD      R0,SavR0       ;r0 <- M[SavR0]
         LD      R1,SavR1       ;r1 <- M[SavR1]
         ADD     R7,R4,#0       ; *** RESTORE BACKUP of R7!
         RET
         .END    Displ
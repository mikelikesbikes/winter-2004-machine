Main     .ORIG
         .EXT    Displ,V,X
         .ENT    Start
;
Start    JSR     Displ   ;Display 6..0
         LD      R1,V    ;r1 <- M[V]
         ST      R1,X    ;M[X] <- r1
         JSR     Displ   ;Display 2..0
         TRAP    x25     ;halt
         .END    Start 
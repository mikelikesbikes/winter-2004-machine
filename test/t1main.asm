MAIN     .ORIG
msg      .STRZ   "Trivial"
Begin    LEA     R0,msg
         TRAP    x22
         TRAP    x25
         .END    Begin
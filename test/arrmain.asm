main     .ORIG
         .EXT    arr1,arr2
         ST      R0,arr1
         TRAP    x31
         ST      R0,arr2
         TRAP    x31
         TRAP    x25
         .END
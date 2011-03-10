CC=g++
flag=-c
M=main
L=Linker
U=Utils

objects = $(M).o $(L).o $(U).o


link:	$(objects)
	$(CC) -o link $(objects)


$(M).o: $(M).cpp $(M).h
	$(CC) $(flag) $(M).cpp
$(L).o: $(L).cpp $(L).h
	$(CC) $(flag) $(L).cpp
$(U).o: $(U).cpp $(U).h
	$(CC) $(flag) $(U).cpp

objects = Assembler.o Simulator.o Parser.o Get.o Interpreter.o Writer.o \
	Tokenizer.o Loader.o Encoder.o Linker.o Display.o Utility.o

.FAILED:
	@echo "Failed"
.DONE:	
	@echo "Complete"

all : $(objects) defs.h
	@echo "Compiling IGYW (W05 Emulator)..."
	@g++ -o ../IGYW $(objects) IGYW.cpp
	@echo "Setting File Permissions..."
	@chmod -f 775 *

Assembler.o : Assembler.cpp Assembler.h defs.h Parser.o Writer.o Encoder.o
	@echo "Compiling Assembler..."
	@g++ -c Assembler.cpp
Simulator.o : Simulator.cpp Simulator.h defs.h Interpreter.o Loader.o
	@echo "Compiling Simulator..."
	@g++ -c Simulator.cpp
Parser.o : Parser.cpp Parser.h defs.h Get.o
	@echo "Compiling Parser..."
	@g++ -c Parser.cpp
Get.o : Get.cpp Get.h defs.h Tokenizer.o
	@echo "Compiling Get..."
	@g++ -c Get.cpp
Interpreter.o : Interpreter.cpp Interpreter.h defs.h Display.o
	@echo "Compiling Interpreter..."
	@g++ -c Interpreter.cpp
Writer.o : Writer.cpp Writer.h defs.h Utility.o
	@echo "Compiling Writer..."
	@g++ -c Writer.cpp
Tokenizer.o : Tokenizer.cpp Tokenizer.h defs.h Utility.o
	@echo "Compiling Tokenizer..."
	@g++ -c Tokenizer.cpp
Loader.o : Loader.cpp Loader.h defs.h Utility.o
	@echo "Compiling Loader..."
	@g++ -c Loader.cpp
Encoder.o : Encoder.cpp Encoder.h defs.h Utility.o
	@echo "Compiling Encoder..."
	@g++ -c Encoder.cpp
Linker.o : Linker.cpp Linker.h defs.h Utility.o
	@echo "Compiling Linker..."
	@g++ -c Linker.cpp
Display.o : Display.cpp Display.h defs.h
	@echo "Compiling Display..."
	@g++ -c Display.cpp
Utility.o : Utility.cpp Utility.h defs.h
	@echo "Compiling Utility..."
	@g++ -c Utility.cpp

clean:	
	@echo "Deleting Object Files..."
	@rm -f $(objects) IGYW

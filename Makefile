# Makefile for PDP-11 ISA Simulator
#
# Builds the PDP-11 .PCascii file and then calls the qmake
# makefile in the src directory to build the simulator

AS = bin/macro11
TRANS = bin/obj2ascii
PY = python2

# Macro11 assembly
MACS = $(wildcard src/*.mac)
PDP_TARGETS = $(patsubst src/%.mac, src/%.ascii, $(MACS))

# Python2 .lst parser
PYS = src/asciiPCinsert.py
PY_TARGETS = $(patsubst src/%.mac, src/%.PCascii, $(MACS))

# Simulator binary
SIM = src/simulator
SIM_FLAGS = -V
SIM_GUI_FLAGS = -V -g

# Make commands
all : $(PDP_TARGETS) $(PY_TARGETS)
	cd src;$(MAKE)


src/%.PCascii : src/%.ascii
	$(PY) $(PYS) src/$*.ascii src/$*.lst $@


src/%.ascii : src/%.mac
	$(AS) $< -o src/$*.obj -l src/$*.lst
	$(TRANS) src/$*.obj $@


debug: all
	valgrind\
		--tool=memcheck\
		--leak-check=full\
		--track-origins=yes\
		--vgdb=yes\
		--vgdb-error=0\
		-v\
		./$(SIM) $(PY_TARGETS)


leak-check: all
	valgrind\
		--tool=memcheck\
		--leak-check=full\
		--show-leak-kinds=all\
		--track-origins=yes\
		-v\
		./$(SIM) $(PY_TARGETS)


simulate: all
	./$(SIM) $(SIM_FLAGS) $(PY_TARGETS)


simulate-gui: all
	./$(SIM) $(SIM_GUI_FLAGS) $(PY_TARGETS)


clean :
	rm -rf src/*.obj
	rm -rf src/*.lst
	rm -rf src/*.ascii
	rm -rf src/*.PCascii
	rm -rf src/*.o
	rm -rf $(SIM)
	rm -rf src/trace.txt
	cd src; make clean

.PHONY : all clean debug leak-check simulate simulate-gui

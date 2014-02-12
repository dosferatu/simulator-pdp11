# Makefile for PDP-11 ISA Simulator

AS = ../bin/macro11
TRANS = ../bin/obj2ascii
CC = g++
CCFLAGS = -g -Wall -pipe -std=gnu++11
PY = python2

# Macro11 assembly
MACS = $(wildcard *.mac)
PDP_TARGETS = $(patsubst %.mac, %.ascii, $(MACS))

# Python2 .lst parser
PYS = asciiPCinsert.py
PY_TARGETS = $(patsubst %.mac, %.PCascii, $(MACS))

# C++ source
SIM = simulator
SIM_FLAGS = -V	# Verbosity: v for minimal, V for verbose debug output
SIMS = $(wildcard *.cpp)
SIMOBJ = $(patsubst %.cpp, %.o, $(SIMS))
SIM_TARGETS = $(patsubst %.cpp, %.o, $(SIMS))

# Make commands
all : $(PDP_TARGETS) $(PY_TARGETS) $(SIM_TARGETS) $(SIMOBJ)


%.PCascii : %.ascii
	$(PY) $(PYS) $*.ascii $*.lst $@


%.ascii : %.mac
	$(AS) $< -o $*.obj -l $*.lst
	$(TRANS) $*.obj $@


%.o : %.cpp
	$(CC) $(CCFLAGS) $(SIMS) -o $(SIM)


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


clean :
	rm -rf *.obj *.lst *.ascii *.PCascii
	rm -rf *.o
	rm -rf $(SIM)
	rm -rf trace.txt

.PHONY : all clean debug leak-check simulate

# Makefile for PDP-11 ISA Simulator

AS = ../bin/macro11
TRANS = ../bin/obj2ascii
CC = g++
CCFLAGS = -g -Wall -pipe -std=gnu++11

# Compile and translate any .mac in the directory
MACS = $(wildcard *.mac)
PDP_TARGETS = $(patsubst %.mac, %.ascii, $(MACS))
SIM = simulator
SIMS = $(wildcard *.cpp)
SIMOBJ = $(patsubst %.cpp, %.o, $(SIMS))
SIM_TARGETS = $(patsubst %.cpp, %.o, $(SIMS))

all : $(PDP_TARGETS) $(SIM_TARGETS) $(SIMOBJ)

%.ascii : %.mac
	$(AS) $< -o $*.obj -l $*.lst
	$(TRANS) $*.obj $@

%.o : %.cpp
	$(CC) $(CCFLAGS) $(SIMS) -o $(SIM)

debug: all
	valgrind\
		--tool=memcheck\
		--leak-check=yes\
		--track-origins=yes\
		--vgdb=yes\
		--vgdb-error=0\
		-v\
		./$(SIM) $(PDP_TARGETS)

leak-check: all
	valgrind\
		--tool=memcheck\
		--leak-check=yes\
		--leak-check=full\
		--show-leak-kinds=all\
		--track-origins=yes\
		-v\
		./$(SIM) $(PDP_TARGETS)

clean :
	rm -rf *.obj *.lst *.ascii
	rm -rf *.o
	rm -rf $(SIM)

simulate: all
	./$(SIM) $(PDP_TARGETS)

.PHONY : all clean debug leak-check simulate

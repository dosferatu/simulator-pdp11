# Makefile for PDP-11 ISA Simulator

AS = ../bin/macro11
TRANS = ../bin/obj2ascii
CC = g++
CCFLAGS = -g -Wall -std=gnu++11

# Compile and translate any .mac in the directory
MACS = $(wildcard *.mac)
PDP_TARGETS = $(patsubst %.mac, %.ascii, $(MACS))
SIMS = $(wildcard *.cpp)
SIMOBJ = $(patsubst %.cpp, %.o, $(SIMS))
SIM_TARGETS = $(patsubst %.cpp, %.o, $(SIMS))

all : $(PDP_TARGETS) $(SIM_TARGETS) $(SIMOBJ)

%.ascii : %.mac
	$(AS) $< -o $*.obj -l $*.lst
	$(TRANS) $*.obj $@

%.o : %.cpp
	$(CC) $(CCFLAGS) $< -o simulator
	valgrind --tool=memcheck --leak-check=yes ./simulator

clean :
	rm -rf *.obj *.lst *.ascii
	rm -rf *.o
	rm -rf simulator

simulate:

.PHONY : all clean simulate

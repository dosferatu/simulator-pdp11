# Makefile assumes binaries in bin directory at top level of project folder.

# Path to the Macro-11 assembler
ASM = ../bin/macro11 

# Path to the translator
TRANS = ../bin/obj2ascii

# List all macro11 source files here 
TARGET = main

# Clean message
MSG_CLEANING = Cleaning build directory...

# Remove directive
REMOVE = rm -rf

# Build the project
all:
	$(ASM) $(TARGET).mac -o $(TARGET).obj -l $(TARGET).lst
	$(TRANS) $(TARGET).obj $(TARGET).ascii

# Clean the project directory of all generated files
clean:
	@echo
	@echo $(MSG_CLEANING)
	$(REMOVE) $(TARGET).obj
	$(REMOVE) $(TARGET).lst
	$(REMOVE) $(TARGET).ascii

# List phony targets to avoid issues with files in the build directory having
# the same name as the makefile command
.PHONY : all clean

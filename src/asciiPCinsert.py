#!/usr/bin/env python

#   asciiPCinsert.py      
#     uses the .ascii PDP-11/20 memory image file format (developed my Mark Faust), along with the macro11 assembler .lst file 
#     to automatically insert initial PC values into the output .PCascii file, for automatic test generation
#   
#   The format of the added line is: the first line of the output .PCascii file is e.g.: "*177764" where 177764 is the initial PC value
#
#   Also verifies that the input .ascii file does not already have a *-line, and if it does, it leaves the .ascii file intact
#     and copies it to the .PCascii file untouched. This will help if the format of the .ascii file changes to include this 
#     *-line in the future.
#
#   Copyright 2014 Addy Gronquist


import sys

if (len(sys.argv) < 4 ):
    print "Usage:  asciiPCinsert.py   <input.ascii> <input.lst> <output.PCascii>\n\n"
    exit(-1)


try:
    inAsciiFile = open(sys.argv[1], 'r')
except:
    print "Problem opening input .ascii file: " + str(sys.argv[1]) + ", exiting...\n"
    exit(-2)    
inAscii = [line for line in inAsciiFile]    

try:
    inLstFile = open(sys.argv[2], 'r')
except:
    print "Problem opening input .lst file: " + str(sys.argv[2]) + ", exiting...\n"
    exit(-3)    
inLst = [line for line in inLstFile]    


# Scan through the input .lst file to see if there is a "START:" label, and if there is, grab the address on the next line
#    we predicate the correct operation of the program upon: 
#       A) there is a START: label   
#       B) the START: label is on a line of its own immediately before the instruction we wish to load as the initial PC 
#    in this scenario, we can always read in the second whitespace seperated string of the following line as an octal value 
#      and use this as the initial PC 

foundSTART = 0  # flag to see when we find the START tag
foundPC    = 0  # flag for a valid PC found
initialPC  = 0  # initial address for PC

for line in inLst:
    if foundSTART:
        initialPC = line.strip().split(" ")[1]
        print "found initial PC value = " + str(initialPC)
        # make sure this is a 6 digit octal value between 000000 and 177777:
        initialPCoct = -1
        try:
            initialPCint = int(initialPC,8)  # the second argument converts from octal, otherwise it will throw an exception
            if ( ( initialPCint >= 0 ) and ( initialPCint <= 65535 ) ):
                foundPC = 1
            else:
                print "Error: initial PC found (second value on line) is not within the 64k memory address range of a PDP-11/20...?"
                print "  on line: " + str(line)
                exit(-4)
        except:
            print "Error: initial PC found (second value on the line) is not a valid octal/numeric value: " + str(line)
            exit(-5)
        break
    if (line.find("START:") != -1):
        foundSTART = 1
        print "found START: " + str(line)
        continue



# make sure we found a valid initial PC value in the .lst file
if ( not (foundSTART and foundPC)):
    print "Error 405: no 'START:' tag found in the input .lst file, exiting..."
    exit(-6)

# now we just have to check that there are no *-lines in the input .ascii file:

suppressPCoutput = 0
foundStar = 0
for line in inAscii:
    if ( line.find("*") == 0 ):
        foundStar = foundStar + 1

if (foundStar > 0):
    print "Error 417: Found existing *-lines in the input .ascii file, just copying the the input .ascii file to the output .PCascii file"
    suppressPCoutput = 1
    


# Now that we have all the inputs verified as needing of an output file, we can open it on the filesystem.
try:
    outPCascii = open(sys.argv[3], 'w')
except:
    print "Problem opening output .PCascii file: " + str(sys.argv[3]) + ", exiting...\n"
    exit(-4)    

if (not suppressPCoutput):
    # write out the new *-line to the output .PCascii file
    print "writing *line header"
    outPCascii.write("*" + initialPC + "\n") 
    
for line in inAscii:
    outPCascii.write(line)    


inAsciiFile.close()
inLstFile.close()
outPCascii.close()    
print ".PCascii output successfully written"

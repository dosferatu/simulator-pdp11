/* This file contains the simulator as a whole.  It executes and manages all aspects of the
 *  PDP-11/20 simulator.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include "memory.h"

/*
 * Create data structures to simulate PDP-11 memory and registers.
 * Addresses between 0 and 400 are reserved for interrupt and trap vectors.
 * Addresses between 400 and 157777 is for processor stack and programs/data.
 * Addresses above 160000 are reserved for peripheral device status, control, and data regs.
 *
 * System is word (16-bit) or byte (8-bit) addressable.
 * 32768 total words (or 65536) total bytes are available.
 */

// Architecture modules
Memory *memory;
// CPU MODULE FOR FETCH DECODE EXECUTE & STATE CHANGES

/*
 * Logistical data structures used by the simulator
 */
std::vector<std::string> *instruction;
std::vector<std::string> *source;
std::fstream *macFile;

/*
 * Internal state buffers.
 * These fields are used to hold everything about the current
 * state of the PDP11/20 during program execution.
 */
// *** MOSTLY PLACEHOLDERS FOR NOW -- UPDATE WITH PROPER TYPES
int addressPointer;
int programCounter;
int flags;


// Change so we just call the simulator with the .ascii file as the argument for it to parse and run
int main(int argc, char **argv)
{
  // Support only one .ascii file
  if (argc != 2)
  {
    std::cout << "Usage: simulator <ascii file>" << std::endl;
    return 0;
  }

  /*
   * Ideally would perform validation on input name and check to see if the file exists
   * so we can print the appropriate error if something goes wrong. This isn't critical
   * right now so it isn't implemented.
   */
  memory = new Memory();

  // Parse in .ascii file and retrieve instructions until EOF
  macFile = new std::fstream();

  try
  {
    macFile->open(argv[1]);
    source = new std::vector<std::string>;
    std::string buffer;

    // Read in a line and store it in to our instruction source vector
    while (!macFile->eof())
    {
      std::getline(*macFile, buffer);
      source->push_back(buffer.c_str());
    }

    // File IO is finished, so close the file
    macFile->close();
  }

  catch (const std::ios_base::failure &e)
  {
    // Implement file not found, access denied, and !success eventually

    // Ghetto file not found version
    std::cout << "Error parsing Macro11 assembler source file!" << std::endl;
    return 0;
  }

  // First remove the last item in the vector that contains only a '\n' character
  source->pop_back();

  // How do we figure out relative vs absolute addressing mode?

  // Make sure each line starts with a - or @ and only has numbers following
  for (std::vector<std::string>::iterator it = source->begin(); it != source->end(); ++it)
  {
    switch(it->c_str()[0])
    {
      case '@': // This is the line we set our address pointer to
        {
          // Shift addressPointer left 3 bits, then add the next octal value
          for (std::string::iterator i = (it->begin() + 1); i != it->end(); ++i)
          {
            addressPointer = 0;
            addressPointer = addressPointer << 3;
            addressPointer = addressPointer + (*i - '0'); // Convert the ascii number to it's integer equivalent.
          }

          break;
        }

      case '-': // This means we load this value in to the memory location pointed to by addressPointer
        {
          // Use address pointer to write to mem, then increment address pointer.
          // If the value loaded in is 16 bits then incremenent address pointer by 2.
          int value = 0;

          for (std::string::iterator i = (it->begin() + 1); i != it->end(); ++i)
          {
            value = value << 3;
            value = value + (*i - '0'); // Convert the ascii number to it's integer equivalent.
          }

          memory->Write(addressPointer, value);
          break;
        }

      case '*': // This is the value we set the PC to
        {

          break;
        }

      default:
        {
          std::cout << "Unrecognized instruction at line " << it - source->begin() << std::endl;
          return 0;
        }
    }
  }

  /*
   * Now that preliminary validation is complete we will break each instruction down in to
   * PDP11 state changes; if any illegal instructions are detected during this stage the
   * simulator will error out with line number and instruction detail.
   */

  // Run the simulator using the instruction vector
  // CPU module should perform fetch, decode, execute, and then cause any state changes.

  // Just to verify that the Macro11 source file(s) have been parsed correctly.
  //for (std::vector<std::string>::iterator it = source->begin(); it != source->end(); ++it)
  //{
    //std::cout << it->c_str() << std::endl;
  //}
  

  
  // Garbage collection
  if (instruction)
  {
    delete instruction;
  }

  if (source)
  {
    delete source;
  }

  if (macFile)
  {
    delete macFile;
  }

  if (memory)
  {
    delete memory;
  }
}

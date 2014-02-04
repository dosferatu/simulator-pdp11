/* This file contains the simulator as a whole.  It executes and manages all aspects of the
 *  PDP-11/20 simulator.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include "cpu.h"

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
CPU *cpu; 

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
//int programCounter;
//int flags;

// If signed addresses are encountered then relative, otherwise mode is absolute.
enum AddressMode
{
  relative,
  absolute
};


/**************************************************************************************************
 *
 * BEGIN MAIN
 *
 *************************************************************************************************/
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
  // Remove the last item in the vector that contains only a '\n' character
  source->pop_back();

  memory = new Memory(source);
  cpu = new CPU(memory);

  // Loop the CPU which will handle state changes internally.
  //while (true)
  //{
    //cpu->FDE();
  //}

  // Garbage collection
  if (cpu)
  {
    delete cpu;
  }

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
}

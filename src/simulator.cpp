#include <fstream>
#include <iostream>
#include <vector>
#include "cpu.h"

/******************************************************************************
 *
 *                            PDP 11/20 SIMULATOR
 *
 *****************************************************************************/
// Architecture modules
Memory *memory;
CPU *cpu; 

/*
 * Logistical data structures used by the simulator
 */
std::vector<std::string> *source;
std::fstream *macFile;

/******************************************************************************
 *
 *                                BEGIN MAIN
 *
 *****************************************************************************/
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
  // Need to make sure program halting is handled in CPU.
  int status = 0;
  char cont;
  do
  {
    status = cpu->FDE();
    
    if (status == -1)
    { 
      std::cout << "PDP 11/20 received HALT instruction\n" << std::endl;
      
      /* The HALT results in a process halt but can be resumed after the user
       *  presses continue on the console.  In this case we are using the
       *  enter key to denote the continue key on the console.
       */
      std::cout << "Press Enter to continue\n" << std::endl;
      std::cin >> cont;
      status = 0;  // Reset status to allow process to continue.
    }
  } while (status >= 0);


  // Garbage collection
  delete cpu;
  delete source;
  delete macFile;

  return 0;
}

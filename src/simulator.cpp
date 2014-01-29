#include <iostream>
#include <fstream>
#include <vector>

/*
 * Create data structures to simulate PDP-11 memory and registers
 */
int RAM[177777];

/*
 * Logistical data structures used by the simulator
 */
std::vector<std::string> *instruction;
std::vector<std::string> *source;
std::fstream *macFile;

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

  // Perform verification before performing simulator logic
  
  // First remove the last item in the vector that contains only a '\n' character
  source->pop_back();

  // Make sure each line starts with a - or @ and only has numbers following
  for (std::vector<std::string>::iterator it = source->begin(); it != source->end(); ++it)
  {
    // Quit if the first character is unrecognized as an address load, store, or PC set
    if (it->c_str()[0] != '@' && it->c_str()[0] != '*' && it->c_str()[0] != '-')
    {
      std::cout << "Unrecognized instruction at line " << it - source->begin() << std::endl;
      return 0;
    }

    // Quit if a non-numeric character is encountered after the first character
    else
    {
      for (std::string::iterator i = it->begin() + 1; i != it->end(); ++i)
      {
        if (!std::isdigit(*i))
        {
          std::cout << "Unrecognized instruction at line " << it - source->begin() << std::endl;
          return 0;
        }
      }
    }
  }

  /*
   * Now that preliminary validation is complete we will break each instruction down in to
   * PDP11 state changes; if any illegal instructions are detected during this stage the
   * simulator will error out with line number and instruction detail.
   */

  // Begin the loop for instruction break down.
  for (std::vector<std::string>::iterator it = source->begin(); it != source->end(); ++it)
  {
  }

  // Run the simulator using the instruction vector

  // Just to verify that the Macro11 source file(s) have been parsed correctly.
  for (std::vector<std::string>::iterator it = source->begin(); it != source->end(); ++it)
  {
    std::cout << it->c_str() << std::endl;
  }

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
}

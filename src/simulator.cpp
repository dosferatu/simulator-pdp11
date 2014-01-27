#include <iostream>
#include <fstream>
#include <vector>

/*
 * Create data structures to simulate PDP-11 memory and registers
 */
std::vector<std::string> *source;
std::fstream *Imac;

int main()
{
  // Parse in .ascii file and retrieve instructions until EOF
  Imac = new std::fstream();

  try
  {
    Imac->open("main.ascii");
    source = new std::vector<std::string>;
    std::string buffer;

    // Read in a line and store it in to our instruction source vector
    while (!Imac->eof())
    {
      std::getline(*Imac, buffer);
      source->push_back(buffer.c_str());
    }

    // File IO is finished, so close the file
    Imac->close();
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
    // Quit if the first character is unrecognized
    if (it->c_str()[0] != '@' && it->c_str()[0] != '-')
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

  // Just to verify that the Macro11 source file(s) have been parsed correctly.
  //for (std::vector<std::string>::iterator it = source->begin(); it != source->end(); ++it)
  //{
    //std::cout << it->c_str() << std::endl;
  //}

  // Garbage collection
  if (source)
  {
    delete source;
  }

  if (Imac)
  {
    delete Imac;
  }
}

#include <iostream>
#include "memory.h"

Memory::Memory()
{
  RAM = new char[65536];

  // Open trace file for output
  traceFile = new std::fstream();
  try
  {
    traceFile->open("trace.txt", std::ios::out);
  }

  catch (const std::ios_base::failure &e)
  {
    std::cout << "Error opening trace file for output mode!" << std::endl;
  }
}

// Initialize memory with the obj2ascii translated MACRO11 assembly source instructions.
Memory::Memory(std::vector<std::string> *source)
{
  RAM = new char[65536];
  short addressIndex;

  // Open trace file for output
  traceFile = new std::fstream();
  try
  {
    traceFile->open("trace.txt", std::ios::out);
  }

  catch (const std::ios_base::failure &e)
  {
    std::cout << "Error opening trace file for output mode!" << std::endl;
  }

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
            addressIndex = 0;
            addressIndex = addressIndex << 3;
            addressIndex = addressIndex + (*i - '0'); // Convert the ascii number to it's integer equivalent.
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

          // Update internal memory
          this->RAM[addressIndex] = value & 0xFF;
          this->RAM[addressIndex + 1] = value >> 8;
          break;
        }

      case '*': // This is the value we set the PC to
        {

          break;
        }

      default:
        {
          std::cout << "Unrecognized instruction at line " << it - source->begin() << std::endl;
        }
    }
  }
}

Memory::~Memory()
{
  traceFile->close();
  delete [] RAM;
  delete traceFile;
}

short Memory::Read(short effectiveAddress)
{
  // Read both bytes from memory, and return the combined value

  // STUB *** TRACE FILE READ
  // 0 <address>

  return (RAM[effectiveAddress + 1] >> 8) + (RAM[effectiveAddress] & 0xFF);
}

short Memory::ReadInstruction(short effectiveAddress)
{
  // Read both bytes from memory, and return the combined value

  // STUB *** TRACE FILE INSTRUCTION READ
  // 2 <address>

  return (RAM[effectiveAddress + 1] >> 8) + (RAM[effectiveAddress] & 0xFF);
}

void Memory::Write(short effectiveAddress, short data)
{
  // Write the data to the specified memory address
  RAM[effectiveAddress] = data & 0xFF;
  RAM[effectiveAddress + 1] = data >> 8;

  // STUB *** TRACE FILE WRITE
  // 1 <address>

  return;
}

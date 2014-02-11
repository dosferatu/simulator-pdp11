#include <iostream>
#include "memory.h"

Memory::Memory()
{
  this->RAM = new char[65536];

  // Open trace file for output
  try
  {
    traceFile = new std::ofstream("trace.txt", std::ios::out | std::ios::app);
  }

  catch (const std::ios_base::failure &e)
  {
    std::cout << "Error opening trace file for output mode!" << std::endl;
  }
}

// Initialize memory with the obj2ascii translated MACRO11 assembly source instructions.
Memory::Memory(std::vector<std::string> *source)
{
  this->RAM = new char[65536];
  short addressIndex;

  try
  {
    traceFile = new std::ofstream("trace.txt", std::ios::out | std::ios::app);
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
          this->Write(addressIndex, value);
          addressIndex = addressIndex + 2;
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
  this->traceFile->close();
  delete [] RAM;
  delete traceFile;
}

short Memory::Read(int effectiveAddress)
{
  // Trace file output
  std::string buffer = "0 <address>\n";
  *traceFile << buffer;

  // Read both bytes from memory, and return the combined value
  return (this->RAM[effectiveAddress + 1] << 8) + (this->RAM[effectiveAddress] & 0xFF);
}

short Memory::ReadInstruction(int effectiveAddress)
{
  // Trace file output
  std::string buffer = "2 <address>\n";
  *traceFile << buffer;

  // Read both bytes from memory, and return the combined value
  return (this->RAM[effectiveAddress + 1] << 8) + (this->RAM[effectiveAddress] & 0xFF);
}

void Memory::Write(int effectiveAddress, short data)
{
  // Write the data to the specified memory address
  this->RAM[effectiveAddress] = data & 0xFF;
  this->RAM[effectiveAddress + 1] = data >> 8;

  // Trace file output
  std::string buffer = "1 <address>\n";
  *traceFile << buffer;

  return;
}

void Memory::SetDebugMode()
{
  this->debugLevel = Verbosity::minimal;
  return;
}

void Memory::ClearDebugMode()
{
  this->debugLevel = Verbosity::off;
  return;
}

short Memory::StackPop()
{
  return 0;
}

void Memory::StackPush(int _register)
{
  // Translate register number to the appropriate address
  switch(_register)
  {
    case 0:
      {
        _register = R0;
        break;
      }

    case 1:
      {
        _register = R1;
        break;
      }

    case 2:
      {
        _register = R2;
        break;
      }

    case 3:
      {
        _register = R3;
        break;
      }

    case 4:
      {
        _register = R4;
        break;
      }

    case 5:
      {
        _register = R5;
        break;
      }

    case 6:
      {
        _register = SP;
        break;
      }

    case 7:
      {
        _register = PC;
        break;
      }

    default:
      break;
  }

  /*
   * Check if stack pointer has exceeded it's limit.
   * If it has then we need to crash and burn.
   * Limit is 0160000
   */
  //if (this->Read(SP) < 0160000)
  //{
    //short sp = this->Read(SP);
    //sp += 02;
    //this->Write(sp, _register);
    //this->Write(SP, this->Read(sp));
  //}

  return;
}

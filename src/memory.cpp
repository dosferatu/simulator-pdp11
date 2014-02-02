#include "memory.h"

Memory::Memory()
{
  RAM = new short[32768];
}

Memory::~Memory()
{
  delete RAM;
}

short Memory::Read(short effectiveAddress)
{
  // Read both bytes from memory, and return the combined value
  
  // STUB *** TRACE FILE READ

  return (RAM[effectiveAddress + 1] >> 8) + (RAM[effectiveAddress] & 0xFF);
}

void Memory::Write(short effectiveAddress, short data)
{
  // Write the data to the specified memory address
  RAM[effectiveAddress] = data & 0xFF;
  RAM[effectiveAddress + 1] = data >> 8;

  // STUB *** TRACE FILE WRITE
  
  return;
}

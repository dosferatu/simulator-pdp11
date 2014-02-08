#include <iostream>
#include "cpu.h"

CPU::CPU()
{
  this->memory = new Memory();
}

CPU::CPU(Memory *memory)
{
  this->memory = memory;
  this->reg[PC] = 010;
}

CPU::~CPU()
{
  delete memory;
}

/*
 * Takes in the program counter register value in order to
 * be able to fetch, decode, and execute the next instruction
 * .
 */ 
int CPU::FDE()
{
  /*
   * BEGIN INSTRUCTION FETCH
   */
  // -(SP) pushes, +(SP) pops, X(SP) index accesses
  short instruction = memory->ReadInstruction(this->reg[PC]);
  this->reg[PC] += 2;

  /*
   * BEGIN INSTRUCTION DECODE
   */
  int mode = instruction & 070;
  int reg = instruction & 07;
  int instructionWord = instruction & 0177700;
  std::cout << "Instruction fetched: " << instruction << std::endl;
  std::cout << "Mode: " << mode << std::endl;
  std::cout << "Register: " << reg << std::endl;
  std::cout << "Instruction word: " << instructionWord << std::endl;

  /*
   * Register addressing modes
   * 
   * Mode 0 - General Register
   * Mode 1 - Deferred Register
   * Mode 6 - Indexed
   * Mode 7 - Deferred Indexed
   * Mode 2 - Autoincrement
   * Mode 3 - Autoincrement Deferred
   * Mode 4 - Autodecrement
   * Mode 5 - Autodecrement Deferred
   */

  /*
   * BEGIN INSTRUCTION EXECUTE
   */
  // Temporary locator for HALT condition
  if (instruction == 0)
    return -1;

  return 0;
}

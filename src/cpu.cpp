#include <iostream>
#include "cpu.h"

CPU::CPU()
{
  this->memory = new Memory();
  this->reg[0] = R0;
  this->reg[1] = R1;
  this->reg[2] = R2;
  this->reg[3] = R3;
  this->reg[4] = R4;
  this->reg[5] = R5;
  this->reg[6] = SP;
  this->reg[7] = PC;
}

CPU::CPU(Memory *memory)
{
  this->memory = memory;
  this->reg[0] = R0;
  this->reg[1] = R1;
  this->reg[2] = R2;
  this->reg[3] = R3;
  this->reg[4] = R4;
  this->reg[5] = R5;
  this->reg[6] = SP;
  this->reg[7] = PC;
  memory->Write(PC, 010);
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
   *
   */

  // Retrieve the PC value
  short pc = memory->Read(PC);

  // -(SP) pushes, +(SP) pops, X(SP) index accesses
  short instruction = memory->ReadInstruction(pc);
  pc += 2;

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
   * Perform instruction decode
   * Use addressing modes when necessary
   */

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
   * PC addressing modes
   *
   * Mode 2 - Immediate
   * Mode 3 - Absolute
   * Mode 6 - Relative
   * Mode 7 - Relative Deferred
   */

  /*
   * BEGIN INSTRUCTION EXECUTE
   */
  
  // Write the new PC value
  memory->Write(PC, pc);
  // Temporary locator for HALT condition
  if (instruction == 0)
    return -1;

  return 0;
}

/*
 * PC addressing modes
 *
 * Mode 2 - Immediate
 * Mode 3 - Absolute
 * Mode 6 - Relative
 * Mode 7 - Relative Deferred
 */

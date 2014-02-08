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
  std::cout << "Instruction fetched: " << instruction << std::endl;

  /*
   * BEGIN INSTRUCTION EXECUTE
   */
  // Temporary locator for HALT condition
  if (instruction == 0)
    return -1;

  return 0;
}

#include "cpu.h"

CPU::CPU()
{
  memory = new Memory();
}

CPU::CPU(Memory *memory)
{
  this->memory = memory;
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
void CPU::FDE()
{
  /*
   * BEGIN INSTRUCTION FETCH
   */

  /*
   * BEGIN INSTRUCTION DECODE
   */

  /*
   * BEGIN INSTRUCTION EXECUTE
   */

  return;
}

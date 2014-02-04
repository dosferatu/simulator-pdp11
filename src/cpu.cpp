#include <iostream>
#include "cpu.h"

CPU::CPU()
{
  this->memory = new Memory();
}

CPU::CPU(Memory *memory)
{
  this->memory = memory;
  this->PC = 010;
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
  this->SP = memory->ReadInstruction(this->PC);
  this->PC += 2;
  std::cout << this->SP << std::endl;

  /*
   * BEGIN INSTRUCTION DECODE
   */

  /*
   * BEGIN INSTRUCTION EXECUTE
   */

  return;
}

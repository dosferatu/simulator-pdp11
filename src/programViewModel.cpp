#include <iostream>
#include "programViewModel.h"

// Constructors/*{{{*/
programViewModel::programViewModel(QObject *parent) :
  QObject(parent)
{
  this->breakPoints = new std::vector<unsigned short>();
  this->currentInstruction = -1;
}

programViewModel::programViewModel(CPU *cpu, Memory *memory, QObject *parent) :
  QObject(parent)
{
  this->breakPoints = new std::vector<unsigned short>();
  this->cpu = cpu;
  this->currentInstruction = -1;
  this->memory = memory;
}/*}}}*/

// Destructor/*{{{*/
programViewModel::~programViewModel()
{
  delete breakPoints;
}/*}}}*/

// Program execution functions/*{{{*/

// Continue until HALT or break point/*{{{*/
void programViewModel::continueExecution()
{
  std::cout << "Continue!" << std::endl;
  int status = 0;

  // Run until HALT or break point/*{{{*/
  do
  {
    this->currentInstruction = this->memory->RetrievePC();
    status = cpu->FDE();
    this->memory->IncrementPC();

  } while (status > 0 && this->currentInstruction != this->nextBreak);/*}}}*/

  // Report exit status/*{{{*/
  if (this->currentInstruction % 2 != 0)
  {
    std::cout << "Warning: program counter is not an even number!" << std::endl;
  }

  if (status == 0)
  {
    std::cout << "PDP 11/20 received HALT instruction\n" << std::endl;

    /* The HALT results in a process halt but can be resumed after the user
     *  presses continue on the console.  In this case we are using the
     *  enter key to denote the continue key on the console.
     */
    //std::cout << "Press Enter to continue\n" << std::endl;
    //std::cin.get();
    //status = 0;  // Reset status to allow process to continue.
  }

  else if (this->currentInstruction == this->nextBreak)
  {
    std::cout << "Break point encountered!\n" << std::endl;
  }/*}}}*/
}/*}}}*/

// Run program command/*{{{*/
void programViewModel::run()
{
  /*
   * If current instr > 0 then prompt for confirmation
   * to restart running program.
   */

  std::cout << "Run!" << std::endl;
  int status = 0;
  this->memory->ResetPC();

  // Run until HALT or break point/*{{{*/
  do
  {
    this->currentInstruction = this->memory->RetrievePC();
    status = cpu->FDE();
    this->memory->IncrementPC();

  } while (status > 0 && this->currentInstruction != this->nextBreak);/*}}}*/

  // Report exit status/*{{{*/
  if (this->currentInstruction % 2 != 0)
  {
    std::cout << "Warning: program counter is not an even number!" << std::endl;
  }

  if (status == 0)
  {
    std::cout << "PDP 11/20 received HALT instruction\n" << std::endl;

    /* The HALT results in a process halt but can be resumed after the user
     *  presses continue on the console.  In this case we are using the
     *  enter key to denote the continue key on the console.
     */
    //std::cout << "Press Enter to continue\n" << std::endl;
    //std::cin.get();
    //status = 0;  // Reset status to allow process to continue.
  }

  else if (this->currentInstruction == this->nextBreak)
  {
    std::cout << "Break point encountered!\n" << std::endl;
  }/*}}}*/
}/*}}}*/

// Step in to a single instruction/*{{{*/
void programViewModel::step()
{
  std::cout << "Step!" << std::endl;
  int status = 0;

  this->currentInstruction = this->memory->RetrievePC();
  status = cpu->FDE();
  this->memory->IncrementPC();

  // Report exit status/*{{{*/
  if (this->currentInstruction % 2 != 0)
  {
    std::cout << "Warning: program counter is not an even number!" << std::endl;
  }

  if (status == 0)
  {
    std::cout << "PDP 11/20 received HALT instruction\n" << std::endl;

    /* The HALT results in a process halt but can be resumed after the user
     *  presses continue on the console.  In this case we are using the
     *  enter key to denote the continue key on the console.
     */
    //std::cout << "Press Enter to continue\n" << std::endl;
    //std::cin.get();
    //status = 0;  // Reset status to allow process to continue.
  }

  else if (this->currentInstruction == this->nextBreak)
  {
    std::cout << "Break point encountered!\n" << std::endl;
  }/*}}}*/
}/*}}}*/

// Stop simulation/*{{{*/
void programViewModel::stop()
{
  std::cout << "Stop!" << std::endl;
  this->memory->ResetPC();
  // Reset state of CPU simulator here
}/*}}}*/
/*}}}*/

// Break point functions/*{{{*/
void programViewModel::setBreak()
{
}

void programViewModel::clearBreak()
{
}

void programViewModel::clearAllBreaks()
{
}/*}}}*/

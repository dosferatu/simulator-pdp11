#ifndef CPU_H
#define CPU_H

#include "memory.h"

class CPU
{
  public:
    CPU(Memory *memory);
    ~CPU();
    short EA(short encodedAddress);
    int FDE();
    void SetDebugMode(Verbosity verbosity);
    void ResetInstructionCount();

  private:
    int debugLevel;             // Debug verbosity level
    unsigned long long instructionCount;       // Statistics
    Memory *memory;             // RAM
    short reg[9];               // General-purpose registers
                                // R6 is the processor stack pointer
                                // R7 is the program counter
                                // R8 is the Processor Status Register
};
#endif // CPU_H

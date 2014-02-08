#include <vector>
#include "memory.h"

#define SP 6
#define PC 7

class CPU
{
  public:
    CPU();
    CPU(Memory *memory);
    ~CPU();
    int FDE();

  private:
    Memory *memory;             // RAM
    short PS;                   // Process status register
    short reg[8];               // General-purpose registers
                                // R6 is the processor stack pointer
                                // R7 is the program counter
};

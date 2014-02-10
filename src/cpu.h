#include <vector>
#include "memory.h"

#define R0 0177700
#define R1 0177702
#define R2 0177704
#define R3 0177706
#define R4 0177711
#define R5 0177713
#define SP 0177715
#define PC 0177717
#define PS 0177776

class CPU
{
  public:
    CPU();
    CPU(Memory *memory);
    ~CPU();
    short EA(short encodedAddress);
    int FDE();

  private:
    Memory *memory;             // RAM
    short reg[9];               // General-purpose registers
                                // R6 is the processor stack pointer
                                // R7 is the program counter
                                // PS is the Processor Status Register
};

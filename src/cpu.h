#include <vector>
#include "memory.h"

class CPU
{
  public:
    CPU();
    CPU(Memory *memory);
    ~CPU();
    int FDE();

  private:
    Memory *memory;             // RAM
    short reg[6];               // General-purpose registers
    short SP;                   // Stack pointer
    short PS;                   // Process status register
    short PC;                   // Program counter
};

#include <vector>
#include "memory.h"

// Condition codes for arithmetic operations
enum ConditionCode
{
};

class CPU
{
  public:
    CPU();
    CPU(Memory *memory);
    ~CPU();
    void FDE();

  private:
    Memory *memory;             // RAM
    short reg[6];               // General-purpose registers
    short SP;                   // Stack pointer
    short PS;                   // Process status register
    short PC;                   // Program counter
    // Code struct for conditional codes
};

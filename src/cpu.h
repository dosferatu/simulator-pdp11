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
    Memory *memory;
    // Registers available to the CPU
    // Code struct for conditional codes
};

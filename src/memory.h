#include <fstream>
#include <string>
#include <vector>

// Register memory locations
#define R0 0177700
#define R1 0177702
#define R2 0177704
#define R3 0177706
#define R4 0177711
#define R5 0177713
#define SP 0177715
#define PC 0177717
#define PS 0177776

// Debug levels
enum Verbosity
{
  off,
  minimal
};

class Memory
{
  public:
    Memory();
    Memory(std::vector<std::string> *source);
    ~Memory();
    short Read(int effectiveAddress);
    short ReadInstruction(int effectiveAddress);
    void Write(int effectiveAddress, short data);
    void SetDebugMode();
    void ClearDebugMode();
    short StackPop();
    void StackPush(int _register);

  private:
    int debugLevel;
    char *RAM;
    std::ofstream *traceFile;
};

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
  minimal,
  verbose
};

// Transaction types
enum Transaction
{
  read,
  write,
  instruction
};


class Memory
{
  public:
    Memory(std::vector<std::string> *source);
    ~Memory();
    void DecrementPC() { RAM[PC] = RAM[PC] - 2; };
    void IncrementPC() { RAM[PC] = RAM[PC] + 2; };
    unsigned short RetrievePC();
    unsigned short EA(unsigned short encodedAddress, Transaction type = Transaction::read);
    unsigned short Read(unsigned short encodedAddress);
    unsigned short ReadInstruction();
    void Write(unsigned short encodedAddress, unsigned short data);
    void SetDebugMode(Verbosity verbosity) { debugLevel = verbosity; };
    unsigned short StackPop();
    void StackPush(int _register);
    void RegDump();
    void TraceDump(Transaction type, unsigned short address);
    void SetByteMode() { byteMode = 01; };
    void ClearByteMode() { byteMode = 02; };
    unsigned short ReadPS();
    void WritePS(unsigned short status);

  private:
    int byteMode;
    int debugLevel;
    int regArray[8];
    unsigned char *RAM;
    std::ofstream *traceFile;
};

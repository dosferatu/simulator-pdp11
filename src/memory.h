#ifndef MEMORY_H
#define MEMORY_H

#include <fstream>
#include <string>
#include <vector>

// Register memory locations
#define R0 0177700U
#define R1 0177704U
#define R2 0177710U
#define R3 0177714U
#define R4 0177720U
#define R5 0177724U
#define SP 0177730U
#define PC 0177734U
#define PS 0177776U

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
    void StackPush(unsigned short _register);
    void RegDump();
    void TraceDump(Transaction type, unsigned short address);
    void SetByteMode() { byteMode = 01; };
    void ClearByteMode() { byteMode = 02; };
    void ResetPC();
    void ResetRAM();
    unsigned short ReadPS();
    void WritePS(unsigned short status);

  private:
    int byteMode;
    int debugLevel;
    int regArray[8];
    unsigned char *initialRAM;
    unsigned char *RAM;
    unsigned short initialPC;
    std::ofstream *traceFile;
};
#endif // MEMORY_H

#include <fstream>
#include <string>
#include <vector>

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
    short Read(short effectiveAddress);
    short ReadInstruction(short effectiveAddress);
    void Write(short effectiveAddress, short data);
    void SetDebugMode();
    void ClearDebugMode();

  private:
    int debugLevel;
    char *RAM;
    std::fstream *traceFile;
};

#include <fstream>
#include <string>
#include <vector>

class Memory
{
  public:
    Memory();
    Memory(std::vector<std::string> *source);
    ~Memory();
    short Read(short effectiveAddress);
    short ReadInstruction(short effectiveAddress);
    void Write(short effectiveAddress, short data);

  private:
    char *RAM;
    std::fstream *traceFile;
};

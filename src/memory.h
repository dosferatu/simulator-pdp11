#include <string>
#include <vector>

class Memory
{
  public:
    Memory();
    Memory(std::vector<std::string> *source);
    ~Memory();
    short Read(short effectiveaddress);
    short ReadInstruction(short effectiveaddress);
    void Write(short effectiveaddress, short data);

  private:
    char *RAM;
};

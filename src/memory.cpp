#include <iostream>
#include <sstream>
#include "memory.h"

// Initialize memory using the assembly source/*{{{*/
Memory::Memory(std::vector<std::string> *source)
{
  this->RAM = new unsigned char[65536];
  this->byteMode = 02;          // Default to word addressing
  int addressIndex = 0;

  regArray[0] = R0;
  regArray[1] = R1;
  regArray[2] = R2;
  regArray[3] = R3;
  regArray[4] = R4;
  regArray[5] = R5;
  regArray[6] = SP;
  regArray[7] = PC;

  try
  {
    traceFile = new std::ofstream("trace.txt", std::ios::out);
  }

  catch (const std::ios_base::failure &e)
  {
    std::cout << "Error opening trace file for output mode!" << std::endl;
  }

  // Make sure each line starts with a - or @ and only has numbers following
  for (std::vector<std::string>::iterator it = source->begin(); it != source->end(); ++it)
  {
    switch(it->c_str()[0])
    {
      case '@': // This is the line we set our address pointer to
        {
          // Shift addressPointer left 3 bits, then add the next octal value
          for (std::string::iterator i = (it->begin() + 1); i != it->end(); ++i)
          {
            addressIndex = 0;
            addressIndex = addressIndex << 3;
            addressIndex = addressIndex + (*i - '0'); // Convert the ascii number to it's integer equivalent.
          }

          break;
        }

      case '-': // This means we load this value in to the memory location pointed to by addressPointer
        {
          // Use address pointer to write to mem, then increment address pointer.
          // If the value loaded in is 16 bits then incremenent address pointer by 2.
          int value;

          for (std::string::iterator i = (it->begin() + 1); i != it->end(); ++i)
          {
            value = value << 3;
            value = value + (*i - '0'); // Convert the ascii number to it's integer equivalent.
          }

          // Update internal memory directly to avoid trace output
          this->RAM[addressIndex++] = value & 0xFF;
          this->RAM[addressIndex++] = value >> 8;
          break;
        }

      case '*': // This is the value we set the PC to
        {
          int value;

          for (std::string::iterator i = (it->begin() + 1); i != it->end(); ++i)
          {
            value = value << 3;
            value = value + (*i - '0'); // Convert the ascii number to it's integer equivalent.
          }

          // Update internal memory directly to avoid trace output
          this->RAM[PC] = value & 0xFF;
          this->RAM[PC + 1] = value >> 8;
          break;
        }

      default:
        {
          std::cout << "Unrecognized instruction at line " << it - source->begin() << std::endl;
        }
    }
  }
}
/*}}}*/

Memory::~Memory()/*{{{*/
{
  this->traceFile->close();
  delete [] RAM;
  delete traceFile;
}
/*}}}*/

unsigned short Memory::RetrievePC()/*{{{*/
{
  return (this->RAM[PC + 1] << 8) + (this->RAM[PC] & 0xFF);
}
/*}}}*/

unsigned short Memory::EA(unsigned short encodedAddress)/*{{{*/
{
  unsigned short mode = (encodedAddress & 070) >> 3;
  unsigned short reg = (encodedAddress & 07);
  unsigned short decodedAddress = 0;
  std::string modeType = "Not Set!";

  switch(mode)
  {
    case 0: // General Register
      {
        modeType = "General Register";

        decodedAddress = regArray[reg];
        break;
      }

    case 1: // Deferred Register
      {
        modeType = "Deferred Register";

        decodedAddress = this->RAM[regArray[reg]];
        this->TraceDump(Transaction::read, decodedAddress);
        break;
      }

    case 2: // Autoincrement
      {
        // Check for immediate PC addressing
        if (reg == 07)
        {
          modeType = "Immediate PC";

          // Point to the word after the instruction word
          decodedAddress = this->RetrievePC();
          this->IncrementPC();
        }

        else
        {
          /*
           * Retrieve the memory address from encodedAddress
           * and then increment the pointer stored in encodedAddress
           */
          modeType = "Autoincrement";

          decodedAddress = this->RAM[regArray[reg]];
          this->TraceDump(Transaction::read, decodedAddress);
          this->RAM[regArray[reg]] = decodedAddress + byteMode;
          this->TraceDump(Transaction::write, regArray[reg]);
        }

        break;
      }

    case 3: // Autoincrement Deferred
      {
        // Check for absolute PC addressing
        if (reg == 07)
        {
          modeType = "Absolute PC";

          decodedAddress = this->RAM[this->RetrievePC()];
          this->TraceDump(Transaction::read, decodedAddress);
          this->IncrementPC();
        }

        else
        {
          modeType = "Autoincrement Deferred";

          unsigned short address = this->RAM[regArray[reg]];
          this->TraceDump(Transaction::read, address);
          decodedAddress = this->RAM[address];
          this->TraceDump(Transaction::read, decodedAddress);
          this->RAM[regArray[reg]] = address + byteMode;
          this->TraceDump(Transaction::write, regArray[reg]);
        }

        break;
      }

    case 4: // Autodecrement
      {
        modeType = "Autodecrement Deferred";

        /*
         * decrement address
         * then return value
         */
        //decodedAddress = this->Read(regArray[reg]);
        //this->Write(regArray[reg], decodedAddress + byteMode);
        break;
      }

    case 5: // Autodecrement Deferred
      {
        modeType = "Autodecrement Deferred";

        // Decrement Rn, and return the address in Rn
        this->RAM[regArray[reg]] = this->RAM[regArray[reg]] - byteMode;
        unsigned short address = this->RAM[regArray[reg]];
        decodedAddress = this->RAM[address];
        this->TraceDump(Transaction::read, decodedAddress);
        break;
      }

    case 6: // Indexed
      {
        // Check for relative PC addressing
        if (reg == 07)
        {
          modeType = "Relative PC";

          decodedAddress = this->RAM[this->RetrievePC()];
          this->IncrementPC();
        }

        else
        {
          modeType = "Indexed";

          // Retrieve the index offset from memory
          unsigned short base = this->RAM[this->RetrievePC()];
          this->IncrementPC();
          decodedAddress = base + this->RAM[this->RetrievePC()];
        }

        break;
      }

    case 7: // Deferred Indexed
      {
        // Check for deferred relative PC addressing
        if (reg == 07)
        {
          modeType = "Deferred Relative PC";

          unsigned short address = this->RAM[this->RetrievePC()];
          decodedAddress = this->RAM[address];
          this->IncrementPC();
        }

        else
        {
          modeType = "Deferred Indexed";

          /*
           * The address is the some of the contents pointed to by the
           * operand plus the specified offset which is the word following
           * the instruction
           */
          unsigned short address = this->RAM[this->RetrievePC()];
          unsigned short base = this->RAM[address];
          this->IncrementPC();
          decodedAddress = base + this->RAM[this->RetrievePC()];
        }

        break;
      }

    default:
      break;
  }

  if (this->debugLevel == Verbosity::verbose)
  {
    //std::cout << "Mode Type: " << modeType << "(" << static_cast<int>(mode) << ")" << std::endl;
    //std::cout << "Register: " << static_cast<int>(reg) << std::endl;
  }

  return decodedAddress;
}/*}}}*/

unsigned short Memory::Read(unsigned short encodedAddress)/*{{{*/
{
  unsigned short address = this->EA(encodedAddress);

  // If not a general register operand then do a trace dump
  if (!(address > R0 && address < PC))
  {
    this->TraceDump(Transaction::read, address);
  }

  /*
   * Read either a byte or a word from memory depending on the value
   * of the internal byteMode flag.
   */
  if (this->byteMode)
  {
    return this->RAM[address];
  }

  else
  {
    return (this->RAM[address + 1] << 8) + (this->RAM[address] & 0xFF);
  }
}
/*}}}*/

unsigned short Memory::ReadInstruction()/*{{{*/
{
  unsigned short address = this->RetrievePC();
  // Trace file output
  this->TraceDump(Transaction::instruction, this->RetrievePC());
  return (this->RAM[address + 1] << 8) + (this->RAM[address] & 0xFF);
}
/*}}}*/

void Memory::Write(unsigned short encodedAddress, unsigned short data)/*{{{*/
{
  unsigned short address = this->EA(encodedAddress);

  // If not a general register operand then do a trace dump
  if (!(address > R0 && address < PC))
  {
    this->TraceDump(Transaction::write, address);
  }

  // Write the data to the specified memory address
  if (this->byteMode)
  {
    this->RAM[address] = data & 0xFF;
  }

  else
  {
    this->RAM[address] = data & 0xFF;
    this->RAM[address + 1] = data >> 8;
  }

  return;
}
/*}}}*/

unsigned short Memory::StackPop()/*{{{*/
{
  return 0;
}
/*}}}*/

void Memory::StackPush(int _register)/*{{{*/
{
  // Translate register number to the appropriate address
  switch(_register)
  {
    case 0:
      {
        _register = R0;
        break;
      }

    case 1:
      {
        _register = R1;
        break;
      }

    case 2:
      {
        _register = R2;
        break;
      }

    case 3:
      {
        _register = R3;
        break;
      }

    case 4:
      {
        _register = R4;
        break;
      }

    case 5:
      {
        _register = R5;
        break;
      }

    case 6:
      {
        _register = SP;
        break;
      }

    case 7:
      {
        _register = PC;
        break;
      }

    default:
      break;
  }

  /*
   * Check if stack pointer has exceeded it's limit.
   * If it has then we need to crash and burn.
   * Limit is 0160000
   */
  //if (this->Read(SP) < 0160000)
  //{
  //short sp = this->Read(SP);
  //sp += byteMode;
  //this->Write(sp, _register);
  //this->Write(SP, this->Read(sp));
  //}

  return;
}
/*}}}*/

void Memory::RegDump()
{
    std::cout << "Dumping current register contents..." << std::endl;
    std::cout << "R0: " << std::oct << static_cast<unsigned short>(this->RAM[R0]) << std::endl;
    std::cout << "R1: " << std::oct << static_cast<unsigned short>(this->RAM[R1]) << std::endl;
    std::cout << "R2: " << std::oct << static_cast<unsigned short>(this->RAM[R2]) << std::endl;
    std::cout << "R3: " << std::oct << static_cast<unsigned short>(this->RAM[R3]) << std::endl;
    std::cout << "R4: " << std::oct << static_cast<unsigned short>(this->RAM[R4]) << std::endl;
    std::cout << "R5: " << std::oct << static_cast<unsigned short>(this->RAM[R5]) << std::endl;
    std::cout << "SP: " << std::oct << static_cast<unsigned short>(this->RAM[SP]) << std::endl;
    std::cout << "PC: " << std::oct << static_cast<unsigned short>(this->RAM[PC]) << std::endl;
    std::cout << std::endl;
    std::cout << "Processor status word: " << std::endl;
    std::cout << "N: " << std::oct << ((static_cast<unsigned short>(this->RAM[PS]) & 0x8) >> 3) << std::endl;
    std::cout << "Z: " << std::oct << ((static_cast<unsigned short>(this->RAM[PS]) & 0x4) >> 2) << std::endl;
    std::cout << "V: " << std::oct << ((static_cast<unsigned short>(this->RAM[PS]) & 0x2) >> 1) << std::endl;
    std::cout << "C: " << std::oct << (static_cast<unsigned short>(this->RAM[PS]) & 0x1)        << std::endl;
  return;
}

void Memory::TraceDump(Transaction type, unsigned short address)/*{{{*/
{
  std::string buffer;
  std::stringstream stream;
  stream << std::oct << type;
  buffer.append(stream.str());
  stream.clear();
  stream << std::oct << address;
  buffer.append(stream.str());
  buffer.append("\n");
  *traceFile << buffer;

  return;
}/*}}}*/

// ReadPS() and WritePS(unsigned short)/*{{{*/
// ReadPS()
unsigned short Memory::ReadPS()
{
  return (this->RAM[PS + 1] << 8) + (this->RAM[PS] & 0xFF);
}

// WritePS()
void Memory::WritePS(unsigned short status)
{
  this->RAM[PS] = status & 0xFF;
  this->RAM[PS + 1] = (status >> 8);
} /*}}}*/

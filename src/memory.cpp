#include <iostream>
#include <sstream>
#include "memory.h"
#include <iomanip>

// Initialize memory using the assembly source/*{{{*/
Memory::Memory(std::vector<std::string> *source)
{
  // Initialize RAM to 0's
  this->RAM = new unsigned char[65536] {0};
  this->initialRAM = new unsigned char[65536] {0};
  this->byteMode = 02;          // Default to word addressing
  unsigned int addressIndex = 0;

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

  // Make sure each line starts with a - or @ and only has numbers following/*{{{*/
  for (std::vector<std::string>::iterator it = source->begin(); it != source->end(); ++it)
  {
    switch(it->c_str()[0])
    {
      case '@': // This is the line we set our address pointer to
        {
          // Shift addressPointer left 3 bits, then add the next octal value
          addressIndex = 0;
          std::string temp;
          
          // Grab the value past the first '@' char
          temp = it->substr(1, it->length() - 1);
          std::stringstream stream(temp);
          stream >> std::oct >> addressIndex;

          break;
        }

      case '-': // This means we load this value in to the memory location pointed to by addressPointer
        {
          // Use address pointer to write to mem, then increment address pointer.
          // If the value loaded in is 16 bits then incremenent address pointer by 2.
          unsigned int value = 0;
          std::string temp;
          
          // Grab the value past the first '-' char
          temp = it->substr(1, it->length() - 1);
          std::stringstream stream(temp);
          stream >> std::oct >> value;

          // Update internal memory directly to avoid trace output
          this->RAM[addressIndex++] = value & 0xFF;
          this->RAM[addressIndex++] = value >> 8;
          break;
        }

      case '*': // This is the value we set the PC to
        {
          unsigned int value = 0;
          std::string temp;
          
          // Grab the value past the first '*' char
          temp = it->substr(1, it->length() - 1);
          std::stringstream stream(temp);
          stream >> std::oct >> value;

          // Update internal memory directly to avoid trace output
          this->RAM[PC] = value & 0xFF;
          this->RAM[PC + 1] = value >> 8;
          this->initialPC = value;
          break;
        }

      default:
        {
          std::cout << "Unrecognized instruction at line " << it - source->begin() << std::endl;
        }
    }
  }/*}}}*/

  // Make a copy of the initial memory state to support GUI restart of program execution
  for (int i = 0; i < 65536; ++i)
  {
    this->initialRAM[i] = this->RAM[i];
  }
}
/*}}}*/

Memory::~Memory()/*{{{*/
{
  this->traceFile->close();
  delete [] RAM;
  delete [] initialRAM;
  delete traceFile;
}
/*}}}*/

void Memory::WriteAddress(unsigned short address, unsigned short data)
{
  this->RAM[address] = data & 0xFF;
  this->RAM[address + 1] = (data >> 8);
  return;
}

unsigned short Memory::ReadAddress(unsigned short address)
{
  return (this->RAM[address + 1] << 8) | (this->RAM[address] & 0xFF);
}

unsigned short Memory::RetrievePC()/*{{{*/
{
  return (this->RAM[PC + 1] << 8) | (this->RAM[PC] & 0xFF);
}
/*}}}*/

unsigned short Memory::EA(unsigned short encodedAddress, Transaction type)/*{{{*/
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

        decodedAddress = (this->RAM[regArray[reg] + 1] << 8) | (this->RAM[regArray[reg]] & 0xFF);
        break;
      }

    case 2: // Autoincrement
      {
        // Check for immediate PC addressing
        if (reg == 07)
        {
          modeType = "Immediate PC";

          if (type == Transaction::read)
          {
            this->IncrementPC();
          }

          // Point to the word after the instruction word
          decodedAddress = this->RetrievePC();
        }

        else
        {
          /*
           * Retrieve the memory address from encodedAddress
           * and then increment the pointer stored in encodedAddress
           */
          modeType = "Autoincrement";

          decodedAddress = (this->RAM[regArray[reg] + 1] << 8) | (this->RAM[regArray[reg]] & 0xFF);
          //this->TraceDump(Transaction::read, decodedAddress);

          unsigned short  incrementedAddress;
          if (regArray[reg] == SP)
          {
            incrementedAddress = decodedAddress + 02;
          }

          else
          {
            incrementedAddress = decodedAddress + byteMode;
          }

          this->RAM[regArray[reg]] = incrementedAddress & 0xFF;
          this->RAM[regArray[reg] + 1] = incrementedAddress >> 8;
        }

        break;
      }

    case 3: // Autoincrement Deferred
      {
        // Check for absolute PC addressing
        if (reg == 07)
        {
          modeType = "Absolute PC";

          if (type == Transaction::read)
          {
            this->IncrementPC();
          }

          unsigned short address = this->RetrievePC();
          decodedAddress = (this->RAM[address + 1] << 8) | (this->RAM[address] & 0xFF);
        }

        else
        {
          modeType = "Autoincrement Deferred";

          // Read in address from reg
          unsigned short address = (this->RAM[regArray[reg] + 1] << 8) | (this->RAM[regArray[reg]] & 0xFF);

          // Read in value from address
          decodedAddress = (this->RAM[address + 1] << 8) | (this->RAM[address] & 0xFF);
          this->TraceDump(Transaction::read, address);

          // Possibly case for byteMode?
          unsigned short incrementedAddress = address + byteMode;
          if (regArray[reg] == SP)
          {
            incrementedAddress = address + 02;
          }

          else
          {
            incrementedAddress = address + byteMode;
          }
          this->RAM[regArray[reg]] = incrementedAddress & 0xFF;
          this->RAM[regArray[reg] + 1] = incrementedAddress >> 8;
        }

        break;
      }

    case 4: // Autodecrement
      {
        modeType = "Autodecrement";

        /*
         * decrement address
         * then return value
         */

        // Possibly case for byteMode?
        unsigned short address = (this->RAM[regArray[reg] + 1] << 8) | (this->RAM[regArray[reg]] & 0xFF);
        if (regArray[reg] == SP)
        {
          decodedAddress = address - 02;
        }

        else
        {
          decodedAddress = address - byteMode;
        }
        this->RAM[regArray[reg]] = decodedAddress & 0xFF;
        this->RAM[regArray[reg] + 1] = decodedAddress >> 8;
        break;
      }

    case 5: // Autodecrement Deferred
      {
        modeType = "Autodecrement Deferred";

        // Decrement Rn, and return the address in Rn
        unsigned short address = (this->RAM[regArray[reg] + 1] << 8) | (this->RAM[regArray[reg]] & 0xFF);
        unsigned short decrementedAddress;
        if (regArray[reg] == SP)
        {
          decrementedAddress = address - 02;
        }

        else
        {
          decrementedAddress = address - byteMode;
        }
        decodedAddress = (this->RAM[decrementedAddress + 1] << 8) | (this->RAM[decrementedAddress] & 0xFF);
        this->RAM[regArray[reg]] = decrementedAddress & 0xFF;
        this->RAM[regArray[reg] + 1] = decrementedAddress >> 8;
        this->TraceDump(Transaction::read, decrementedAddress);
        break;
      }

    case 6: // Indexed
      {
        // Check for relative PC addressing
        if (reg == 07)
        {
          modeType = "Relative PC";

          if (type == Transaction::read)
          {
            this->IncrementPC();
          }

          unsigned short address = this->RetrievePC();
          unsigned short relativeAddress = ((this->RAM[address + 1] << 8) & 0xFF00) | (this->RAM[address] & 0xFF);
          decodedAddress = address + relativeAddress + 02;
        }

        else
        {
          modeType = "Indexed";

          if (type == Transaction::read)
          {
            this->IncrementPC();
          }

          // Retrieve the index offset from memory
          unsigned short base = (this->RAM[regArray[reg] + 1] << 8) | (this->RAM[regArray[reg]] & 0xFF);
          unsigned short offsetAddress = this->RetrievePC();
          unsigned short offset = (this->RAM[offsetAddress + 1] << 8) | (this->RAM[offsetAddress] & 0xFF);
          decodedAddress = offset + base;
        }

        break;
      }

    case 7: // Deferred Indexed
      {
        // Check for deferred relative PC addressing
        if (reg == 07)
        {
          modeType = "Deferred Relative PC";

          if (type == Transaction::read)
          {
            this->IncrementPC();
          }

          unsigned short address = this->RetrievePC();
          unsigned short relativeAddress = (this->RAM[address + 1] << 8) | (this->RAM[address] & 0xFF);
          unsigned short relativeAddressAddress = address + relativeAddress;
          decodedAddress = (this->RAM[relativeAddressAddress + 1] << 8) | (this->RAM[relativeAddressAddress] & 0xFF);
        }

        else
        {
          modeType = "Deferred Indexed";

          /*
           * The address is the some of the contents pointed to by the
           * operand plus the specified offset which is the word following
           * the instruction
           */

          if (type == Transaction::read)
          {
            this->IncrementPC();
          }

          unsigned short base = (this->RAM[regArray[reg] + 1] << 8) | (this->RAM[regArray[reg]] & 0xFF);
          unsigned short offsetAddress = this->RetrievePC();
          unsigned short offset = (this->RAM[offsetAddress + 1] << 8) | (this->RAM[offsetAddress] & 0xFF);
          unsigned short address = offset + base;
          decodedAddress = (this->RAM[address + 1] << 8) | (this->RAM[address] & 0xFF);
          this->TraceDump(Transaction::read, address);
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
  if (address < R0)
  {
    this->TraceDump(Transaction::read, address);
  }

  /*
   * Read either a byte or a word from memory depending on the value
   * of the internal byteMode flag.
   */
  if (this->byteMode == 01)
  {
    return this->RAM[address];
  }

  else
  {
    return (this->RAM[address + 1] << 8) | (this->RAM[address] & 0xFF);
  }
}
/*}}}*/

unsigned short Memory::ReadInstruction()/*{{{*/
{
  unsigned short address = this->RetrievePC();
  // Trace file output
  this->TraceDump(Transaction::instruction, this->RetrievePC());
  return (this->RAM[address + 1] << 8) | (this->RAM[address] & 0xFF);
}
/*}}}*/

void Memory::Write(unsigned short encodedAddress, unsigned short data)/*{{{*/
{
  unsigned short address = this->EA(encodedAddress, Transaction::write);

  // If not a general register operand then do a trace dump
  if (address < R0)
  {
    this->TraceDump(Transaction::write, address);
  }

  // Write the data to the specified memory address
  if (this->byteMode == 01)
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
  // Read stack
  unsigned short address = (this->RAM[SP + 1] << 8) | (this->RAM[SP] & 0xFF);

  // Increment stack pointer
  address += 02;
  this->RAM[SP] = address & 0xFF;
  this->RAM[SP + 1] = address >> 8;

  // Return data
  return (this->RAM[address + 1] << 8) | (this->RAM[address] & 0xFF);
}
/*}}}*/

void Memory::StackPush(unsigned short _register)/*{{{*/
{

  unsigned short address = (this->RAM[SP + 1] << 8) | (this->RAM[SP] & 0xFF);
  /*
   * Check if stack pointer has exceeded it's limit.
   * If it has then we need to crash and burn.
   * Limit is 0400
   */
  if (address > 0400)
  {
    // Decrement stack pointer
    address -= 02;
    this->RAM[SP] = address & 0xFF;
    this->RAM[SP + 1] = address >> 8;

    // Get location in memory to write to
    this->TraceDump(Transaction::write, address);
    unsigned short location = (this->RAM[address + 1] << 8) | (this->RAM[address] & 0xFF);

    // Write the data
    this->RAM[location] = _register & 0xFF;
    this->RAM[location + 1] = _register >> 8;
  }

  else
  {
    std::cout << "Warning: stack overflow has occurred!" << std::endl;
  }

  return;
}
/*}}}*/

void Memory::RegDump()/*{{{*/
{
  std::cout << "Dumping current register contents..." << std::endl;
  std::cout << "R0: " << std::oct << static_cast<unsigned short>((this->RAM[R0 + 1] << 8) | (this->RAM[R0] & 0xFF)) << std::endl;
  std::cout << "R1: " << std::oct << static_cast<unsigned short>((this->RAM[R1 + 1] << 8) | (this->RAM[R1] & 0xFF)) << std::endl;
  std::cout << "R2: " << std::oct << static_cast<unsigned short>((this->RAM[R2 + 1] << 8) | (this->RAM[R2] & 0xFF)) << std::endl;
  std::cout << "R3: " << std::oct << static_cast<unsigned short>((this->RAM[R3 + 1] << 8) | (this->RAM[R3] & 0xFF)) << std::endl;
  std::cout << "R4: " << std::oct << static_cast<unsigned short>((this->RAM[R4 + 1] << 8) | (this->RAM[R4] & 0xFF)) << std::endl;
  std::cout << "R5: " << std::oct << static_cast<unsigned short>((this->RAM[R5 + 1] << 8) | (this->RAM[R5] & 0xFF)) << std::endl;
  std::cout << "SP: " << std::oct << static_cast<unsigned short>((this->RAM[SP + 1] << 8) | (this->RAM[SP] & 0xFF)) << std::endl;
  std::cout << "PC: " << std::oct << static_cast<unsigned short>((this->RAM[PC + 1] << 8) | (this->RAM[PC] & 0xFF)) << std::endl;
  std::cout << std::endl;
  std::cout << "Processor status word: " << std::endl;
  std::cout << "N: " << std::oct << ((static_cast<unsigned short>(this->RAM[PS]) & 0x8) >> 3) << std::endl;
  std::cout << "Z: " << std::oct << ((static_cast<unsigned short>(this->RAM[PS]) & 0x4) >> 2) << std::endl;
  std::cout << "V: " << std::oct << ((static_cast<unsigned short>(this->RAM[PS]) & 0x2) >> 1) << std::endl;
  std::cout << "C: " << std::oct << (static_cast<unsigned short>(this->RAM[PS]) & 0x1)        << std::endl;
  return;
}/*}}}*/

void Memory::TraceDump(Transaction type, unsigned short address)/*{{{*/
{
  std::string buffer;
  std::stringstream stream;
  stream << std::oct << type;
  buffer.append(stream.str());
  buffer.append(" ");
  stream.str(std::string());
  stream.clear();
  stream << std::setfill('0') << std::setw(6);
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
  return (this->RAM[PS + 1] << 8) | (this->RAM[PS] & 0xFF);
}

// WritePS()
void Memory::WritePS(unsigned short status)
{
  this->RAM[PS] = status & 0xFF;
  this->RAM[PS + 1] = (status >> 8);
} /*}}}*/

void Memory::ResetPC()/*{{{*/
{
  this->RAM[PC] = initialPC & 0xFF;
  this->RAM[PC + 1] = initialPC >> 8;

  if (debugLevel == Verbosity::verbose)
  {
    std::cout << "Reset PC to " << initialPC << std::endl;
  }

  return;
}/*}}}*/

// Restore RAM to initial state of program
void Memory::ResetRAM()
{
  for (int i = 0; i < 65536; ++i)
  {
    this->RAM[i] = this->initialRAM[i];
  }
}

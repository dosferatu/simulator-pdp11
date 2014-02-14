#include <iostream>
#include "cpu.h"

#define src 3
#define dst 1
#define clr 0
#define set 1
#define Zbit 04
#define Nbit 010
#define Cbit 01
#define Vbit 02
#define Negbit 040
#define WORD 0x8000
#define BYTE 0x0080

CPU::CPU()
{
  this->debugLevel = Verbosity::off;
  this->memory = new Memory();
}

CPU::CPU(Memory *memory)
{
  this->debugLevel = Verbosity::off;
  this->memory = memory;
}

CPU::~CPU()
{
  delete this->memory;
}

short CPU::EA(short encodedAddress)
{
  char mode = encodedAddress & 070;
  char reg = encodedAddress & 07;
  short decodedAddress = 0;
  std::string modeType = "Not Set!";

  /*
   * IMPLEMENT CHECK FOR OPERAND ADDRESSES
   * MAKE SURE THAT RELATIVE AND ABSOLUTE
   * ARE HANDLED FROM THE MACRO11 PROPERLY
   *
   * ESPECIALLY SINCE ABS AND REL DO NOT
   * SEEM TO AFFECT THE INSTRUCTION CODES
   * PRODUCED IN THE .LST FILE
   */

  switch(mode)
  {
    case 0: // General Register
      {
        decodedAddress = encodedAddress;
        break;
      }

    case 1: // Deferred Register
      {
        decodedAddress = this->memory->Read(encodedAddress);
        break;
      }

    case 2: // Autoincrement
      {
        // Check for immediate PC addressing
        if (reg == 07)
        {
          // Point to the word after the instruction word

          decodedAddress = this->memory->Read(PC) + 02;
        }

        else
        {
          /*
           * Retrieve the memory address from encodedAddress
           * and then increment the pointer stored in encodedAddress
           */
          decodedAddress = this->memory->Read(encodedAddress);
          this->memory->Write(encodedAddress, decodedAddress + 02);
        }

        break;
      }

    case 3: // Autoincrement Deferred
      {
        // Check for absolute PC addressing
        if (reg == 07)
        {
          // Retrieve the operand from the address given
          decodedAddress = this->memory->Read(encodedAddress);
        }

        else
        {
          // Do we increment the pointer or address?
          short pointer = memory->Read(encodedAddress);
          short decodedAddress = memory->Read(pointer);
          memory->Write(encodedAddress, decodedAddress + 02);
        }

        break;
      }

    case 4: // Autodecrement
      {
        decodedAddress = this->memory->Read(encodedAddress);
        this->memory->Write(encodedAddress, decodedAddress - 02);
      }

    case 5: // Autodecrement Deferred
      {
        // Do we decrement the pointer or address?
        short pointer = memory->Read(encodedAddress);
        short decodedAddress = memory->Read(pointer);
        memory->Write(encodedAddress, decodedAddress - 02);
      }

    case 6: // Indexed
      {
        // Check for relative PC addressing
        if (reg == 07)
        {
          // Calculate the distance between address of operand and the PC
          decodedAddress = encodedAddress - this->memory->Read(PC);
          break;
        }

        else
        {
          /*
           * The address is the some of the contents of the operand plus the specified
           * offset which is the word following the instruction
           */
          decodedAddress = this->memory->Read(PC + 02) + this->memory->Read(encodedAddress);
        }

        break;
      }

    case 7: // Deferred Indexed
      {
        // Check for deferred relative PC addressing
        if (reg == 07)
        {
          /*
           * Calculate the distance between value at address pointed
           * to by operand and the PC
           */
          short value = this->memory->Read(encodedAddress);
          decodedAddress = value - this->memory->Read(PC);
        }

        else
        {
          /*
           * The address is the some of the contents pointed to by the
           * operand plus the specified offset which is the word following
           * the instruction
           */
          short value = this->memory->Read(encodedAddress);
          decodedAddress = this->memory->Read(PC + 02) + this->memory->Read(value);
        }

        break;
      }

    default:
      break;
  }

  if (this->debugLevel == Verbosity::verbose)
  {
    std::cout << "Mode Type: " << modeType << "(" << static_cast<int>(mode) << ")" << std::endl;
    std::cout << "Register: " << static_cast<int>(reg) << std::endl;
  }

  return decodedAddress;
}

/*
 * Takes in the program counter register value in order to
 * be able to fetch, decode, and execute the next instruction
 * .
 */ 
int CPU::FDE()
{
  short pc;                   // Program counter buffer
  short instruction;          // Instruction word buffer
  short instructionBits[6];   // Dissected instruction word
  /*>>>>>>>>>>>>>*/
  auto address = [=] (const int i) { return (instructionBits[i] << 3) + instructionBits[i - 1]; };
  auto update_flags = [=] (const int i, const int bit) 
  {
    if (i == 0) { 
      short temp = memory->ReadPS();
      temp = temp & ~(bit);
      memory->WritePS(temp);
    }
    else {
      short temp = memory->ReadPS();
      temp = temp | bit;
      memory->WritePS(temp);
    }
  };
  auto resultIsZero = [=] (const int result) \
    { result == 0? update_flags(1,Zbit) : update_flags(0,Zbit); };  // Update Zbit where result is zero
  auto resultLTZero = [=] (const int result) { result < 0? \
    update_flags(1,Nbit) : update_flags(0,Nbit); };                 // Update Nbit where result is negative
  auto resultMSBIsOne = [=] (const int result) { result >> 15 > 0? \
    update_flags(1,Nbit) : update_flags(0,Nbit); };                 // Update Nbit where result MSB is 1 (negative)
  /* <<<<<<<<<<<<<<<*/

  /*
   * BEGIN INSTRUCTION FETCH
   *
   */

  // Retrieve the PC value and increment by 2
  pc = memory->Read(PC);
  memory->Write(PC, pc + 02);

  // Fetch the instruction
  instruction = memory->ReadInstruction(pc);

  /*
   * BEGIN INSTRUCTION DECODE & EXECUTE
   */

  /* Notes about the decoder
  */
  instructionBits[0] = (instruction & 0000007);
  instructionBits[1] = (instruction & 0000070) >> 3; 
  instructionBits[2] = (instruction & 0000700) >> 6;
  instructionBits[3] = (instruction & 0007000) >> 9;
  instructionBits[4] = (instruction & 0070000) >> 12;
  instructionBits[5] = (instruction & 0700000) >> 15; // This is actually only giving us the final bit of the 16-bit word
  short tmp      = 0; // Use for holding temperary values for instruction operations
  short offset   = 0; // Used to hold the PC value for offsetting in cases of branching
  short src_temp = 0; // Used for storing temporary source values
  short dst_temp = 0; // Used for storing temporary destination values

  if(instructionBits[4] == 0) 
  {
    switch(instructionBits[3])
    {
      case 0: switch(instructionBits[2])
              {
                case 0: switch(instructionBits[5])
                        {
                          case 0: switch(instructionBits[0])
                                  {
                                    case 0: return 0; // HALT
                                    case 1: return 1; // WAIT
                                    case 5: return 5; // RESET
                                    default: break;
                                  }
                        }
                case 1: return instruction; // JMP
                case 2: switch(instructionBits[1])
                        {
                          case 0: { // RTS reg
                                    tmp = memory->Read(address(dst));                 // Read register value
                                    memory->Write(007, tmp);                          // reg --> (PC)
                                    memory->Write(address(dst),memory->StackPop());   // pop reg
                                    return instruction;
                                  }
                          case 4: switch(instructionBits[0])
                                  {
                                    case 1: { update_flags (0, Cbit); return instruction; } // CLC - Clear C
                                    case 2: { update_flags (0, Vbit); return instruction; } // CLV - Clear V
                                    case 4: { update_flags (0, Zbit); return instruction; } // CLZ - Clear Z
                                    default: break;
                                  }
                          case 5: { update_flags (0, Nbit); return instruction; }  // CLN - Clear N
                          case 6: switch(instructionBits[0])
                                  {
                                    case 1: { update_flags (1, Cbit); return instruction; } // SEC - Set C
                                    case 2: { update_flags (1, Vbit); return instruction; } // SEV - Set V
                                    case 4: { update_flags (1, Zbit); return instruction; } // SEZ - Set Z
                                    default: break;
                                  }
                          case 7: { update_flags (1, Nbit); return instruction; } // SEN - Set N
                          default: break;
                        }
                case 3: { // SWAB dst - swap bytes of word at dst
                          tmp = memory->Read(address(dst));       // Get value at effective address
                          short byte_temp = tmp << 8;             // Create temp and give it LSByte of value in MSByte
                          tmp = (tmp >> 8) & 0000777;             // Shift MSByte into LSByte and clear MSByte
                          tmp = byte_temp + tmp;                  // Finalize the swap byte
                          memory->Write(address(dst), byte_temp); // Write to register
                          return instruction;
                        }
                default: break;
              }    
      case 4: { // JSR reg, dst
                tmp = memory->Read(address(dst));                 // Get value at effective address for dst
                short reg = memory->Read(instructionBits[2]);     // Get value at effective address for reg
                memory->StackPush(reg);                           // Push value from reg onto stack
                reg = memory->Read(007);                          // Get value from PC
                memory->Write(instructionBits[2],reg);                // Write PC value to register
                memory->Write(007,tmp);                           // Write new address to PC
                return instruction;
              }
      case 5: switch(instructionBits[5])
              {
                case 0: switch(instructionBits[2])
                        {
                          case 0: { // CLR dst - Clear Destination
                                    memory->Write(address(dst), 0);   // Clear value at address
                                    update_flags(1,Zbit);             // Set Z bit
                                    update_flags(0,Nbit);             // Set N bit
                                    update_flags(0,Cbit);             // Set C bit
                                    update_flags(0,Vbit);             // Set V bit
                                    return instruction;
                                  }
                          case 1: { // COM dst: ~(dst) -> (dst)
                                    tmp = memory->Read(address(dst)); // Get value at address
                                    tmp = ~tmp;                       // Compliment value
                                    memory->Write(address(dst), tmp); // Write compiment to memory
                                    resultIsZero(tmp);                // Update Z bit
                                    resultLTZero(tmp);                // Update N bit
                                    tmp == 0? \
                                      update_flags(0,Cbit) : update_flags(1,Cbit);  // Update C bit
                                    (tmp & WORD) > 0? \
                                      update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                                    return instruction;
                                  }
                          case 2: { // INC dst: (dst)++ -> (dst)
                                    dst_temp = memory->Read(address(dst)); // Get value at address
                                    tmp = dst_temp + 1;               // Increment value
                                    memory->Write(address(dst), tmp); // Write to memory
                                    resultIsZero(tmp);                // Update Z bit
                                    resultLTZero(tmp);                // Update N bit
                                                                      // C bit not affected
                                    dst_temp == 0077777? update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                                    return instruction;
                                  }
                          case 3: { // DEC dst: (dst)-- -> (dst)
                                    tmp = memory->Read(address(dst)); // Get value at address
                                    tmp--;                            // Decrement value
                                    memory->Write(address(dst), tmp); // Write to memory
                                    resultIsZero(tmp);                // Update Z bit
                                    resultLTZero(tmp);                // Update N bit
                                    update_flags(0,Cbit);             // Update C bit
                                    update_flags(0,Vbit);             // Update V bit
                                    return instruction;
                                  }
                          case 4: { // NEG dst: -(dst) -> (dst) 
                                    tmp = memory->Read(address(dst)); // Get value at address
                                    tmp = ~tmp + 1;                   // Get 2's comp of value
                                    memory->Write(address(dst),tmp);  // Write to memory
                                    resultIsZero(tmp);                // Update Z bit
                                    resultLTZero(tmp);                // Update N bit
                                    tmp == 0? update_flags(0,Cbit) : update_flags(1,Cbit);          // Update C bit
                                    (tmp & WORD) > 0? update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                                    return instruction;
                                  }
                          case 5: { // ADC: (dst) + (C) -> (dst)
                                    dst_temp = memory->Read(address(dst)); // Get value at address
                                    short tmpC = memory->ReadPS();         // Get current value of PS
                                    tmpC = tmpC & 0x1;                     // Get C bit value
                                    tmp = dst_temp + (tmpC);               // Add C bit to value
                                    memory->Write(address(dst),tmp);       // Write to memory
                                    resultIsZero(tmp);                     // Update Z bit
                                    resultLTZero(tmp);                     // Update N bit
                                    (dst_temp == 0177777) && (tmpC == 1)? \
                                      update_flags(1,Cbit) : update_flags(0,Cbit);                // Update C bit
                                    tmp == 0077777? update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                                    return instruction;
                                  }
                          case 6: { // SBC: (dst) - (C) -> (dst)
                                    tmp = memory->Read(address(dst)); // Get value at address
                                    short tmpC = memory->ReadPS();    // Get current value of PS
                                    tmpC = tmpC & 0x1;                // Get C bit value
                                    tmp = tmp - tmpC;                 // Add C bit to value
                                    memory->Write(address(dst),tmp);  // Write to memory
                                    resultIsZero(tmp);                // Update Z bit
                                    resultLTZero(tmp);                // Update N bit
                                    (tmp == 0) && (tmpC == 1)? \
                                      update_flags(0,Cbit) : update_flags(1,Cbit);  // Update C bit
                                    (tmp & WORD) > 0? update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                                    return instruction;
                                  }
                          case 7: { // TST dst - Tests if dst is 0 (0 - dst)
                                    tmp = memory->Read(address(dst)); // Get value at address
                                    tmp = 0 - tmp;                    // Perform test
                                    resultIsZero(tmp);                // Update Z bit
                                    resultLTZero(tmp);                // Update N bit
                                    update_flags(0,Cbit);             // Update C bit
                                    update_flags(0,Vbit);             // Update V bit   
                                    return instruction;
                                  }
                          default: break;
                        }
                case 1: switch(instructionBits[2])
                        {
                          case 0: { // CLRB dst - Clear Byte
                                    memory->SetByteMode();            // Set byte mode
                                    tmp = memory->Read(address(dst)); // Get value at dst
                                    tmp = tmp & 0x0;                  // Clear byte
                                    memory->Write(address(dst),tmp);  // Write byte to dst
                                    memory->ClearByteMode();          // Clear byte mode
                                    return instruction;
                                  }
                          case 1: { // COMB dst: ~(dst) -> (dst)
                                    memory->SetByteMode();            // Set byte mode
                                    tmp = memory->Read(address(dst)); // Get value at address
                                    tmp = ~tmp & 0x00FF;              // Compliment value
                                    memory->Write(address(dst), tmp); // Write compiment to memory
                                    resultIsZero(tmp);                // Update Z bit
                                    resultLTZero(tmp << 8);           // Update N bit
                                    tmp == 0? \
                                      update_flags(0,Cbit) : update_flags(1,Cbit);  // Update C bit
                                    tmp == BYTE? \
                                      update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                                    memory->ClearByteMode();                        // Clear byte mode
                                    return instruction;
                                  }
                          case 2: { // INCB dst: (dst)++ -> (dst)
                                    memory->SetByteMode();            // Set byte mode
                                    dst_temp = memory->Read(address(dst));  // Get value at address
                                    tmp = dst_temp + 1;               // Increment value
                                    memory->Write(address(dst), tmp); // Write to memory
                                    resultIsZero(tmp);                // Update Z bit
                                    resultLTZero(tmp << 8);           // Update N bit
                                                                      // C bit not affected
                                    dst_temp == 0x00FF? update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                                    memory->ClearByteMode();          // Clear byte mode
                                    return instruction;
                                  }
                          case 3: { // DECB dst: (dst)-- -> (dst)
                                    memory->SetByteMode();            // Set byte mode
                                    tmp = memory->Read(address(dst)); // Get value at address
                                    tmp--;                            // Decrement value
                                    memory->Write(address(dst), tmp); // Write to memory
                                    resultIsZero(tmp);                // Update Z bit
                                    resultLTZero(tmp << 8);           // Update N bit
                                    update_flags(0,Cbit);             // Update C bit
                                    update_flags(0,Vbit);             // Update V bit
                                    memory->ClearByteMode();          // Clear byte mode
                                    return instruction;
                                  }
                          case 4: { // NEGB dst: -(dst) -> (dst)
                                    memory->SetByteMode();            // Set byte mode
                                    tmp = memory->Read(address(dst)); // Get value at address
                                    tmp = (~tmp + 1) & 0x00FF;        // Get 2's comp of value
                                    memory->Write(address(dst),tmp);  // Write to memory
                                    resultIsZero(tmp);                // Update Z bit
                                    resultLTZero(tmp << 8);           // Update N bit
                                    tmp == 0? update_flags(0,Cbit) : update_flags(1,Cbit);       // Update C bit
                                    tmp == BYTE? update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                                    memory->ClearByteMode();          // Clear byte mode
                                    return instruction;
                                  }
                          case 5: { // ADCB: (dst) + (C) -> (dst)
                                    memory->SetByteMode();                 // Set byte mode
                                    dst_temp = memory->Read(address(dst)); // Get value at address
                                    short tmpC = memory->ReadPS();         // Get current value of PS
                                    tmpC = tmpC & 0x1;                     // Get C bit value
                                    tmp = (dst_temp + (tmpC)) & 0x00FF;    // Add C bit to value
                                    memory->Write(address(dst),tmp);       // Write to memory
                                    resultIsZero(tmp);                     // Update Z bit
                                    resultLTZero(tmp << 8);                // Update N bit
                                    (dst_temp == 0x00FF) && (tmpC == 1)? \
                                      update_flags(1,Cbit) : update_flags(0,Cbit);                // Update C bit
                                    tmp == 0x007F? update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                                    memory->ClearByteMode();              // Clear byte mode
                                    return instruction;
                                  }
                          case 6: { // SBCB: (dst) - (C) -> (dst)
                                    memory->SetByteMode();            // Set byte mode
                                    dst_temp = memory->Read(address(dst)); // Get value at address
                                    short tmpC = memory->ReadPS();    // Get current value of PS
                                    tmpC = tmpC & 0x1;                // Get C bit value
                                    tmp = dst_temp - tmpC;            // Add C bit to value
                                    memory->Write(address(dst),tmp);  // Write to memory
                                    resultIsZero(tmp);                // Update Z bit
                                    resultLTZero(tmp);                // Update N bit
                                    (tmp == 0) && (tmpC == 1)? \
                                      update_flags(0,Cbit) : update_flags(1,Cbit);  // Update C bit
                                    ((tmp & BYTE) >> 7) == 1? \
                                      update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                                    memory->ClearByteMode();          // Clear byte mode
                                    return instruction;
                                  }
                          case 7: { // TST dst - Tests if dst is 0 (0 - dst)
                                    tmp = memory->Read(address(dst)); // Get value at address
                                    tmp = 0 - tmp;                    // Perform test
                                    resultIsZero(tmp);                // Update Z bit
                                    resultLTZero(tmp << 8);           // Update N bit
                                    update_flags(0,Cbit);             // Update C bit
                                    update_flags(0,Vbit);             // Update V bit   
                                    return instruction;
                                  }
                          default: break;
                        }
                default: break;
              }
      case 6: switch(instructionBits[5])
              {
                case 0: switch(instructionBits[2])
                        {
                          case 0: { // ROR dst: ROtate Rigtht - include C bit as MSB -> (dst)
                                    dst_temp = memory->Read(address(dst));  // Get value at dst
                                    short tempCN = memoryReadPS() & 0x9;    // Get C and N bits
                                    tmp = (dst_temp >> 1) | ((tempCN & 01) << 15);  // Rotate bits to the right
                                    memory->Write(addres(dst),tmp);         // Write to memory
                                    resultIsZero(tmp);                      // Update Z bit
                                    resultLTZero(tmp);                      // Update N bit
                                    update_flags(dst_temp & 01,Cbit);       // Update C bit
                                    update_flags((tempCN >> 3) ^ (tempCN & 01),Vbit); // Update V bit - C ^ N
                                    return instruction;
                                  }
                          case 1: { // ROL dst: ROtate Left - include C bit as LSB -> (dst)
                                    dst_temp = memory->Read(address(dst));  // Get value at dst
                                    short tempCN = memoryReadPS() & 0x9;    // Get C and N bits
                                    tmp = (dst_temp << 1) | (tempCN & 01);  // Rotate bits to the right
                                    memory->Write(addres(dst),tmp);         // Write to memory
                                    resultIsZero(tmp);                      // Update Z bit
                                    resultLTZero(tmp);                      // Update N bit
                                    update_flags((dst_temp & WORD) >> 15,Cbit); // Update C bit
                                    update_flags((tempCN >> 3) ^ (tempCN & 01),Vbit); // Update V bit - C ^ N
                                    return instruction;
                                  }
                          case 2: { // ASR dst: Arithmetic Shift Right
                                    dst_temp = memory->Read(address(dst));  // Get value at dst
                                    short tempCN = memoryReadPS() & 0x9;     // Get C and N bits
                                    tmp = dst_temp >> 1;                    // Rotate bits to the right
                                    memory->Write(addres(dst),tmp);         // Write to memory
                                    resultIsZero(tmp);                      // Update Z bit
                                    resultLTZero(tmp);                      // Update N bit
                                    update_flags((dst_temp & 01),Cbit);     // Update C bit LSB (result)
                                    update_flags((tempCN >> 3) ^ (tempCN & 01),Vbit); // Update V bit - C ^ N
                                    return instruction;
                                  }
                          case 3:  { // ASL dst: Arithmetic Shift Left 
                                    dst_temp = memory->Read(address(dst));  // Get value at dst
                                    short tempCN = memoryReadPS() & 0x9;    // Get C and N bits
                                    tmp = dst_temp << 1;                    // Rotate bits to the right
                                    memory->Write(addres(dst),tmp);         // Write to memory
                                    resultIsZero(tmp);                      // Update Z bit
                                    resultLTZero(tmp);                      // Update N bit
                                    update_flags((dst_temp & WORD) >> 15,Cbit); // Update C bit
                                    update_flags((tempCN >> 3) ^ (tempCN & 01),Vbit); // Update V bit - C ^ N
                                    return instruction;
                                  }
                          default: break;
                        }
                case 1: switch(instructionBits[2])
                        {
                           case 0: { // RORB dst: ROtate Rigtht - include C bit as MSB -> (dst)
                                     memory->SetByteMode();                  // Set byte mode
                                     dst_temp = memory->Read(address(dst));  // Get value at dst
                                     short tempCN = memoryReadPS() & 0x9;    // Get C and N bits
                                     tmp = (dst_temp >> 1) | ((tempCN & 01) << 7);  // Rotate bits to the right
                                     memory->Write(addres(dst),tmp);         // Write to memory
                                     resultIsZero(tmp);                      // Update Z bit
                                     resultLTZero(tmp);                      // Update N bit
                                     update_flags(dst_temp & 01,Cbit);       // Update C bit
                                     update_flags((tempCN >> 3) ^ (tempCN & 01),Vbit); // Update V bit - C ^ N
                                     memory->ClearByteMode();                // Clear byte mode
                                     return instruction;
                                  }
                          case 1: { // ROLB dst: ROtate Left - include C bit as LSB -> (dst)
                                    memory->SetByteMode();                  // Set byte mode
                                    dst_temp = memory->Read(address(dst));  // Get value at dst
                                    short tempCN = memoryReadPS() & 0x9;    // Get C and N bits
                                    tmp = (dst_temp << 1) | (tempCN & 01);  // Rotate bits to the right
                                    memory->Write(addres(dst),tmp & 0x00FF);// Write to memory
                                    resultIsZero(tmp);                      // Update Z bit
                                    resultLTZero(tmp);                      // Update N bit
                                    update_flags((dst_temp & BYTE) >> 7,Cbit); // Update C bit
                                    update_flags((tempCN >> 3) ^ (tempCN & 01),Vbit); // Update V bit - C ^ N
                                    memory->ClearByteMode();                // Clear byte mode
                                    return instruction;
                                  }
                          case 2: { // ASRB dst: Arithmetic Shift Right
                                    memory->SetByteMode();                  // Set byte mode
                                    dst_temp = memory->Read(address(dst));  // Get value at dst
                                    short tempCN = memoryReadPS() & 0x9;     // Get C and N bits
                                    tmp = dst_temp >> 1;                    // Rotate bits to the right
                                    memory->Write(addres(dst),tmp);         // Write to memory
                                    resultIsZero(tmp);                      // Update Z bit
                                    resultLTZero(tmp);                      // Update N bit
                                    update_flags((dst_temp & 01),Cbit);     // Update C bit LSB (result)
                                    update_flags((tempCN >> 3) ^ (tempCN & 01),Vbit); // Update V bit - C ^ N
                                    memory->ClearByteMode();                // Clear byte mode
                                    return instruction;
                                  }
                          case 3:  { // ASLB dst: Arithmetic Shift Left 
                                     memory->SetByteMode();                 // Set byte mode
                                     dst_temp = memory->Read(address(dst));  // Get value at dst
                                     short tempCN = memoryReadPS() & 0x9;    // Get C and N bits
                                     tmp = dst_temp << 1;                    // Rotate bits to the right
                                     memory->Write(addres(dst),tmp & 0x00FF);// Write to memory
                                     resultIsZero(tmp);                      // Update Z bit
                                     resultLTZero(tmp);                      // Update N bit
                                     update_flags((dst_temp & BYTE) >> 7,Cbit); // Update C bit
                                     update_flags((tempCN >> 3) ^ (tempCN & 01),Vbit); // Update V bit - C ^ N
                                     memory->ClearByteMode();                // Clear byte mode
                                     return instruction;
                                  }
                          default: break;
                        }
                default: break;
              }
      default: break;
    }

    // Check for branches
    switch(instructionBits[3])
    {
      case 0: switch(instruction[2])
              {
                case 0: case 1: 
                case 2: case 3: // BPL - Branch if PLus (positive)
                                {
                                  offset = memory->Read(address(dst));  // Get address for branch
                                  tmp = memory->Read(007);              // Get current address in PC
                                  tmp = tmp + (offset << 2);            // Get new address for branch
                                  memory->ReadPS() & Nbit == 0? \
                                   memory->Write(007,tmp) : ;           // N = 0
                                  return instruction;
                                }
                case 4: case 5: 
                case 6: case 7: // Either BR or BMI
                                switch (instructionBits[5])
                                {
                                  case 0: { // BR - unconditional Branch
                                            offset = memory->Read(address(dst));  // Get address for branch
                                            tmp = memory->Read(007);              // Get current address in PC
                                            tmp = tmp + (offset << 2);            // Get new address for branch
                                            memory->Write(007,tmp);               // Branch always
                                            return instruction;
                                          }
                                  case 1: { // BMI - Branch if negative
                                            offset = memory->Read(address(dst));  // Get address for branch
                                            tmp = memory->Read(007);              // Get current address in PC
                                            tmp = tmp + (offset << 2);            // Get new address for branch
                                            memory->ReadPS() & Nbit > 0? \
                                              memory->Write(007,tmp) : ;          // N = 1
                                            return instruction;
                                          }
                                }
                default: break;
              }
      case 1: switch(instructionBits[2]) // BNE, BHI, BLOS, BEQ
              {
                case 0: case 1: 
                case 2: case 3: switch(instructionBits[5]) // BNE or BHI
                                {
                                  case 0: { // BNE - Branch if Not Equal (zero)
                                            offset = memory->Read(address(dst));  // Get address for branch
                                            tmp = memory->Read(007);              // Get current address in PC
                                            tmp = tmp + (offset << 2);            // Get new address for branch
                                            memory->ReadPS() & Zbit == 0? \
                                              memory->Write(007,tmp) : ;          // Z = 0
                                            return instruction;
                                          }
                                  case 1: { // BHI - Branch if Higher (zero)
                                            offset = memory->Read(address(dst));  // Get address for branch
                                            tmp = memory->Read(007);              // Get current address in PC
                                            tmp = tmp + (offset << 2);            // Get new address for branch
                                            (memory->ReadPS() & (Zbit | Cbit)) == 0? \
                                               memory->Write(007,tmp) :           // C & Z = 0
                                             return instruction;
                                          }
                                }
                case 4: case 5: 
                case 6: case 7: switch(instructionBits[5]) // BLOS or BEQ
                                {
                                  case 0: { // BLOS - Branch if Lower or Same
                                            offset = memory->Read(address(dst));  // Get address for branch
                                            tmp = memory->Read(007);              // Get current address in PC
                                            tmp = tmp + (offset << 2);            // Get new address for branch
                                            (memory->ReadPS() & (Zbit | Cbit)) > 0? \
                                               memory->Write(007,tmp) : ;         // C | Z = 1
                                            return instruction;
                                          }
                                  case 1: { // BEQ - Branch if EQual (zero)
                                            offset = memory->Read(address(dst));  // Get address for branch
                                            tmp = memory->Read(007);              // Get current address in PC
                                            tmp = tmp + (offset << 2);            // Get new address for branch
                                            memory->ReadPS() & Zbit > 0? \
                                              memory->Write(007,tmp) : ;          // Z = 0
                                            return instruction;
                                          }
                                }
                default: break;
              }
                                                
      case 2: switch(instructionBits[2]) // BVC, BGE, BVS, BLT
              {
                case 0: case 1: 
                case 2: case 3: switch(instructionBits[5]) // BGE OR BVC
                                {
                                  case 0: { // BGE - Branch if Greater or Equal (zero)
                                            offset = memory->Read(address(dst));  // Get address for branch
                                            tmp = memory->Read(007);              // Get current address in PC
                                            tmp = tmp + (offset << 2);            // Get new address for branch
                                            ((memory->ReadPS() & Nbit) >> 3) ^ ((memory->ReadPS() & Vbit) >> 1) == 0? \
                                              memory->Write(007,tmp) : ;          // N ^ V = 0
                                            return instruction;
                                          }
                                  case 1: { // BVC - Branch if oVerflow Clear
                                            offset = memory->Read(address(dst));  // Get address for branch
                                            tmp = memory->Read(007);              // Get current address in PC
                                            tmp = tmp + (offset << 2);            // Get new address for branch
                                            memory->ReadPS() & Vbit == 0? \
                                              memory->Write(007,tmp) : ;          // V = 0
                                            return instruction;
                                          }
                                }
                case 4: case 5: 
                case 6: case 7: switch(instructionBits[5]) // BLT OR BVS
                                {
                                  case 0: { // BLT - Branch if Less Than
                                            offset = memory->Read(address(dst));  // Get address for branch
                                            tmp = memory->Read(007);              // Get current address in PC
                                            tmp = tmp + (offset << 2);            // Get new address for branch
                                            ((memory->ReadPS() & Nbit) >> 3) ^ ((memory->ReadPS() & Vbit) >> 1) == 1? \
                                              memory->Write(007,tmp) : ;          // N ^ V == 1
                                            return instruction;
                                          }
                                  case 1: { // BVS - Branch if oVerflow Set
                                            offset = memory->Read(address(dst));  // Get address for branch
                                            tmp = memory->Read(007);              // Get current address in PC
                                            tmp = tmp + (offset << 2);            // Get new address for branch
                                            memory->Read(PS) & Vbit > 0? \
                                              memory->Write(007,tmp) : ;          // V = 1
                                            return instruction;
                                          }
                                }
                default: break;
              }
      case 3: switch(instructionBits[2]) // BGT, BCC, BLE, BCS
              {
                case 0: case 1: 
                case 2: case 3: switch(instructionBits[5]) // BGT OR BCC
                                {
                                  case 0: { // BGT - Branch if Greater Than
                                            offset = memory->Read(address(dst));  // Get address for branch
                                            tmp = memory->Read(007);              // Get current address in PC
                                            tmp = tmp + (offset << 2);            // Get new address for branch
                                            ((memory->ReadPS() & Zbit) >> 2) | \
                                              ((memory->ReadPS() & Nbit) >> 3) ^ ((memory->ReadPS() & Vbit) >> 1) == 0?
                                              memory->Write(007,tmp) : ;          // Z | (N ^ V) = 0
                                            return instruction;
                                          }
                                  case 1: { // BCC - Branch if C is Clear
                                            offset = memory->Read(address(dst));  // Get address for branch
                                            tmp = memory->Read(007);              // Get current address in PC
                                            tmp = tmp + (offset << 2);            // Get new address for branch
                                            memory->ReadPS() & Cbit = 0? \
                                              memory->Write(007,tmp) : ;          // C = 0
                                            return instruction;
                                          }
                                }
                case 4: case 5: 
                case 6: case 7: switch(instructionBits[5]) // BLE or BCS
                                {
                                  case 0: { // BLE - Branch if Less 
                                            case 0: { // BLE loc - Branch if Lower or Equal (zero)
                                                      offset = memory->Read(address(dst));  // Get address for branch
                                                      tmp = memory->Read(007);              // Get current address in PC
                                                      offset = tmp + (offset << 2);         // Get new address for branch
                                                      tmp = memory->ReadPS();               // Get current process status
                                                      (((tmp & Zbit) >> 2) & (((tmp & Nbit) >> 3) ^ ((tmp & Vbit) >> 1))) == 1? \
                                                        memory->Write(007,offset) : ;        // Z(N^V) = 1
                                                      return instruction;
                                                    }
                                            return instruction;
                                          }
                                  case 1: { // BCS - Branch if C Set
                                            offset = memory->Read(addres(dst));             // Get address for branch
                                            tmp = memory->Read(007);                        // Get current address in PC
                                            offset = tmp + (offset << 2);                   // Calc new address for branch
                                            memory->ReadPS() & Cbit > 0? \
                                              memory->Write(007,offset) : ;                 // C = 1
                                            return instruction;
                                          }
                                  default: break;
                                }
                  default: break;
              }
      default: break;
  }

  if(instructionBits[4] > 0)
  {
    switch(instructionBits[5])
    {
      case 0: switch(instructionBits[4])
              {
                case 1: { // MOV src, dst (src) -> (dst)
                          src_temp = memory->Read(address(src));  // Get value at address of src
                          memory->Write(address(dst),src_temp);   // Write value to memory
                          resultIsZero(src_temp);                 // Update Z bit
                          resultLTZero(src_temp);                 // Update N bit
                          update_flags(0,Vbit);                   // Update V bit
                          return instruction;
                        }
                case 2: { // CMP src, dst:  (src) + ~(dst) + 1
                          src_temp = memory->Read(address(src));          // Get value at address of src
                          dst_temp = memory->Read(address(dst));          // Get value at address of dst
                          tmp = src_temp + ~(dst_temp) + 1;               // Compare values
                          resultIsZero(tmp);                              // Update Z bit
                          resultLTZero(tmp);                              // Update N bit
                          (src_temp >= 0) & (dst_temp >= 0) & (tmp < 0)? \
                            update_flags(0,Cbit) : update_flags(1,Cbit);  // Update C bit
                          (((src_temp & WORD) ^ (dst_temp & WORD)) & \
                           ~((dst_temp & WORD) ^ (tmp & WORD))) == 0? \
                            update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                          return instruction;
                        }
                case 3: { // BIT src, dst:  src & dst
                          tmp = memory->Read(address(src)) & memory->Read(address(dst)); // Get test value
                          resultIsZero(tmp);            // Update Z bit
                          tmp & WORD = 0? \
                            update_flags(1,Nbit) : update_flags(0,Nbit);  // Update N bit if positive (weird)
                          update_flags(0,Vbit);         // Update V bit
                          return instruction;
                        }
                case 4: { // BIC src, dst:  Bit Clear
                          tmp = ~(memory->Read(address(src))) & memory->Read(address(dst)); // Get ~src & dst value
                          memory->Write(address(dst),tmp);                                  // Write value to dst
                          resultIsZero(tmp);                                                // Update Z bit
                          resultMSBISOne(tmp);                                              // Update N bit
                          update_flags(0,Vbit);                                             // Update V bit
                          return instruction;
                        }
                case 5: { // BIS src, dst: Bit Set
                          tmp = (memory->Read(address(src)) & memory->Read(address(dst));   // Get src & dst value
                          memory->Write(address(dst),tmp);                                  // Write value to dst
                          resultIsZero(tmp);                                                // Update Z bit
                          resultLTZero(tmp);                                                // Update N bit
                          update_flags(0,Vbit);                                             // Update V bit
                          return instruction;
                        }
                case 6: { // ADD src, dst:  (src) + (dst) -> (dst)
                          src_temp = memory->Read(address(src));  // Get source value
                          dst_temp = memory->Read(address(dst));  // Get destination value
                          tmp = src_temp + dst_temp               // Add src and dst
                          memory->Write(addres(dst),tmp);         // Write result to memory
                          resultIsZero(tmp);                      // Update Z bit
                          resultLTZero(tmp);                      // Update N bit
                          (src_temp & WORD) | (dst_temp & WORD)) \
                            & (tmp > 0)? update_flags(1,Cbit) : update_flags(0,Cbit);  // Update C bit
                          (src_temp & WORD) !^ (dst_temp & WORD))) \
                            & ~(tmp & WORD) > 0? update_flags(1,Vbit) : update_flags(0,Vbit) // Update V bit (src !^ dst) & ~result
                          return instruction;
                        }
                default: break;
              }
      case 1: switch(instructionBits[4])
              {
                case 1: { // MOVB src, dst: (src) -> (dst)
                          memory->SetByteMode();                  // Set byte mode
                          src_temp = memory->Read(address(src));  // Get value at address of src
                          memory->Write(address(dst),src_tmp);    // Write value to memory
                          resultIsZero(src_temp);                 // Update Z bit
                          resultLTZero(src_temp << 8);            // Update N bit
                          update_flags(0,Vbit);                   // Update V bit
                          memory->ClearByteMode();                // Clear byte mode
                          return instruction;
                        }
                case 2: { // CMPB src, dst:  (src) + ~(dst) + 1
                          memory->SetByteMode();                          // Set byte mode
                          src_temp = memory->Read(address(src));          // Get value at address of src
                          dst_temp = memory->Read(address(dst));          // Get destination value
                          tmp = src_temp + ~(dst_temp) + 1;               // Compare values
                          resultIsZero(tmp);                              // Update Z bit
                          resultLTZero(tmp);                              // Update N bit
                          (src_temp >= 0) & (dst_temp >= 0) & (tmp < 0)? \
                            update_flags(0,Cbit) : update_flags(1,Cbit);  // Update C bit
                          ((src_temp & BYTE) ^ (dst_temp & BYTE)) & \
                           ~((dst_temp & BYTE) ^ (tmp & BYTE))) == 0? \
                            update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                          memory->ClearByteMode();                        // Clear byte mode
                          return instruction;
                        }
                case 3: { // BITB src, dst:  src & dst
                          memory->SetByteMode;          // Set byte mode
                          tmp = memory->Read(address(src)) & memory->Read(address(dst)); // Get test value
                          resultIsZero(tmp);            // Update Z bit
                          tmp & BYTE = 0? \
                            update_flags(1,Nbit) : update_flags(0,Nbit);  // Update N bit if positive (weird)
                          update_flags(0,Vbit);         // Update V bit
                          memory->ClearByteMode;        // Clear byte mode
                          return instruction;
                        }
                case 4: { // BICB src, dst:  Bit Clear
                          memory->SetByteMode;          // Set byte mode
                          tmp = ~(memory->Read(address(src))) & memory->Read(address(dst)); // Get ~src & dst value
                          memory->Write(address(dst),tmp);                                  // Write value to dst
                          resultIsZero(tmp);                                                // Update Z bit
                          resultMSBISOne(tmp << 8);                                         // Update N bit
                          update_flags(0,Vbit);                                             // Update V bit
                          memory->ClearByteMode;        // Clear byte mode
                          return instruction;
                        }
                case 5: { // BIS src, dst: Bit Set
                          memory->SetByteMode;          // Set byte mode
                          tmp = (memory->Read(address(src)) & memory->Read(address(dst));   // Get src & dst value
                          memory->Write(address(dst),tmp);                                  // Write value to dst
                          resultIsZero(tmp);                                                // Update Z bit
                          resultLTZero(tmp);                                                // Update N bit
                          update_flags(0,Vbit);                                             // Update V bit
                          memory->ClearByteMode;        // Clear byte mode
                          return instruction;
                        }
                case 6: { // SUB src, dst: (dst) + ~(src) -> (dst)
                          dst_temp = memory->Read(address(dst));  // Get value of dst
                          src_temp = memory->Read(address(src));  // Get value of src
                          tmp = dst_temp + ~(src_temp) + 1;       // Subtract
                          memory->Write(address(dst), tmp);       // Write to memory
                          resultIsZero(tmp);                      // Update Z bit
                          resultLTZero(tmp);                      // Update N bit
                          (dst_temp & WORD) !^ (src_temp & WORD) \
                            & (tmp & WORD) == 0? update_flags(0,Cbit) : update_flags(1,Cbit);   // Update C bit
                          ((dst_temp & WORD) ^ (src_temp) & WORD)) & ((tmp & WORD) & (src_temp & WORD)) > 1? \
                            update_flags(1,Vbit) : update_flags(0,Vbit);                          // Update V bit
                          return instruction;
                        }
                default: break;
              }
      default: break;
    }
  }
}

// For debug purposes
if (instruction == 0)
{
  return -1;
}

return 0;
}

void CPU::SetDebugMode(Verbosity verbosity)
{
  this->debugLevel = verbosity;
  return;
}

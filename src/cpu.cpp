#include <iostream>
#include "cpu.h"

CPU::CPU()
{
  this->debugLevel = Verbosity::off;
  this->memory = new Memory();
}

CPU::CPU(Memory *memory)
{
  this->debugLevel = Verbosity::off;
  this->memory = memory;

  // Temporarily initialize the PC for now. Remove when implemented properly.
  this->memory->Write(PC, 010);
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

  if (this->debugLevel == Verbosity::minimal)
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

  if(instructionBits[4] == 0) 
  {
    switch(instructionBits[4])
    {
      case 0: switch(instructionBits[3])
              {
                case 0: switch(instructionBits[2])
                        {
                          case 0: switch(instructionBits[5])
                                  {
                                    case 0: switch(instructionBits[0])
                                            {
                                              case 0: return -1; // HALT
                                              case 1: return 0;  // WAIT
                                              case 5: return 1;  // RESET
                                              default: break;
                                            }
                                    case 1: // BPL loc (loc is an offset dependent on ABS or REL addressing
                                            // Get effective address
                                            // N = 0? loc --> (PC)
                                            break;
                                    default: break;
                                  }
                          case 1: // JMP dst
                                  // Get effective address
                                  // effective address --> (PC) unconditional
                                  break;
                          case 2: switch(instructionBits[1])
                                  {
                                    case 0: // RTS reg
                                      // reg --> (PC), pop reg
                                      break;
                                    case 4: switch(instructionBits[0])
                                            {
                                              case 1: break;  // CLC - Clear C
                                              case 2: break;  // CLV - Clear V
                                              case 4: break;  // CLZ - Clear Z
                                              default: break;
                                            }
                                    case 5: break;  // CLN - Clear N
                                    case 6: switch(instructionBits[0])
                                            {
                                              case 1: break; // SEC - Set C
                                              case 2: break; // SEV - Set V
                                              case 4: break; // SEZ - Set Z
                                              default: break;
                                            }
                                    case 7: break;  // SEN - Set N
                                  }
                          case 3: break; // SWAB dst - swap bytes of word at dst
                          case 4: switch(instructionBits[5])
                                  {
                                    case 0: break;  // BR loc - unconditional branch
                                    case 1: break;  // BMI loc
                                    default: break;
                                  }
                          default: break;
                        }
                case 1: switch(instructionBits[2])
                        {
                          case 0: switch(instructionBits[5])
                                  {
                                    case 0: break;  // BNE loc
                                    case 1: break;  // BHI loc
                                    default: break;
                                  }
                          case 4: switch(instructionBits[5])
                                  {
                                    case 0: break;  // BEQ loc
                                    case 1: break;  // BLOS loc
                                    default: break;
                                  }
                          default: break;
                        }
                case 2: switch(instructionBits[2])
                        {
                          case 0: switch(instructionBits[5])
                                  {
                                    case 0: break;  // BGE loc
                                    case 1: break;  // BVC loc
                                    default: break;
                                  }
                          case 4: switch(instructionBits[5])
                                  {
                                    case 0: break;  // BLT loc
                                    case 1: break;  // BVS loc
                                    default: break;
                                  }
                          default: break;
                        }
                case 3: switch(instructionBits[2])
                        {
                          case 0: switch(instructionBits[5])
                                  {
                                    case 0: break;  // BGT loc
                                    case 1: break;  // BCC loc
                                    default: break;
                                  }
                          case 4: switch(instructionBits[5])
                                  {
                                    case 0: break;  // BLE loc
                                    case 1: break;  // BCS loc
                                    default: break;
                                  }
                          default: break;
                        }
                case 4: break;  // JSR reg, dst
                case 5: switch(instructionBits[5])
                        {
                          case 0: switch(instructionBits[2])
                                  {
                                    case 0: break;  // CLR dst
                                    case 1: break;  // COM dst
                                    case 2: break;  // INC dst
                                    case 3: break;  // DEC dst
                                    case 4: break;  // NEG dst
                                    case 5: break;  // ADC dst
                                    case 6: break;  // SBC dst
                                    case 7: break;  // TST dst
                                    default: break;
                                  }
                          case 1: switch(instructionBits[2])
                                  {

                                    case 0: break;  // CLRB dst
                                    case 1: break;  // COMB dst
                                    case 2: break;  // INCB dst
                                    case 3: break;  // DECB dst
                                    case 4: break;  // NEGB dst
                                    case 5: break;  // ADCB dst
                                    case 6: break;  // SBCB dst
                                    case 7: break;  // TSTB dst
                                    default: break;
                                  }
                          default: break;
                        }
                case 6: switch(instructionBits[5])
                        {
                          case 0: switch(instructionBits[2])
                                  {
                                    case 0: break;  // ROR dst
                                    case 1: break;  // ROL dst
                                    case 2: break;  // ASR dst
                                    case 3: break;  // ASL dst
                                    default: break;
                                  }
                          case 1: switch(instructionBits[2])
                                  {
                                    case 0: break;  // RORB dst
                                    case 1: break;  // ROLB dst
                                    case 2: break;  // ASRB dst
                                    case 3: break;  // ASLB dst
                                    default: break;
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
                  case 1: break;  // MOV src, dst
                  case 2: break;  // CMP src, dst
                  case 3: break;  // BIT src, dst
                  case 4: break;  // BIC src, dst
                  case 5: break;  // BIS src, dst
                  case 6: break;  // ADD src, dst
                  default: break;
                }
        case 1: switch(instructionBits[4])
                {
                  case 1: break;  // MOVB src, dst
                  case 2: break;  // CMPB src, dst
                  case 3: break;  // BITB src, dst
                  case 4: break;  // BICB src, dst
                  case 5: break;  // BISB src, dst
                  case 6: break;  // ADDB src, dst
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

void CPU::SetDebugMode()
{
  this->debugLevel = Verbosity::minimal;
  return;
}

void CPU::ClearDebugMode()
{
  this->debugLevel = Verbosity::off;
  return;
}

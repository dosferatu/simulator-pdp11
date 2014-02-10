#include <iostream>
#include "cpu.h"

CPU::CPU()
{
  this->memory = new Memory();
  this->reg[0] = R0;
  this->reg[1] = R1;
  this->reg[2] = R2;
  this->reg[3] = R3;
  this->reg[4] = R4;
  this->reg[5] = R5;
  this->reg[6] = SP;
  this->reg[7] = PC;
  this->reg[8] = PS;
}

CPU::CPU(Memory *memory)
{
  this->memory = memory;
  this->reg[0] = R0;
  this->reg[1] = R1;
  this->reg[2] = R2;
  this->reg[3] = R3;
  this->reg[4] = R4;
  this->reg[5] = R5;
  this->reg[6] = SP;
  this->reg[7] = PC;
  this->reg[8] = PS;
  memory->Write(PC, 010);
}

CPU::~CPU()
{
  delete memory;
}

short CPU::EA(short encodedAddress)
{
  char mode = encodedAddress & 070;
  //char reg = encodedAddress & 07;
  short decodedAddress = 0;

  switch(mode)
  {
    case 0: // General Register
      {
        return encodedAddress;
      }

    case 1: // Deferred Register
      {
      }

    case 2: // Autoincrement
      {}

    case 3: // Autoincrement Deferred
      {}

    case 4: // Autodecrement
      {}

    case 5: // Autodecrement Deferred
      {}

    case 6: // Indexed
      {}

    case 7: // Deferred Indexed
      {}

    default:
      break;
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
  /*
   * BEGIN INSTRUCTION FETCH
   *
   */

  // Retrieve the PC value
  short pc = memory->Read(PC);

  // -(SP) pushes, +(SP) pops, X(SP) index accesses
  short instruction = memory->ReadInstruction(pc);
  pc += 2;


  /*
   * BEGIN INSTRUCTION DECODE
   */

  /* Notes about the decoder
  */
  short instruction_0 = (instruction & 0000007);
  short instruction_1 = (instruction & 0000070) >> 3; 
  short instruction_2 = (instruction & 0000700) >> 6;
  short instruction_3 = (instruction & 0007000) >> 9;
  short instruction_4 = (instruction & 0070000) >> 12;
  short instruction_5 = (instruction & 0700000) >> 15; // This is actually only giving us the final bit of the 16-bit word


  if(instruction_4 == 0) 
  {
    switch(instruction_4)
    {
      case 0: switch(instruction_3) 
              {
                case 0: switch(instruction_2)
                        {
                          case 0: switch(instruction_5)
                                  {
                                    case 0: switch(instruction_0)
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
                          case 2: switch(instruction_1)
                                  {
                                    case 0: // RTS reg
                                      // reg --> (PC), pop reg
                                      break;
                                    case 4: switch(instruction_0)
                                            {
                                              case 1: break;  // CLC - Clear C
                                              case 2: break;  // CLV - Clear V
                                              case 4: break;  // CLZ - Clear Z
                                              default: break;
                                            }
                                    case 5: break;  // CLN - Clear N
                                    case 6: switch(instruction_0)
                                            {
                                              case 1: break; // SEC - Set C
                                              case 2: break; // SEV - Set V
                                              case 4: break; // SEZ - Set Z
                                              default: break;
                                            }
                                    case 7: break;  // SEN - Set N
                                  }
                          case 3: break; // SWAB dst - swap bytes of word at dst
                          case 4: switch(instruction_5)
                                  {
                                    case 0: break;  // BR loc - unconditional branch
                                    case 1: break;  // BMI loc
                                    default: break;
                                  }
                          default: break;
                        }
                case 1: switch(instruction_2)
                        {
                          case 0: switch(instruction_5)
                                  {
                                    case 0: break;  // BNE loc
                                    case 1: break;  // BHI loc
                                    default: break;
                                  }
                          case 4: switch(instruction_5)
                                  {
                                    case 0: break;  // BEQ loc
                                    case 1: break;  // BLOS loc
                                    default: break;
                                  }
                          default: break;
                        }
                case 2: switch(instruction_2)
                        {
                          case 0: switch(instruction_5)
                                  {
                                    case 0: break;  // BGE loc
                                    case 1: break;  // BVC loc
                                    default: break;
                                  }
                          case 4: switch(instruction_5)
                                  {
                                    case 0: break;  // BLT loc
                                    case 1: break;  // BVS loc
                                    default: break;
                                  }
                          default: break;
                        }
                case 3: switch(instruction_2)
                        {
                          case 0: switch(instruction_5)
                                  {
                                    case 0: break;  // BGT loc
                                    case 1: break;  // BCC loc
                                    default: break;
                                  }
                          case 4: switch(instruction_5)
                                  {
                                    case 0: break;  // BLE loc
                                    case 1: break;  // BCS loc
                                    default: break;
                                  }
                          default: break;
                        }
                case 4: break;  // JSR reg, dst
                case 5: switch(instruction_5)
                        {
                          case 0: switch(instruction_2)
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
                          case 1: switch(instruction_2)
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
                case 6: switch(instruction_5)
                        {
                          case 0: switch(instruction_2)
                                  {
                                    case 0: break;  // ROR dst
                                    case 1: break;  // ROL dst
                                    case 2: break;  // ASR dst
                                    case 3: break;  // ASL dst
                                    default: break;
                                  }
                          case 1: switch(instruction_2)
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

    if(instruction_4 > 0)
    {
      switch(instruction_5)
      {
        case 0: switch(instruction_4)
                {
                  case 1: break;  // MOV src, dst
                  case 2: break;  // CMP src, dst
                  case 3: break;  // BIT src, dst
                  case 4: break;  // BIC src, dst
                  case 5: break;  // BIS src, dst
                  case 6: break;  // ADD src, dst
                  default: break;
                }
        case 1: switch(instruction_4)
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







  /*
   * Register addressing modes
   * 
   * Mode 0 - General Register
   * Mode 1 - Deferred Register
   * Mode 6 - Indexed
   * Mode 7 - Deferred Indexed
   * Mode 2 - Autoincrement
   * Mode 3 - Autoincrement Deferred
   * Mode 4 - Autodecrement
   * Mode 5 - Autodecrement Deferred
   */

  /*
   * PC addressing modes
   *
   * Mode 2 - Immediate
   * Mode 3 - Absolute
   * Mode 6 - Relative
   * Mode 7 - Relative Deferred
   */

  /*
   * BEGIN INSTRUCTION EXECUTE
   */

  // Write the new PC value
  memory->Write(PC, pc);

  return 0;
}

/*
 * PC addressing modes
 *
 * Mode 2 - Immediate
 * Mode 3 - Absolute
 * Mode 6 - Relative
 * Mode 7 - Relative Deferred
 */

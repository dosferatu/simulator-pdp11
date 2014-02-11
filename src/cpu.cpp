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
  //short instructionBits[6];   // Dissected instruction word
  //auto address = [=] (const int i) { return (instructionBits[i] << 3) + instructionBits[i - 1]; };
  //auto update_flags = [=] (const int i, const int bit) 
  //{
    //if (i == 0) { 
      //short temp = memory->Read(PS);
      //temp = temp & ~(bit);
      //memory->Write(PS, temp);
    //}
    //else {
      //short temp = memory->Read(PS);
      //temp = temp | bit;
      //memory->Write(PS, temp);
    //}
  //};
  //auto resultIsZero = [=] (const int result) { result == 0? update_flags(1,Zbit) : update_flags(0,Zbit); };  // Update Zbit
  //auto resultLTZero = [=] (const int result) { result < 0? update_flags(1,Nbit) : update_flags(0,Nbit); };  // Update Nbit



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
  //instructionBits[0] = (instruction & 0000007);
  //instructionBits[1] = (instruction & 0000070) >> 3; 
  //instructionBits[2] = (instruction & 0000700) >> 6;
  //instructionBits[3] = (instruction & 0007000) >> 9;
  //instructionBits[4] = (instruction & 0070000) >> 12;
  //instructionBits[5] = (instruction & 0700000) >> 15; // This is actually only giving us the final bit of the 16-bit word
  //short tmp;      // Use for holding temperary values for instruction operations
  //short src_temp; // Stores the working value of the src register
  //short dst_temp; // Stores the working value of the dst register

  //if(instructionBits[4] == 0) 
  //{
    //switch(instructionBits[3])
    //{
      //case 0: switch(instructionBits[2])
              //{
                //case 0: switch(instructionBits[5])
                        //{
                          //case 0: switch(instructionBits[0])
                                  //{
                                    //case 0: return 0; // HALT
                                    //case 1: return 1; // WAIT
                                    //case 5: return 5; // RESET
                                    //default: break;
                                  //}
                          //case 1: { // BPL loc (loc is an offset dependent on ABS or REL addressing
                                    //dst_tmp = EA(address(dst));   // Get effective address for destination
                                    //if (memory->Read(PS) & Nbit)  // N = 0? loc --> (PC)
                                    //{
                                      //// Needs some work ***********************
                                      //if(memory->Read(PC) & Negbit) 
                                      //{
                                        //dst_tmp = dst_tmp + memory->Read();
                                        //memory->Write(PC, dst_tmp * 2);
                                      //}
                                    //}

                                    //return instruction;
                                    //default: break;
                                  //}
                          //case 1: { // JMP dst
                                    //dst_tmp = EA(address(dst)); // Get effective address
                                    //memory->Write(PC, dst_tmp); // effective address --> (PC) unconditional
                                    //break;
                                  //}
                          //case 2: switch(instructionBits[1])
                                  //{
                                    //case 0: { // RTS reg
                                              //tmp = (01 << 3) | instructionBits[0];
                                              //memory->Write(PC, EA(tmp));                 // reg --> (PC)
                                              //memory->Write(EA(tmp),memory->StackPop());  // pop reg
                                              //return instruction;
                                            //}
                                            //break;
                                    //case 4: switch(instructionBits[0])
                                            //{
                                              //case 1: { update_flags (0, Cbit); return instruction; } // CLC - Clear C
                                              //case 2: { update_flags (0, Vbit); return instruction; } // CLV - Clear V
                                              //case 4: { update_flags (0, Zbit); return instruction; } // CLZ - Clear Z
                                              //default: break;
                                            //}
                                    //case 5: { update_flags (0, Nbit); return instruction; }  // CLN - Clear N
                                    //case 6: switch(instructionBits[0])
                                            //{
                                              //case 1: { update_flags (1, Cbit); return instruction; } // SEC - Set C
                                              //case 2: { update_flags (1, Vbit); return instruction; } // SEV - Set V
                                              //case 4: { update_flags (1, Zbit); return instruction; } // SEZ - Set Z
                                              //default: break;
                                            //}
                                    //case 7: { update_flags (1, Nbit); return instruction; } // SEN - Set N
                                  //}
                          //case 3: { // SWAB dst - swap bytes of word at dst
                                    //tmp_dst = EA(address(dst));             // Get effective address
                                    //tmp = memory->Read(tmp_dst);            // Get value at effective address
                                    //short byte_temp = tmp << 8;             // Create temp and give it LSByte of value in MSByte
                                    //tmp = (tmp >> 8) & 0x00FF;              // Shift MSByte into LSByte and clear MSByte
                                    //tmp = byte_temp + tmp;                  // Finalize the swap byte
                                    //memory->Write(tmp_dst, byte_temp);      // Write to register
                                    //return instruction;
                                  //}
                          //case 4: switch(instructionBits[5])
                                  //{
                                    //case 0: { // BR loc - unconditional branch
                                              //tmp = address(dst);           // Get offset
                                              //// Need to figure out the offset stuff
                                              //return instruction;
                                            //}
                                    //case 1: { // BMI loc - Branch on minus
                                              //tmp = address(dst);           // Get offset
                                              //return instruction;
                                            //}
                                    //default: break;
                                  //}
                          //default: break;
                        //}
                //case 1: switch(instructionBits[2])
                        //{
                          //case 0: switch(instructionBits[5])
                                  //{
                                    //case 0: { // BNE loc - Branch if Not Equal (zero)
                                              //tmp = address(dst);
                                              //return instruction;
                                            //}
                                    //case 1: { // BHI loc - Branch if Higher (neither Z nor C set)
                                              //tmp = address(dst);
                                              //return instruction;  // BHI loc
                                            //}
                                    //default: break;
                                  //}
                          //case 4: switch(instructionBits[5])
                                  //{
                                    //case 0: { // BEQ loc - Brach if EQual (zero)
                                              //tmp = address(dst);
                                              //return instruction;
                                            //}
                                    //case 1: { // BLOS loc - Branch if Lower Or Same (either C or Z set)
                                              //tmp = address(dst);
                                              //return instruction;
                                            //}
                                    //default: break;
                                  //}
                          //default: break;
                        //}
                //case 2: switch(instructionBits[2])
                        //{
                          //case 0: switch(instructionBits[5])
                                  //{
                                    //case 0: { // BGE loc - Branch if Greater or Equal (zero)
                                              //tmp = address(dst);
                                              //return instruction;
                                            //}
                                    //case 1: { // BVC loc - Branch if oVerflow Clear
                                              //tmp = address(dst);
                                              //return instruction;
                                            //}
                                    //default: break;
                                  //}
                          //case 4: switch(instructionBits[5])
                                  //{
                                    //case 0: { // BLT loc - Branch if Less Than (zero)
                                              //tmp = address(dst);
                                              //return instruction;
                                            //}
                                    //case 1: { // BVS loc - Branch if oVerflow Set
                                              //tmp = address(dst);
                                              //return instruction;
                                            //}
                                    //default: break;
                                  //}
                          //default: break;
                        //}
                //case 3: switch(instructionBits[2])
                        //{
                          //case 0: switch(instructionBits[5])
                                  //{
                                    //case 0: { // BGT loc - Branch if Greater Than
                                              //tmp = address(dst);
                                              //return instruction;
                                            //}
                                    //case 1: { // BCC loc - Branch if Carry Clear (C = 0)
                                              //tmp = address(tmp);
                                              //return instruction;
                                            //}
                                    //default: break;
                                  //}
                          //case 4: switch(instructionBits[5])
                                  //{
                                    //case 0: { // BLE loc - Branch if Lower or Equal (zero)
                                              //tmp = address(dst);
                                              //return instruction;
                                            //}
                                    //case 1: { // BCS loc - Branch if Carry Set (C = 1)
                                              //tmp = address(dst);
                                              //return instruction;
                                            //}
                                    //default: break;
                                  //}
                          //default: break;
                        //}
                //case 4: { // JSR reg, dst
                          //// Need to figure this out
                          //return instruction;
                        //}
                //case 5: switch(instructionBits[5])
                        //{
                          //case 0: switch(instructionBits[2])
                                  //{
                                    //case 0: { // CLR dst - Clear Destination
                                              //tmp_dst = EA(address(dst));   // Get effective address
                                              //memory->Write(tmp_dst, 0);    // Clear value at address
                                              //update_flags(1,Zbit);         // Set Z bit - No change to other condition codes
                                              //return instruction;
                                            //}
                                    //case 1: { // COM dst - Compliment Destination
                                              //tmp_dst = EA(address(dst));   // Get effective address
                                              //tmp = memory->Read(tmp_dst);  // Get value at address
                                              //tmp = ~tmp;                   // Compliment value
                                              //memory->Write(tmp_dst, tmp);  // Write compiment to memory
                                              //resultIsZero(tmp);            // Update Z bit
                                              //tmp & 0x80 == 1? update_flags(1,Nbit) : update_flags(0,Nbit); // Update N bit
                                              //update_flags(1,Cbit);         // Update C bit
                                              //update_flags(0,Vbit);         // Update V bit
                                              //return instruction;
                                            //}
                                    //case 2: { // INC dst - Increment Destination
                                              //tmp_dst = EA(address(dst));   // Get effective address
                                              //tmp = memory->Read(tmp_dst);  // Get value at address
                                              //tmp == 0077777? update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                                              //tmp++;                        // Increment value
                                              //memory->Write(tmp_dst, tmp)   // Write to memory
                                                //resultIsZero(tmp);            // Update Z bit
                                              //resultLTZero(tmp);            // Update N bit
                                              //return instruction;
                                            //}
                                    //case 3: { // DEC dst - Decrement Destination
                                              //tmp_dst = EA(address(dst));   // Get effective address
                                              //tmp = memory->Read(tmp_dst);  // Get value at address
                                              //tmp--;                        // Decrement value
                                              //memory->Write(tmp_dst, tmp);  // Write to memory
                                              //resultIsZero(tmp);            // Update Z bit
                                              //resultLTZero(tmp);            // Update N bit
                                              //update_flags(0,Cbit);         // Update C bit
                                              //update_flags(0,Vbit);         // Update V bit
                                              //return instruction;
                                            //}
                                    //case 4: { // NEG dst - Gives 2's Compliment of destination
                                              //tmp_dst = EA(address(dst));   // Get effective address
                                              //tmp = memory->Read(tmp_dst);  // Get value at address
                                              //tmp = ~tmp + 1;               // Get 2's comp of value
                                              //memory->Write(tmp_dst,tmp);   // Write to memory
                                              //resultIsZero(tmp);            // Update Z bit
                                              //resultLTZero(tmp);            // Update N bit
                                              //tmp == 0? update_flags(0,Cbit) : update_flags(1,Cbit);        // Update C bit
                                              //tmp == 0100000? update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                                              //return instruction;
                                            //}
                                    //case 5: { // ADC - Add the compliment to destination
                                              //tmp_dst = EA(address(dst));   // Get effective address
                                              //tmp = memory->Read(tmp_dst);  // Get value at address
                                              //short tmpC = memory->Read(PS);// Get current value of PS
                                              //tmp = tmp + (tmpC & 0x01);    // Add C bit to value
                                              //memory->Write(tmp_dst,tmp);   // Write to memory
                                              //resultIsZero(tmp);            // Update Z bit
                                              //resultLTZero(tmp);            // Update N bit
                                              //tmp == 0200000? update_flags(1,Cbit) : update_flags(0,Cbit);  // Update C bit
                                              //tmp == 0100000? update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                                              //return instruction;
                                            //}
                                    //case 6: return instruction;  // SBC dst
                                    //case 7: return instruction;  // TST dst
                                    //default: break;
                                  //}
                          //case 1: switch(instructionBits[2])
                                  //{

                                    //case 0: return instruction;  // CLRB dst
                                    //case 1: return instruction;  // COMB dst
                                    //case 2: return instruction;  // INCB dst
                                    //case 3: return instruction;  // DECB dst
                                    //case 4: return instruction;  // NEGB dst
                                    //case 5: return instruction;  // ADCB dst
                                    //case 6: return instruction;  // SBCB dst
                                    //case 7: return instruction;  // TSTB dst
                                    //default: break;
                                  //}
                          //default: break;
                        //}
                //case 6: switch(instructionBits[5])
                        //{
                          //case 0: switch(instructionBits[2])
                                  //{
                                    //case 0: return instruction;  // ROR dst
                                    //case 1: return instruction;  // ROL dst
                                    //case 2: return instruction;  // ASR dst
                                    //case 3: return instruction;  // ASL dst
                                    //default: break;
                                  //}
                          //case 1: switch(instructionBits[2])
                                  //{
                                    //case 0: return instruction;  // RORB dst
                                    //case 1: return instruction;  // ROLB dst
                                    //case 2: return instruction;  // ASRB dst
                                    //case 3: return instruction;  // ASLB dst
                                    //default: break;
                                  //}
                          //default: break;
                        //}
                //default: break;
              //}

              //if(instructionBits[4] > 0)
              //{
                //switch(instructionBits[5])
                //{
                  //case 0: switch(instructionBits[4])
                          //{
                            //case 1: return instruction;  // MOV src, dst
                            //case 2: return instruction;  // CMP src, dst
                            //case 3: return instruction;  // BIT src, dst
                            //case 4: return instruction;  // BIC src, dst
                            //case 5: return instruction;  // BIS src, dst
                            //case 6: return instruction;  // ADD src, dst
                            //default: break;
                          //}
                  //case 1: switch(instructionBits[4])
                          //{
                            //case 1: return instruction;  // MOVB src, dst
                            //case 2: return instruction;  // CMPB src, dst
                            //case 3: return instruction;  // BITB src, dst
                            //case 4: return instruction;  // BICB src, dst
                            //case 5: return instruction;  // BISB src, dst
                            //case 6: return instruction;  // ADDB src, dst
                            //default: break;
                          //}
                  //default: break;
                //}
              //}
    //}

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

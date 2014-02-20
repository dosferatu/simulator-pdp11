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

CPU::CPU(Memory *memory)/*{{{*/
{
  this->debugLevel = Verbosity::off;
  this->instructionCount = 0;
  this->memory = memory;
}
/*}}}*/

CPU::~CPU()/*{{{*/
{
  delete this->memory;
}
/*}}}*/

/*
 * Takes in the program counter register value in order to
 * be able to fetch, decode, and execute the next instruction
 * .
 */ 
int CPU::FDE()/*{{{*/
{
  unsigned short instruction = 0;   // Instruction word buffer
  unsigned short iB[6];             // Dissected instruction word

  // Lambda declarations and definitions/*{{{*/
  auto address = [&] (const int i) { return (iB[i] << 3) + iB[i - 1]; };
  auto update_flags = [&] (unsigned short i, unsigned short bit) 
  {
    if (i == 0) { 
      unsigned short temp = memory->ReadPS();
      temp = temp & ~(bit);
      memory->WritePS(temp);
    }
    else {
      unsigned short temp = memory->ReadPS();
      temp = temp | bit;
      memory->WritePS(temp);
    }
  };
  auto resultIsZero = [&] (unsigned short result) \
  { result == 0? update_flags(1,Zbit) : update_flags(0,Zbit); };  // Update Zbit where result is zero
  auto resultLTZero = [&] (unsigned short result) { result & WORD? \
    update_flags(1,Nbit) : update_flags(0,Nbit); };                 // Update Nbit where result is negative
  auto resultMSBIsOne = [&] (unsigned short result) { result >> 15 > 0? \
    update_flags(1,Nbit) : update_flags(0,Nbit); };                 // Update Nbit where result MSB is 1 (negative)
  auto NOP = [] () {;}; // For NOPping
  /*}}}*/

  // Instruction fetch/*{{{*/

  // Fetch the instruction and increment PC
  instruction = this->memory->ReadInstruction();
  ++this->instructionCount;

  // Optional instruction fetch state dump/*{{{*/
  if (debugLevel == Verbosity::verbose)
  {
    std::cout << "********************************************************************************" << std::endl;
    std::cout << "                                        BREAK" << std::endl;
    std::cout << "********************************************************************************" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "********************************************************************************" << std::endl;
    std::cout << std::endl;
    std::cout << "                              INSTRUCTION #" << std::dec << this->instructionCount << std::endl;
    std::cout << std::endl;
    std::cout << "********************************************************************************" << std::endl;
    std::cout << "Fetched instruction: " << std::oct << instruction << std::endl;
    std::cout << std::endl;
    this->memory->RegDump();
  }
  /*}}}*/
  /*}}}*/

  // Decode & execute/*{{{*/
  iB[0] = (instruction & 0000007);
  iB[1] = (instruction & 0000070) >> 3; 
  iB[2] = (instruction & 0000700) >> 6;
  iB[3] = (instruction & 0007000) >> 9;
  iB[4] = (instruction & 0070000) >> 12;
  iB[5] = (instruction & 0700000) >> 15; // This is actually only giving us the final bit of the 16-bit word
  unsigned short tmp      = 0; // Use for holding temperary values for instruction operations
  unsigned short offset   = 0; // Used to hold the PC value for offsetting in cases of branching
  unsigned short src_temp = 0; // Used for storing temporary source values
  unsigned short dst_temp = 0; // Used for storing temporary destination values

  //condition bit operation, system instruction, or branch/*{{{*/
  if(iB[4] == 0 && iB[3] <= 3)
  {
    //condition bit operation or system instruction/*{{{*/
    if(!iB[5] && !iB[4] && !iB[3] && !(iB[2] & 04))
    {
      if(iB[2] == 0) //then system instruction/*{{{*/
      {
        switch(iB[0])
        {
          case 0:
            {
              return 0; //HALT
            }
          case 1:
            {
              return 1; // WAIT
            }
          case 5:
            {
              return 5; // RESET
            }
          default:
            break;
        }
      }/*}}}*/

      else if(iB[2] == 1)/*{{{*/
      {
        //JUMP
        dst_temp = memory->Read(address(dst));      // Get address JMP
        memory->Write(007, dst_temp);               // Put in PC
        return instruction;
      }/*}}}*/

      else if(iB[2] == 2)	//Condition code operation/*{{{*/
      {
        switch(iB[1])
        {
		  case 0:
			{
				// RTS reg
				tmp = memory->Read(address(dst));                 // Read register value
                memory->Write(007, tmp);                          // reg --> (PC)
				tmp = memory->Read(026);                        // Push value of reg onto stack
                memory->Write(address(dst),tmp);   // pop reg
                return instruction;
			}
          case 4:
            {
              switch(iB[0])
              {
                case 1:
                  {
                    //Clear C
                    update_flags (0, Cbit); // CLC 
                    return instruction;
                  }
                case 2:
                  {
                    //Clear V
                    update_flags (0, Vbit); // CLV
                    return instruction;
                  }
                case 4:
                  {
                    //Clear Z
                    update_flags (0, Zbit); // CLZ
                    return instruction;
                  }
                default:
                  break;
              }
            }
          case 5:
            {
              if(iB[0] == 0)
              {
                //Clear N
                update_flags (0, Nbit); // CLN
                return instruction;
              }
            }
          case 6:
            {
              switch(iB[0])
              {
                case 1:
                  {
                    //Set C
                    update_flags (1, Cbit); // SEC
                    return instruction;
                  }
                case 2:
                  {
                    //Set V
                    update_flags (1, Vbit); // SEV
                    return instruction;
                  }
                case 4:
                  {
                    //Set Z
                    update_flags (1, Zbit); // SEZ
                    return instruction;
                  }
                default:
                  break;
              }
            }
          case 7:
            {
              if(iB[0] == 0)
              {
                //Set N
                update_flags (1, Nbit); // SEN
                return instruction;
              }
            }
          default:
            break;
        }
      }/*}}}*/

      else if(iB[2] == 3)/*{{{*/
      {
        //SWAB
        tmp = memory->Read(address(dst));       // Get value at effective address // SWAB
        unsigned short byte_temp = tmp << 8;    // Create temp and give it LSByte of value in MSByte
        tmp = (tmp >> 8) & 0x00FF;             // Shift MSByte into LSByte and clear MSByte
		(tmp == 0)?	\ 
		update_flags(1,Zbit): update_flags(0,Zbit);	//Update Z bit based on low order byte
		((tmp & 0x0080) == 0x0080)? \				
		update_flags(1,Nbit): update_flags(0,Nbit); //Update N bit based on low order byte
        tmp = byte_temp + tmp;                  // Finalize the swap byte
        memory->Write(address(dst), byte_temp); // Write to register
		
        update_flags(0,Cbit);              // Set C bit
		update_flags(0,Vbit);              // Set V bit
        return instruction;
      }/*}}}*/
    }/*}}}*/

    //branch operations/*{{{*/
    else if((iB[3] > 0) || (!iB[3] && (iB[5] || iB[2] > 3)))
    {
      switch(iB[3])/*{{{*/
      {
        case 0:	//BPL, BR, or BMI/*{{{*/
          {
            if(iB[5]) //BPL or BMI
            {
              if(iB[2] >= 3)
              {
                //BMI
                offset = (instruction & 0x00FF);          // Get address for branch BMI
                tmp = memory->Read(007);                  // Get current address in PC
                offset = offset << 1;                     // Multiply offset by 2
                if (offset & 0400)                        // Check if neg
                  offset = offset | (07777000);           // Pad with ones
                tmp = tmp + offset;                       // Get new address for branch
                (memory->ReadPS() & Nbit) > 0? \
                  memory->Write(007,tmp) : NOP();         // N = 1
                return instruction;
              }
              else if(iB[2] < 3)
              {
                //BPL
                offset = (instruction & 0x00FF);      // Get address for branch BPL
                tmp = memory->Read(007);              // Get current address in PC
                offset = offset << 1;                 // Multiply offset by 2
                if (offset & 0400)                    // Check if neg
                  offset = offset | (07777000);       // Pad with ones
                tmp = tmp + offset;                   // Get new address for branch
                (memory->ReadPS() & Nbit) == 0? \
                  memory->Write(007,tmp) : NOP();     // N = 0
                return instruction;
              }
            }
            else if(!iB[5] && iB[2] > 3)
            {
              //BR
              offset = (instruction & 0x00FF);          // Get address for branch BR
              tmp = memory->Read(007);                  // Get current address in PC
              offset = offset << 1;                     // Multiply offset by 2
              if (offset & 0400)                        // Check if neg
                offset = offset | (07777000);           // Pad with ones
              tmp = tmp + offset;                       // Get new address for branch
              memory->Write(007,tmp);                   // Branch always
              return instruction;
            }
          }/*}}}*/

        case 1: //BNE, BHI, BLOS, BEQ/*{{{*/
          {
            if(iB[5]) //BHI or BLOS
            {
              if(iB[2] <= 3)
              {
                //BHI
                offset = (instruction & 0x00FF);          // Get address for branch BHI
                tmp = memory->Read(007);                  // Get current address in PC
                offset = offset << 1;                     // Multiply offset by 2
                if (offset & 0400)                        // Check if neg
                  offset = offset | (07777000);           // Pad with ones
                tmp = tmp + offset;                       // Get new address for branch
                (memory->ReadPS() & (Zbit | Cbit)) == 0? \
                  memory->Write(007,tmp) : NOP();          // C & Z = 0
                return instruction;
              }
              else if(iB[2] > 3)
              {
                //BLOS
                offset = (instruction & 0x00FF);          // Get address for branch BLOS
                tmp = memory->Read(007);                  // Get current address in PC
                offset = offset << 1;                     // Multiply offset by 2
                if (offset & 0400)                        // Check if neg
                  offset = offset | (07777000);           // Pad with ones
                tmp = tmp + offset;                       // Get new address for branch
                (memory->ReadPS() & (Zbit | Cbit)) > 0? \
                  memory->Write(007,tmp) : NOP();         // C | Z = 1
                return instruction;
              }
            }
            else if (!iB[5]) //BNE or BEQ
            {
              if(iB[2] <= 3)
              {
                //BNE
                offset = (instruction & 0x00FF);          // Get address for branch BNE
                tmp = memory->Read(007);                  // Get current address in PC
                offset = offset << 1;                     // Multiply offset by 2
                if (offset & 0400)                        // Check if neg
                  offset = offset | (07777000);           // Pad with ones
                tmp = tmp + offset;                       // Get new address for branch
                (memory->ReadPS() & Zbit) == 0? \
                  memory->Write(007,tmp) : NOP();         // Z = 0
                return instruction;
              }
              else if(iB[2] > 3)
              {
                //BEQ
                offset = (instruction & 0x00FF);          // Get address for branch
                tmp = memory->Read(007);                // Get current address in PC
                offset = offset << 1;                   // Multiply offset by 2
                if (offset & 0400)                      // Check if neg
                  offset = offset | (07777000);        // Pad with ones
                tmp = tmp + offset;                     // Get new address for branch
                (memory->ReadPS() & Zbit) > 0? \
                  memory->Write(007,tmp) : NOP();          // Z = 0
                return instruction;
              }	
            }
          }/*}}}*/

        case 2:	//BVC, BGE, BVS, BLT/*{{{*/
          {
            if(iB[5]) //BVC or BVS
            {
              if(iB[2] <= 3)
              {
                //BVC
                offset = (instruction & 0x00FF);          // Get address for branch BVC
                tmp = memory->Read(007);                  // Get current address in PC
                offset = offset << 1;                     // Multiply offset by 2
                if (offset & 0400)                        // Check if neg
                  offset = offset | (07777000);           // Pad with ones
                offset = tmp + offset;                       // Get new address for branch
                (memory->ReadPS() & Vbit) == 0? \
                  memory->Write(007,offset) : NOP();          // V = 0
                return instruction;
              }
              else if(iB[2] > 3)
              {
                //BVS
                offset = (instruction & 0x00FF);          // Get address for branch BVS
                tmp = memory->Read(007);                  // Get current address in PC
                offset = offset << 1;                     // Multiply offset by 2
                if (offset & 0400)                        // Check if neg
                  offset = offset | (07777000);           // Pad with ones
                offset = tmp + offset;                       // Get new address for branch
                (memory->ReadPS() & Vbit) == Vbit? \
                  memory->Write(007,offset) : NOP();          // V = 1
                return instruction;
              }
            }
            else if (!iB[5]) //BGE or BLT
            {
              if(iB[2] <= 3)
              {
                //BGE
                offset = (instruction & 0x00FF);          // Get address for branch
                tmp = memory->Read(007);                // Get current address in PC
                offset = offset << 1;                   // Multiply offset by 2
                if (offset & 0400)                      // Check if neg
                  offset = offset | (07777000);        // Pad with ones
                offset = tmp + offset;                     // Get new address for branch
                (((memory->ReadPS() & Nbit) >> 3) ^ ((memory->ReadPS() & Vbit) >> 1)) == 0? \
                  memory->Write(007,offset) : NOP();          // N ^ V = 0
                return instruction;
              }
              else if(iB[2] > 3)
              {
                //BLT
                offset = (instruction & 0x00FF);          // Get address for branch
                tmp = memory->Read(007);                // Get current address in PC
                offset = offset << 1;                   // Multiply offset by 2
                if (offset & 0400)                      // Check if neg
                  offset = offset | (07777000);        // Pad with ones
                offset = tmp + offset;                     // Get new address for branch
                (((memory->ReadPS() & Nbit) >> 3) ^ ((memory->ReadPS() & Vbit) >> 1)) == 1? \
                  memory->Write(007,offset) : NOP();          // N ^ V == 1
                return instruction;
              }	
            }
          }/*}}}*/

        case 3:	//BGT, BCC, BLE, or BCS/*{{{*/
          {
            if(iB[5]) //BCC or BCS
            {
              if(iB[2] <= 3)
              {
                //BCC
                offset = (instruction & 0x00FF);        // Get address for branch BCC
                tmp = memory->Read(007);                // Get current address in PC
                offset = offset << 1;                   // Multiply offset by 2
                if (offset & 0400)                      // Check if neg
                  offset = offset | (07777000);         // Pad with ones
                offset = tmp + offset;                     // Get new address for branch
                (memory->ReadPS() & Cbit) == 0? \
                  memory->Write(007,offset) : NOP();       // C = 0
                return instruction;
              }
              else if(iB[2] > 3)
              {
                //BCS
                offset = (instruction & 0x00FF);        // Get address for branch BCS
                tmp = memory->Read(007);                // Get current address in PC
                offset = offset << 1;                   // Multiply offset by 2
                if (offset & 0400)                      // Check if neg
                  offset = offset | (07777000);         // Pad with ones
                offset = tmp + offset;                     // Get new address for branch
                ((memory->ReadPS() & Cbit) == Cbit)? \
                  memory->Write(007,offset) : NOP();    // C = 1
                return instruction;
              }
            }
            else if (!iB[5]) //BLE or BGT
            {
              if(iB[2] <= 3)
              {
                //BGT
                offset = (instruction & 0x00FF);          // Get address for branch BGT
                tmp = memory->Read(007);                  // Get current address in PC
                offset = offset << 1;                     // Multiply offset by 2
                if (offset & 0400)                        // Check if neg
                  offset = offset | (07777000);           // Pad with ones
                offset = tmp + offset;                       // Get new address for branch
                (((memory->ReadPS() & Zbit) >> 2) | \
                 (((memory->ReadPS() & Nbit) >> 3) ^ ((memory->ReadPS() & Vbit) >> 1))) == 0? \
                  memory->Write(007,offset) : NOP();          // Z | (N ^ V) = 0
                return instruction;
              }
              else if(iB[2] > 3)
              {
                //BLE
                offset = (instruction & 0x00FF);          // Get address for branch BLE
                tmp = memory->Read(007);                  // Get current address in PC
                offset = offset << 1;                     // Multiply offset by 2
                if (offset & 0400)                        // Check if neg
                  offset = offset | (07777000);           // Pad with ones
                offset = tmp + offset;                    // Get new address for branch
                tmp = memory->ReadPS();                   // Get current process status
                (((tmp & Zbit) >> 2) | (((tmp & Nbit) >> 3) ^ ((tmp & Vbit) >> 1))) == 1? \
                  memory->Write(007,offset) : NOP();      // Z(N^V) = 1
                return instruction;
              }	
            }	
          }/*}}}*/

        default:
          break;
      }/*}}}*/
    }/*}}}*/
  }/*}}}*/

  // Single Operand Instructions (not including condition code instructions or branches)/*{{{*/
  else if ((iB[5] == 0) && (iB[4] == 0) && (iB[3] >= 4)) // Single Operand Word Operations
  {
    switch(iB[3])
    {
      // JSR reg, dst /*{{{*/
      case 4:
        { 
          tmp = memory->EA(address(dst));                 // Get address to jump to    JSR
          unsigned short reg = memory->Read(iB[2] & 007); // Get value of reg to store
          memory->Write(046, reg);                        // Push value of reg onto stack
          reg = memory->Read(007);                        // Get value from PC
          memory->Write((iB[2] & 007),reg);               // Write PC value to register
          memory->Write(007,tmp - 02);                         // Write new address to PC
          return instruction;
        }/*}}}*/

        //Case 5 /*{{{*/
      case 5:
        {
          switch(iB[2])
          {
            case 0: { // CLR dst - Clear Destination
                      memory->Write(address(dst), 0);    // Clear value at address CLR
                      update_flags(1,Zbit);              // Set Z bit
                      update_flags(0,Nbit);              // Set N bit
                      update_flags(0,Cbit);              // Set C bit
                      update_flags(0,Vbit);              // Set V bit
                      return instruction;
                    }
            case 1: { // COM dst: ~(dst) -> (dst)
                      tmp = memory->Read(address(dst));  // Get value at address COM
                      tmp = ~tmp;                        // Compliment value
                      memory->Write(address(dst), tmp);  // Write compiment to memory
                      resultIsZero(tmp);                 // Update Z bit
                      resultLTZero(tmp);                 // Update N bit
                      update_flags(1, Cbit);			 // Update C bit
                      update_flags(0, Vbit);			 // Update V bit
                      return instruction;
                    }
            case 2: { // INC dst: (dst)++ -> (dst)
                      dst_temp = memory->Read(address(dst)); // Get value at address INC
                      tmp = dst_temp + 1;                // Increment value
                      memory->Write(address(dst), tmp);  // Write to memory
                      resultIsZero(tmp);                 // Update Z bit
                      resultLTZero(tmp);                 // Update N bit
                      // C bit not affected
                      dst_temp == 0077777? \
                        update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                      return instruction;
                    }
            case 3: { // DEC dst: (dst)-- -> (dst)
                      dst_temp = memory->Read(address(dst));  // Get value at address DEC
                      tmp = dst_temp - 1;                     // Decrement value
                      memory->Write(address(dst), tmp);       // Write to memory
                      resultIsZero(tmp);                      // Update Z bit
                      resultLTZero(tmp);                      // Update N bit
                      //C bit not affected (typo in handbook)
					  dst_temp == 0100000? \
                        update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                      return instruction;
                    }
            case 4: { // NEG dst: -(dst) -> (dst) 
                      tmp = memory->Read(address(dst));       // Get value at address NEG
                      tmp = ~tmp + 1;                         // Get 2's comp of value
                      memory->Write(address(dst),tmp);        // Write to memory
                      resultIsZero(tmp);                      // Update Z bit
                      resultLTZero(tmp);                      // Update N bit
                      tmp == 0? update_flags(0,Cbit) : update_flags(1,Cbit);          // Update C bit
                      tmp == 0100000? update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                      return instruction;
                    }
            case 5: { // ADC: (dst) + (C) -> (dst)
                      dst_temp = memory->Read(address(dst));    // Get value at address ADC
                      unsigned short tmpC = memory->ReadPS();   // Get current value of PS
                      tmpC = tmpC & 0x1;                        // Get C bit value
                      tmp = dst_temp + (tmpC);                  // Add C bit to value
                      memory->Write(address(dst),tmp);          // Write to memory
                      resultIsZero(tmp);                        // Update Z bit
                      resultLTZero(tmp);                        // Update N bit
                      ((dst_temp == 0177777) && (tmpC == 1))? \
                                 update_flags(1,Cbit) : update_flags(0,Cbit);  // Update C bit
                      ((dst_temp == 0077777) && (tmpC == 1))? update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                      return instruction;
                    }
            case 6: { // SBC: (dst) - (C) -> (dst)
                      tmp = memory->Read(address(dst));         // Get value at address SBC
                      unsigned short tmpC = memory->ReadPS();   // Get current value of PS
                      tmpC = tmpC & 0x1;                        // Get C bit value
                      tmp = tmp - tmpC;                         // Add C bit to value
                      memory->Write(address(dst),tmp);          // Write to memory
                      resultIsZero(tmp);                        // Update Z bit
                      resultLTZero(tmp);                        // Update N bit
                      (tmp == 0) && (tmpC == 1)? \
                            update_flags(0,Cbit) : update_flags(1,Cbit);  // Update C bit
                      (tmp == 0100000)? update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                      return instruction;
                    }
            case 7: { // TST dst - Tests if dst is 0 (0 - dst)
                      tmp = memory->Read(address(dst)); // Get value at address TST
                      tmp = 0 - tmp;                    // Perform test
                      resultIsZero(tmp);                // Update Z bit
                      resultLTZero(tmp);                // Update N bit
                      update_flags(0,Cbit);             // Update C bit
                      update_flags(0,Vbit);             // Update V bit   
                      return instruction;
                    }
            default: break;
          }
        }/*}}}*/
      case 6:
        {
          switch(iB[2])/*{{{*/
          {
            case 0:
              { // ROR dst: ROtate Rigtht - include C bit as MSB -> (dst)
                dst_temp = memory->Read(address(dst));  // Get value at dst
                unsigned short tempCN = memory->ReadPS() & 0x9; // Get C and N bits
                tmp = (dst_temp >> 1) | ((tempCN & 01) << 15);  // Rotate bits to the right
                memory->Write(address(dst),tmp);         // Write to memory
                resultIsZero(tmp);                      // Update Z bit
                resultLTZero(tmp);                      // Update N bit
                update_flags(dst_temp & 01,Cbit);       // Update C bit
                update_flags((tempCN >> 3) ^ (tempCN & 01),Vbit); // Update V bit - C ^ N
                return instruction;
              }
            case 1:
              { // ROL dst: ROtate Left - include C bit as LSB -> (dst)
                dst_temp = memory->Read(address(dst));  // Get value at dst
                unsigned short tempCN = (memory->ReadPS() & 0x9); // Get C and N bits
                tmp = ((dst_temp << 1) | (tempCN & 01));  // Rotate bits to the right
                memory->Write(address(dst),tmp);         // Write to memory
                resultIsZero(tmp);                      // Update Z bit
                resultLTZero(tmp);                      // Update N bit
                update_flags((dst_temp & WORD) >> 15,Cbit); // Update C bit
                update_flags((tempCN >> 3) ^ (tempCN & 01),Vbit); // Update V bit - C ^ N
                return instruction;
              }
            case 2:
              { // ASR dst: Arithmetic Shift Right
                dst_temp = memory->Read(address(dst));  // Get value at dst
                unsigned short tempCN = memory->ReadPS() & 0x9;     // Get C and N bits
				tmp = dst_temp >> 1;                    // Rotate bits to the right
                if(dst_tmp & WORD)						//If destination is negative
					dst_temp = dst_temp | WORD;			//then shift in a 1 on the end
				memory->Write(address(dst),tmp);        // Write to memory
                resultIsZero(tmp);                      // Update Z bit
                resultLTZero(tmp);                      // Update N bit
                update_flags((dst_temp & 01),Cbit);     // Update C bit LSB (result)
                update_flags((tempCN >> 3) ^ (tempCN & 01),Vbit); // Update V bit - C ^ N
                return instruction;
              }
            case 3: 
             { // ASL dst: Arithmetic Shift Left 
                dst_temp = memory->Read(address(dst));  // Get value at dst
                unsigned short tempCN = memory->ReadPS() & 0x9;    // Get C and N bits
                tmp = dst_temp << 1;                    // Rotate bits to the right
                memory->Write(address(dst),tmp);         // Write to memory
                resultIsZero(tmp);                      // Update Z bit
                resultLTZero(tmp);                      // Update N bit
                update_flags((dst_temp & WORD) >> 15,Cbit); // Update C bit
                update_flags((tempCN >> 3) ^ (tempCN & 01),Vbit); // Update V bit - C ^ N
                return instruction;
              }
            default: break;
          }/*}}}*/
        }
      default: break;
    }
  }/*}}}*/

  // Single Operand Byte Operations /*{{{*/
  else if ((iB[5] == 1) && (iB[4] == 0) && (iB[3] >= 4))
  {
    switch(iB[3])
    {
      case 5: 
        {
          switch(iB[2])/*{{{*/
          {
            case 0:
              { // CLRB dst - Clear Byte
                memory->SetByteMode();            // Set byte mode CLRB
                tmp = memory->Read(address(dst)); // Get value at dst
                tmp = tmp & 0x0;                  // Clear byte
                memory->Write(address(dst),tmp);  // Write byte to dst
				update_flags(1,Zbit);              // Set Z bit
                update_flags(0,Nbit);              // Set N bit
                update_flags(0,Cbit);              // Set C bit
                update_flags(0,Vbit);              // Set V bit
                memory->ClearByteMode();          // Clear byte mode
                return instruction;
              }
            case 1:
              { // COMB dst: ~(dst) -> (dst)
                memory->SetByteMode();            // Set byte mode COMB
                tmp = memory->Read(address(dst)); // Get value at address
                tmp = ~tmp & 0x00FF;              // Compliment value
                memory->Write(address(dst), tmp); // Write compiment to memory
                resultIsZero(tmp);                // Update Z bit
                resultLTZero(tmp << 8);           // Update N bit
                tmp == 0? update_flags(0,Cbit) : update_flags(1,Cbit);  // Update C bit
                tmp == BYTE? update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                memory->ClearByteMode();                        // Clear byte mode
                return instruction;
              }
            case 2:
              { // INCB dst: (dst)++ -> (dst)
                memory->SetByteMode();                  // Set byte mode INCB
                dst_temp = memory->Read(address(dst));  // Get value at address
                tmp = dst_temp + 1;                     // Increment value
                memory->Write(address(dst), tmp);       // Write to memory
                resultIsZero(tmp);                      // Update Z bit
                resultLTZero(tmp << 8);                 // Update N bit
                // C bit not affected
                dst_temp == 0x00FF? \
                  update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                memory->ClearByteMode();                        // Clear byte mode
                return instruction;
              }
            case 3:
              { // DECB dst: (dst)-- -> (dst)
                memory->SetByteMode();            // Set byte mode DECB
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
            case 4:
              { // NEGB dst: -(dst) -> (dst)
                memory->SetByteMode();            // Set byte mode NEGB
                tmp = memory->Read(address(dst)); // Get value at address
                tmp = (~tmp) & 0x00FF;        // Get 2's comp of value
                memory->Write(address(dst),tmp);  // Write to memory
                resultIsZero(tmp);                // Update Z bit
                resultLTZero(tmp << 8);           // Update N bit
                tmp == 0? update_flags(0,Cbit) : update_flags(1,Cbit);       // Update C bit
                tmp == BYTE? update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                memory->ClearByteMode();          // Clear byte mode
                return instruction;
              }
            case 5:
              { // ADCB: (dst) + (C) -> (dst)
                memory->SetByteMode();                  // Set byte mode ADCB
                dst_temp = memory->Read(address(dst));  // Get value at address
                unsigned short tmpC = memory->ReadPS(); // Get current value of PS
                tmpC = tmpC & 0x1;                      // Get C bit value
                tmp = (dst_temp + (tmpC)) & 0x00FF;     // Add C bit to value
                memory->Write(address(dst),tmp);        // Write to memory
                resultIsZero(tmp);                      // Update Z bit
                resultLTZero(tmp << 8);                 // Update N bit
                (dst_temp == 0x00FF) && (tmpC == 1)? \
                  update_flags(1,Cbit) : update_flags(0,Cbit);                // Update C bit
                tmp == 0x007F? update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                memory->ClearByteMode();              // Clear byte mode
                return instruction;
              }
            case 6:
              { // SBCB: (dst) - (C) -> (dst)
                memory->SetByteMode();                  // Set byte mode SBCB
                dst_temp = memory->Read(address(dst));  // Get value at address
                unsigned short tmpC = memory->ReadPS(); // Get current value of PS
                tmpC = tmpC & 0x1;                      // Get C bit value
                tmp = dst_temp - tmpC;                  // Add C bit to value
                memory->Write(address(dst),tmp);        // Write to memory
                resultIsZero(tmp);                      // Update Z bit
                resultLTZero(tmp << 8);                      // Update N bit
                (tmp == 0) && (tmpC == 1)? \
                  update_flags(0,Cbit) : update_flags(1,Cbit);  // Update C bit
                ((tmp & BYTE) >> 7) == 1? \
                  update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
                memory->ClearByteMode();          // Clear byte mode
                return instruction;
              }
            case 7:
              { // TSTB dst - Tests if dst is 0 (0 - dst)
                tmp = memory->Read(address(dst)); // Get value at address TST
                tmp = 0 - tmp;                    // Perform test
                resultIsZero(tmp);                // Update Z bit
                resultLTZero(tmp << 8);           // Update N bit
                update_flags(0,Cbit);             // Update C bit
                update_flags(0,Vbit);             // Update V bit   
                return instruction;
              }
            default: break;
          }/*}}}*/
        }
      case 6: 
        {
          switch(iB[2])/*{{{*/
          {
            case 0: 
              { // RORB dst: ROtate Rigtht - include C bit as MSB -> (dst)
                memory->SetByteMode();                            // Set byte mode RORB
                dst_temp = memory->Read(address(dst));            // Get value at dst
                unsigned short tempCN = memory->ReadPS() & 0x9;   // Get C and N bits
                tmp = (dst_temp >> 1) | ((tempCN & 01) << 7);     // Rotate bits to the right
                memory->Write(address(dst),tmp);                  // Write to memory
                resultIsZero(tmp);                                // Update Z bit
                resultLTZero(tmp << 8);                                // Update N bit
                update_flags(dst_temp & 01,Cbit);                 // Update C bit
                update_flags((tempCN >> 3) ^ (tempCN & 01),Vbit); // Update V bit - C ^ N
                memory->ClearByteMode();                          // Clear byte mode
                return instruction;
              }
            case 1:
              { // ROLB dst: ROtate Left - include C bit as LSB -> (dst)
                memory->SetByteMode();                            // Set byte mode ROLB
                dst_temp = memory->Read(address(dst));            // Get value at dst
                unsigned short tempCN = memory->ReadPS() & 0x9;   // Get C and N bits
                tmp = (dst_temp << 1) | (tempCN & 01);            // Rotate bits to the right
                memory->Write(address(dst),tmp & 0x00FF);         // Write to memory
                resultIsZero(tmp);                                // Update Z bit
                resultLTZero(tmp << 8);                                // Update N bit
                update_flags((dst_temp & BYTE) >> 7,Cbit);        // Update C bit
                update_flags((tempCN >> 3) ^ (tempCN & 01),Vbit); // Update V bit - C ^ N
                memory->ClearByteMode();                          // Clear byte mode
                return instruction;
              }
            case 2:
              { // ASRB dst: Arithmetic Shift Right
                memory->SetByteMode();                            // Set byte mode ASRB
                dst_temp = memory->Read(address(dst));            // Get value at dst
                unsigned short tempCN = memory->ReadPS() & 0x9;   // Get C and N bits
                tmp = dst_temp >> 1;                              // Rotate bits to the right
                memory->Write(address(dst),tmp);                  // Write to memory
                resultIsZero(tmp);                                // Update Z bit
                resultLTZero(tmp << 8);                                // Update N bit
                update_flags((dst_temp & 01),Cbit);               // Update C bit LSB (result)
                update_flags((tempCN >> 3) ^ (tempCN & 01),Vbit); // Update V bit - C ^ N
                memory->ClearByteMode();                          // Clear byte mode
                return instruction;
              }
            case 3: 
              { // ASLB dst: Arithmetic Shift Left 
                memory->SetByteMode();                            // Set byte mode ASLB
                dst_temp = memory->Read(address(dst));            // Get value at dst
                unsigned short tempCN = memory->ReadPS() & 0x9;   // Get C and N bits
                tmp = dst_temp << 1;                              // Rotate bits to the right
                memory->Write(address(dst),tmp & 0x00FF);         // Write to memory
                resultIsZero(tmp);                                // Update Z bit
                resultLTZero(tmp << 8);                                // Update N bit
                update_flags((dst_temp & BYTE) >> 7,Cbit);        // Update C bit
                update_flags((tempCN >> 3) ^ (tempCN & 01),Vbit); // Update V bit - C ^ N
                memory->ClearByteMode();                          // Clear byte mode
                return instruction;
              }
            default: break;
          }/*}}}*/
        }
    }
  }/*}}}*/

  // Double Operand Word Operations/*{{{*/
  else if (iB[5] == 0 && iB[4] != 6)
  {
    switch (iB[4])
    {
      // MOV (src) -> (dst)
      case 1:
        {
          src_temp = memory->Read(address(src));  // Get value at address of src MOV
          dst_temp = memory->Read(address(dst));  // Dummy read for PC convention
          memory->Write(address(dst),src_temp);   // Write value to memory
          resultIsZero(src_temp);                 // Update Z bit
          resultLTZero(src_temp);                 // Update N bit
          update_flags(0,Vbit);                   // Update V bit
          return instruction;
        }

        // CMP (src) + ~(dst) + 1
      case 2:
        {
          src_temp = memory->Read(address(src));          // Get value at address of src CMP
          dst_temp = memory->Read(address(dst));          // Get value at address of dst
          dst_temp = ~(dst_temp) + 1;                     // Get two's compliment
          int result = src_temp + dst_temp;               // Calculate result
          //tmp = src_temp - dst_temp;                    // Compare values
          resultIsZero(result);                           // Update Z bit
          resultLTZero(result);                           // Update N bit
          result & 0x10000? update_flags(0, Cbit) : update_flags(1,Cbit);	//Update C bit
          (((src_temp & WORD) == (dst_temp & WORD)) && ((dst_temp & WORD) != (result & WORD)))? \
            update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
          
          return instruction;
        }

        // BIT (src) ^ (dst)
      case 3:
        {
          tmp = memory->Read(address(src)) & memory->Read(address(dst)); // Get test value BIT
          resultIsZero(tmp);            // Update Z bit
          resultLTZero(tmp);			// Update N bit
          update_flags(0,Vbit);         // Update V bit
          return instruction;
        }

        // BIC ~(src) ^ (dst) -> (dst)
      case 4:
        {
          tmp = ~(memory->Read(address(src))) & memory->Read(address(dst)); // Get ~src & dst value BIC
          memory->Write(address(dst),tmp);                                  // Write value to dst
          resultIsZero(tmp);                                                // Update Z bit
          resultLTZero(tmp);                                              // Update N bit
          update_flags(0,Vbit);                                             // Update V bit
          return instruction;
        }
		// BIS (src) V (dst) -> (dst)
      case 5:
        {
          tmp = ((memory->Read(address(src))) | memory->Read(address(dst))); // Get ~src & dst value BIC
          memory->Write(address(dst),tmp);                                  // Write value to dst
          resultIsZero(tmp);                                                // Update Z bit
          resultLTZero(tmp);                                              // Update N bit
          update_flags(0,Vbit);                                             // Update V bit
          return instruction;
        }
    }
  }/*}}}*/

  // Double Operand Byte Operations/*{{{*/
  else if (iB[5] == 1 && iB[4] != 6)
  {
    switch (iB[4])
    {
      // MOVB (src) -> (dst)
      case 1:
        {
          memory->SetByteMode();                  // Set byte mode
          src_temp = memory->Read(address(src));  // Get value at address of src
          memory->Write(address(dst),src_temp);   // Write value to memory
          resultIsZero(src_temp);                 // Update Z bit
          resultLTZero(src_temp << 8);            // Update N bit
          update_flags(0,Vbit);                   // Update V bit
          memory->ClearByteMode();                // Clear byte mode
          return instruction;
        }

        // CMPB (src) + ~(dst) + 1
      case 2:
        {
          memory->SetByteMode();                          // Set byte mode
          src_temp = memory->Read(address(src));          // Get value at address of src
          dst_temp = memory->Read(address(dst));          // Get destination value
          tmp = src_temp + ~(dst_temp) + 1;               // Compare values
          resultIsZero(tmp);                              // Update Z bit
          resultLTZero(tmp << 8);                              // Update N bit
          (((src_temp & BYTE) & (dst_temp & BYTE)) && ((dst_temp & BYTE)^(tmp & BYTE)))? \
            update_flags(0,Cbit) : update_flags(1,Cbit);  // Update C bit
          (((src_temp & BYTE) ^ (dst_temp & BYTE)) && (~((dst_temp & WORD) ^ (tmp & BYTE)) & BYTE))? \
            update_flags(1,Vbit) : update_flags(0,Vbit);  // Update V bit
          memory->ClearByteMode();                        // Clear byte mode
          return instruction;
        }

        // BITB ~(src) ^ (dst)
      case 3:
        {
          memory->SetByteMode();        // Set byte mode
          tmp = memory->Read(address(src)) & memory->Read(address(dst)); // Get test value
          resultIsZero(tmp);            // Update Z bit
          resultLTZero(tmp << 8);		// Update N bit
          update_flags(0,Vbit);         // Update V bit
          memory->ClearByteMode();      // Clear byte mode
          return instruction;
        }

        // BICB ~(src) ^ (dst) -> (dst)
      case 4:
        {
          memory->SetByteMode();          // Set byte mode
          tmp = ~(memory->Read(address(src))) & memory->Read(address(dst)); // Get ~src & dst value
          memory->Write(address(dst),tmp);                                  // Write value to dst
          resultIsZero(tmp);                                                // Update Z bit
          resultMSBIsOne(tmp << 8);                                         // Update N bit
          update_flags(0,Vbit);                                             // Update V bit
          memory->ClearByteMode();        // Clear byte mode
          return instruction;
        }
		
		//BISB (src) V (dst) -> (dst)
	  case 5:
        {
          memory->SetByteMode();        // Set byte mode
          tmp = memory->Read(address(src)) | memory->Read(address(dst)); // Get value
          memory->Write(address(dst),tmp);                               // Write value to dst
		  resultIsZero(tmp);            // Update Z bit
          resultLTZero(tmp << 8);		// Update N bit
          update_flags(0,Vbit);         // Update V bit
          memory->ClearByteMode();      // Clear byte mode
          return instruction;
        }
    }
  }

  // ADD or SUB operation
  else if (iB[4] == 6)
  {
    // ADD
    if (iB[5] == 0)
    {
      src_temp = memory->Read(address(src));  // Get source value ADD
      dst_temp = memory->Read(address(dst));  // Get destination value
      int result = src_temp + dst_temp;       // Add src and dst
      memory->Write(address(dst),result);     // Write result to memory
      resultIsZero(result);                   // Update Z bit
      resultLTZero(result);                   // Update N bit
      (result & 0xF0000) > 0? update_flags(1,Cbit) : update_flags(0,Cbit);
      (((src_temp & WORD) == (dst_temp & WORD)) && ((result & WORD) != (dst_temp & WORD)))? \
       update_flags(1,Vbit) : update_flags(0,Vbit);
      return instruction;
    }

    // SUB
    else
    {
      dst_temp = memory->Read(address(dst));    // Get value of dst SUB
      src_temp = memory->Read(address(src));    // Get value of src
      int result = dst_temp + ~(src_temp) + 1;  // Subtract
      memory->Write(address(dst), result);         // Write to memory
      resultIsZero(result);                        // Update Z bit
      resultLTZero(result);                        // Update N bit
      (result & 0x10000)? update_flags(0,Cbit) : update_flags(1,Cbit);
      //(((src_temp & WORD) ^ (dst_temp & WORD)) & ((src_temp & WORD) & ((result & 0x10000) >> 1)))? \
        update_flags(1,Vbit) : update_flags(0,Vbit);
	  (((src_temp & WORD) != (dst_temp & WORD)) && ((result & WORD) == (src_temp & WORD)))? \
       update_flags(1,Vbit) : update_flags(0,Vbit);
      return instruction;
    }
  }

  // SHOULD NOT EVER REACH THIS POINT
  // PLUG IN WARNING
  return 0;
}
/*}}}*/
/*}}}*/
/*}}}*/

void CPU::SetDebugMode(Verbosity verbosity)/*{{{*/
{
  this->debugLevel = verbosity;
  return;
}
/*}}}*/

void CPU::ResetInstructionCount()/*{{{*/
{
  this->instructionCount = 0;
  return;
}/*}}}*/

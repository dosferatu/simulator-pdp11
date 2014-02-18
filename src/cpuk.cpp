//condition bit operation, system instruction, or branch
if(iB[4] == 0 && iB[3] <= 3)
{
	//condition bit operation or system instruction
	if(!iB[5] && !iB[4] && !iB[3] && !(iB[2] & 04))
	{
		if(iB[2] == 0) //then system instruction
		{
		switch(iB[0])
			{
				case 0:
				{
					//HALT
					return 0;
				}
				case 1:
				{
					//WAIT
					return 1;
				}
				case 5:
				{
					//RESET
					return 5;
				}
				default:
					break;
			}
		}
		else if(iB[2] == 1)
		{
			//JUMP
			dst_temp = memory->Read(address(dst));      // Get address
            memory->Write(007, dst_temp);      // Put in PC
			return instruction; //JMP
		}
		else if(iB[2] == 2)	//Condition code operation
		{
			switch(iB[1])
			{
				case 4:
				{
					switch(iB[0])
					{
						case 1:
						{
							//Clear C
							update_flags (0, Cbit); 
							return instruction;
						}
						case 2:
						{
							//Clear V
							update_flags (0, Vbit); 
							return instruction;
						}
						case 4:
						{
							//Clear Z
							update_flags (0, Zbit); 
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
						update_flags (0, Nbit); 
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
							update_flags (1, Cbit); 
							return instruction;
						}
						case 2:
						{
							//Set V
							update_flags (1, Vbit); 
							return instruction;
						}
						case 4:
						{
							//Set Z
							update_flags (1, Zbit); 
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
						update_flags (1, Nbit); 
						return instruction;
					}
				}
				default:
					break;
			}
		}
		else if(iB[2] == 3)
		{
			//SWAB
			tmp = memory->Read(address(dst));       // Get value at effective address
            unsigned short byte_temp = tmp << 8;    // Create temp and give it LSByte of value in MSByte
            tmp = (tmp >> 8) & 0000777;             // Shift MSByte into LSByte and clear MSByte
            tmp = byte_temp + tmp;                  // Finalize the swap byte
            memory->Write(address(dst), byte_temp); // Write to register
            return instruction;
		}
	}
	
	//branch operations
	else if((iB[3] > 0) || (!iB[3] && (iB[5] || iB[2] > 3)))
	{
		switch(iB[3])
		{
			case 0:	//BPL, BR, or BMI
			{
				if(iB[5]) //BPL or BMI
				{
					if(iB[2] >= 3)
					{
						//BMI
						offset = memory->Read(address(dst));  // Get address for branch
                        tmp = memory->Read(007);              // Get current address in PC
                        tmp = tmp + (offset << 2);            // Get new address for branch
                        (memory->ReadPS() & Nbit) > 0? \
                        memory->Write(007,tmp) : NOP();     // N = 1
                        return instruction;
					}
					else if(iB[2] < 3)
					{
						//BPL
						offset = memory->Read(address(dst));  // Get address for branch
                        tmp = memory->Read(007);              // Get current address in PC
                        tmp = tmp + (offset << 2);            // Get new address for branch
                        (memory->ReadPS() & Nbit) == 0? \
                        memory->Write(007,tmp) : NOP();  // N = 0
                        return instruction;
					}
				}
				else if(!iB[5] && iB[2] > 3)
				{
					//BR
					offset = memory->Read(address(dst));  // Get address for branch
                    tmp = memory->Read(007);              // Get current address in PC
                    tmp = tmp + (offset << 2);            // Get new address for branch
                    memory->Write(007,tmp);               // Branch always
                    return instruction;
				}
			}
			case 1: //BNE, BHI, BLOS, BEQ
			{
				if(iB[5]) //BHI or BLOS
				{
					if(iB[2] <= 3)
					{
						//BHI
						offset = memory->Read(address(dst));  // Get address for branch
                        tmp = memory->Read(007);              // Get current address in PC
                        tmp = tmp + (offset << 2);            // Get new address for branch
                        (memory->ReadPS() & (Zbit | Cbit)) == 0? \
                        memory->Write(007,tmp) : NOP();          // C & Z = 0
                        return instruction;
					}
					else if(iB[2] > 3)
					{
						//BLOS
						offset = memory->Read(address(dst));  // Get address for branch
                        tmp = memory->Read(007);              // Get current address in PC
                        tmp = tmp + (offset << 2);            // Get new address for branch
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
						offset = memory->Read(address(dst));  // Get address for branch
                        tmp = memory->Read(007);              // Get current address in PC
                        tmp = tmp + (offset << 2);            // Get new address for branch
                        (memory->ReadPS() & Zbit) == 0? \
                        memory->Write(007,tmp) : NOP();       // Z = 0
                        return instruction;
					}
					else if(iB[2] > 3)
					{
						//BEQ
						offset = memory->Read(address(dst));  // Get address for branch
                        tmp = memory->Read(007);              // Get current address in PC
                        tmp = tmp + (offset << 2);            // Get new address for branch
                        (memory->ReadPS() & Zbit) > 0? \
                        memory->Write(007,tmp) : NOP();          // Z = 0
                        return instruction;
					}	
				}
			}
			case 2:	//BVC, BGE, BVS, BLT
			{
				if(iB[5]) //BVC or BVS
				{
					if(iB[2] <= 3)
					{
						//BVC
						offset = memory->Read(address(dst));  // Get address for branch
                        tmp = memory->Read(007);              // Get current address in PC
                        tmp = tmp + (offset << 2);            // Get new address for branch
                        (memory->ReadPS() & Vbit) == 0? \
                        memory->Write(007,tmp) : NOP();          // V = 0
                        return instruction;
					}
					else if(iB[2] > 3)
					{
						//BVS
						offset = memory->Read(address(dst));  // Get address for branch
                        tmp = memory->Read(007);              // Get current address in PC
                        tmp = tmp + (offset << 2);            // Get new address for branch
                        (memory->Read(PS) & Vbit) > 0? \
                        memory->Write(007,tmp) : NOP();          // V = 1
                        return instruction;
					}
				}
				else if (!iB[5]) //BGE or BLT
				{
					if(iB[2] <= 3)
					{
						//BGE
						offset = memory->Read(address(dst));  // Get address for branch
                        tmp = memory->Read(007);              // Get current address in PC
                        tmp = tmp + (offset << 2);            // Get new address for branch
                        (((memory->ReadPS() & Nbit) >> 3) ^ ((memory->ReadPS() & Vbit) >> 1)) == 0? \
                        memory->Write(007,tmp) : NOP();          // N ^ V = 0
                        return instruction;
					}
					else if(iB[2] > 3)
					{
						//BLT
						offset = memory->Read(address(dst));  // Get address for branch
                        tmp = memory->Read(007);              // Get current address in PC
                        tmp = tmp + (offset << 2);            // Get new address for branch
                        (((memory->ReadPS() & Nbit) >> 3) ^ ((memory->ReadPS() & Vbit) >> 1)) == 1? \
                        memory->Write(007,tmp) : NOP();          // N ^ V == 1
                        return instruction;
					}	
				}
			}
			case 3:	//BGT, BCC, BLE, or BCS
			{
				if(iB[5]) //BCC or BCS
				{
					if(iB[2] <= 3)
					{
						//BCC
						offset = memory->Read(address(dst));  // Get address for branch
                        tmp = memory->Read(007);              // Get current address in PC
                        tmp = tmp + (offset << 2);            // Get new address for branch
                        (memory->ReadPS() & Cbit) == 0? \
                        memory->Write(007,tmp) : NOP();          // C = 0
                        return instruction;
					}
					else if(iB[2] > 3)
					{
						//BCS
						offset = memory->Read(address(dst));             // Get address for branch
                        tmp = memory->Read(007);                        // Get current address in PC
                        offset = tmp + (offset << 2);                   // Calc new address for branch
                        (memory->ReadPS() & Cbit) > 0? \
                        memory->Write(007,offset) : NOP();                 // C = 1
                        return instruction;
					}
				}
				else if (!iB[5]) //BLE or BGT
				{
					if(iB[2] <= 3)
					{
						//BGT
						offset = memory->Read(address(dst));  // Get address for branch
                        tmp = memory->Read(007);              // Get current address in PC
                        tmp = tmp + (offset << 2);            // Get new address for branch
                        (((memory->ReadPS() & Zbit) >> 2) | \
                        (((memory->ReadPS() & Nbit) >> 3) ^ ((memory->ReadPS() & Vbit) >> 1))) == 0? \
                        memory->Write(007,tmp) : NOP();          // Z | (N ^ V) = 0
                        return instruction;
					}
					else if(iB[2] > 3)
					{
						//BLE
						offset = memory->Read(address(dst));  // Get address for branch
                        tmp = memory->Read(007);              // Get current address in PC
                        offset = tmp + (offset << 2);         // Get new address for branch
                        tmp = memory->ReadPS();               // Get current process status
                        (((tmp & Zbit) >> 2) & (((tmp & Nbit) >> 3) ^ ((tmp & Vbit) >> 1))) == 1? \
                        memory->Write(007,offset) : NOP();        // Z(N^V) = 1
                        return instruction;
					}	
				}	
			}
			default:
				break;
		}
	}
}
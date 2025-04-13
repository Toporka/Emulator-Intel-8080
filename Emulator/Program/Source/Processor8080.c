#include "Processor8080.h"

// Проверка на чётность
int parity(int value)
{
	int bits_on = 0;
	value = (value & ((1<<8)-1));
	for(int i = 0; i < 8; i++) {
		if(value & 0x1) 
			bits_on++;
		value = value >> 1;	
	}
	return (0 == (bits_on & 0x1));
}

void WriteMem(Processor8080* processor, uint16_t address, uint8_t value)
{
    processor->Memory[address] = value;
}

void WriteToHL(Processor8080* processor, uint8_t value)
{
    uint16_t offset = (processor->H << 8) | processor->L;
    processor->Memory[offset] = value;
}

void Push(Processor8080* processor, uint8_t high, uint8_t low)
{
    WriteMem(processor, processor->StackPointer - 1, high);
    WriteMem(processor, processor->StackPointer - 2, low);
    processor->StackPointer = processor->StackPointer - 2;
}

void Pop(Processor8080* processor, uint8_t *high, uint8_t *low)
{
    *low = processor->Memory[processor->StackPointer];
    *high = processor->Memory[processor->StackPointer+1];
    processor->StackPointer += 2;
}

void LogicFlagsA(Processor8080 *processor)
{
	processor->Flag.CY = processor->Flag.AC = 0;
	processor->Flag.Z = (processor->A == 0);
	processor->Flag.S = (0x80 == (processor->A & 0x80));
	processor->Flag.P = parity(processor->A & 0xff);
}

void ArithFlagsA(Processor8080 *processor, uint16_t res)
{
	processor->Flag.CY = (res > 0xff);
	processor->Flag.Z = (res == 0);
	processor->Flag.S = (0x80 == (res & 0x80));
	processor->Flag.P = parity(res & 0xff);
}

void FlagsZSP(Processor8080 *processor, uint8_t value)
{
    processor->Flag.Z = (value == 0);
    processor->Flag.S = (0x80 == (value & 0x80));
    processor->Flag.P = parity(value);    
}

void STAX_B(Processor8080 *processor, unsigned char *opcode)
{
    uint16_t offset = (processor->B << 8) | processor->C;
    processor->Memory[offset] = processor->A;

    processor->ProgrammCounter++;
}
void STAX_D(Processor8080 *processor, unsigned char *opcode)
{
    uint16_t offset = (processor->D << 8) | processor->E;
    WriteMem(processor, offset, processor->A);

    processor->ProgrammCounter++;
}

void NOP(Processor8080* processor, unsigned char *opcode)
{
    processor->ProgrammCounter++;
}

void EI(Processor8080* processor, unsigned char *opcode)
{
    processor->int_enable = 1;

    processor->ProgrammCounter++;
}

void XCHG(Processor8080* processor, unsigned char *opcode)
{
    uint8_t save1 = processor->D;
    uint8_t save2 = processor->E;
    processor->D = processor->H;
    processor->E = processor->L;
    processor->H = save1;
    processor->L = save2;

    processor->ProgrammCounter++;
}

void SUI(Processor8080* processor, unsigned char *opcode)
{
    uint8_t a = processor->A - opcode[1];
    FlagsZSP(processor, a&0xff);
    processor->Flag.CY = (processor->A < opcode[1]);
    processor->A = a;
    
    processor->ProgrammCounter += 2;
}
void ORI(Processor8080* processor, unsigned char *opcode)
{
    uint8_t a = processor->A | opcode[1];
    FlagsZSP(processor, a);
    processor->Flag.CY = 0;
    processor->A = a;

    processor->ProgrammCounter += 2;
}
void ANI(Processor8080* processor, unsigned char *opcode)
{
    processor->A = processor->A & opcode[1];
	LogicFlagsA(processor);

	processor->ProgrammCounter += 2;
}
void ADI(Processor8080* processor, unsigned char *opcode)
{
    uint16_t result = (uint16_t) processor->A + (uint16_t) opcode[1];

    FlagsZSP(processor, result&0xff);
    processor->Flag.CY = (result > 0xff);
    processor->A = result & 0xFF;
    
    processor->ProgrammCounter += 2; 
}

void OUT(Processor8080* processor, unsigned char *opcode)
{
    processor->ProgrammCounter += 2;
}

void CALL(Processor8080* processor, unsigned char *opcode)
{
     
    uint16_t	ret = processor->ProgrammCounter + 3;
    WriteMem(processor, processor->StackPointer-1, ret >> 8);
    WriteMem(processor, processor->StackPointer-2, (ret & 0xff));
    processor->StackPointer = processor->StackPointer - 2;
    processor->ProgrammCounter= (opcode[2] << 8) | opcode[1]; 
    
}

void JMP(Processor8080* processor, unsigned char *opcode)
{
    processor->ProgrammCounter = (opcode[2] << 8) | opcode[1];
}

void RET(Processor8080* processor, unsigned char *opcode)
{
    processor->ProgrammCounter = (processor->Memory[processor->StackPointer] | (processor->Memory[processor->StackPointer + 1] << 8));    
    processor->StackPointer += 2; 
}

void PUSH_PSW(Processor8080* processor, unsigned char *opcode)
{
    processor->Memory[processor->StackPointer-1] = processor->A;
    uint8_t flags = processor->Flag.Z << 0 
                    | processor->Flag.S << 1
                    | processor->Flag.P << 2
                    | processor->Flag.CY << 3
                    | processor->Flag.AC << 4;
    processor->Memory[processor->StackPointer-2] = flags;
    processor->StackPointer -= 2;

    processor->ProgrammCounter++;
}
void PUSH_H(Processor8080* processor, unsigned char *opcode)
{
    Push(processor, processor->H, processor->L);

    processor->ProgrammCounter++;
}
void PUSH_D(Processor8080* processor, unsigned char *opcode)
{
    Push(processor, processor->D, processor->E);

    processor->ProgrammCounter++;
}
void PUSH_B(Processor8080* processor, unsigned char *opcode)
{
    Push(processor, processor->B, processor->C);

    processor->ProgrammCounter++;
}

void JNZ(Processor8080* processor, unsigned char *opcode)
{
    if (0 == processor->Flag.Z)    
        processor->ProgrammCounter = ((opcode[2] << 8) | opcode[1]);    
    else    
        processor->ProgrammCounter += 3;
}
void JNC(Processor8080* processor, unsigned char *opcode)
{
    if (0 == processor->Flag.CY)    
        processor->ProgrammCounter = ((opcode[2] << 8) | opcode[1]);    
    else    
        processor->ProgrammCounter += 3;
}
void JPO(Processor8080* processor, unsigned char *opcode)
{
    if (0 == processor->Flag.P)    
        processor->ProgrammCounter = ((opcode[2] << 8) | opcode[1]);    
    else    
        processor->ProgrammCounter += 3;
}
void JP(Processor8080* processor, unsigned char *opcode)
{
    if (0 == processor->Flag.S)    
        processor->ProgrammCounter = ((opcode[2] << 8) | opcode[1]);    
    else    
        processor->ProgrammCounter += 3;
}

void POP_PSW(Processor8080* processor, unsigned char *opcode)
{
    processor->A = processor->Memory[processor->StackPointer+1];
    processor->Flag.Z = (0x01 == (processor->Memory[processor->StackPointer] & 0x01));
    processor->Flag.S = (0x02 == (processor->Memory[processor->StackPointer] & 0x02));
    processor->Flag.P = (0x04 == (processor->Memory[processor->StackPointer] & 0x04));
    processor->Flag.CY = (0x08 == (processor->Memory[processor->StackPointer] & 0x08));
    processor->Flag.AC = (0x10 == (processor->Memory[processor->StackPointer] & 0x10));
    processor->StackPointer += 2;

    processor->ProgrammCounter++;
}
void POP_H(Processor8080* processor, unsigned char *opcode)
{
    Pop(processor, &processor->H, &processor->L);

    processor->ProgrammCounter++;
}
void POP_D(Processor8080* processor, unsigned char *opcode)
{
    Pop(processor, &processor->D, &processor->E);

    processor->ProgrammCounter++;
}
void POP_B(Processor8080* processor, unsigned char *opcode)
{
    Pop(processor, &processor->B, &processor->C);

    processor->ProgrammCounter++;
}

void XRA(Processor8080* processor, unsigned char *opcode, uint8_t reg)
{
    processor->A = processor->A ^ reg;
    LogicFlagsA(processor);

    processor->ProgrammCounter++;
}
void XRA_A(Processor8080* processor, unsigned char *opcode)
{
    XRA(processor, opcode, processor->A);
}
void XRA_B(Processor8080* processor, unsigned char *opcode)
{
    XRA(processor, opcode, processor->B);
}
void XRA_C(Processor8080* processor, unsigned char *opcode)
{
    XRA(processor, opcode, processor->C);
}
void XRA_D(Processor8080* processor, unsigned char *opcode)
{
    XRA(processor, opcode, processor->D);
}
void XRA_E(Processor8080* processor, unsigned char *opcode)
{
    XRA(processor, opcode, processor->E);
}
void XRA_H(Processor8080* processor, unsigned char *opcode)
{
    XRA(processor, opcode, processor->H);
}
void XRA_L(Processor8080* processor, unsigned char *opcode)
{
    XRA(processor, opcode, processor->L);
}
void XRA_M(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | processor->L;
    XRA(processor, opcode, processor->Memory[offset]);
}

void ANA(Processor8080* processor, unsigned char *opcode, uint8_t reg)
{
    processor->A = processor->A & reg;

    processor->Flag.CY = 0;
	processor->Flag.Z = ((processor->A & 0xff) == 0);
	processor->Flag.S = (0x80 == (processor->A & 0x80));
	processor->Flag.P = parity(processor->A & 0xff);

    processor->ProgrammCounter++;
}
void ANA_A(Processor8080* processor, unsigned char *opcode)
{
    ANA(processor, opcode, processor->A);
}
void ANA_B(Processor8080* processor, unsigned char *opcode)
{
    ANA(processor, opcode, processor->B);
}
void ANA_C(Processor8080* processor, unsigned char *opcode)
{
    ANA(processor, opcode, processor->C);
}
void ANA_D(Processor8080* processor, unsigned char *opcode)
{
    ANA(processor, opcode, processor->D);
}
void ANA_E(Processor8080* processor, unsigned char *opcode)
{
    ANA(processor, opcode, processor->E);
}
void ANA_H(Processor8080* processor, unsigned char *opcode)
{
    ANA(processor, opcode, processor->H);
}
void ANA_L(Processor8080* processor, unsigned char *opcode)
{
    ANA(processor, opcode, processor->L);
}
void ANA_M(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | processor->L;
    ANA(processor, opcode, processor->Memory[offset]);
}

void LDA(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (opcode[2]<<8) | (opcode[1]);
    processor->A = processor->Memory[offset];

    processor->ProgrammCounter += 3;
}

void STA(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (opcode[2]<<8) | (opcode[1]);
    WriteMem(processor, offset, processor->A);

    processor->ProgrammCounter += 3;
}

void LDAX_B(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->B << 8) | processor->C;
    processor->A = processor->Memory[offset];

    processor->ProgrammCounter++;
}
void LDAX_D(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->D<<8) | processor->E;
    processor->A = processor->Memory[offset];

    processor->ProgrammCounter++;
}

void INX_B(Processor8080* processor, unsigned char *opcode)
{
    processor->C++;
    if (processor->C == 0)
    {
        processor->B++;
    }
    
    processor->ProgrammCounter++;
}
void INX_H(Processor8080* processor, unsigned char *opcode)
{
    processor->L++;
    if (processor->L == 0)
    {
        processor->H++;
    }
    
    processor->ProgrammCounter++;
}
void INX_D(Processor8080* processor, unsigned char *opcode)
{
    processor->E++;
    if (processor->E == 0)
    {
        processor->D++;
    }
    
    processor->ProgrammCounter++;
}
void INX_SP(Processor8080* processor, unsigned char *opcode)
{
    processor->StackPointer++;
    processor->ProgrammCounter++;
}

void RRC(Processor8080* processor, unsigned char *opcode)
{
    uint8_t a = processor->A;
    
    processor->A = ((a & 1) << 7) | (a >> 1);
    processor->Flag.CY = (1 == (a&1)); 

    processor->ProgrammCounter++;
}
void RLC(Processor8080* processor, unsigned char *opcode)
{
    uint8_t a = processor->A;
    
    processor->A = ((a & 0x80) >> 7) | (a << 1);
    processor->Flag.CY = (0x80 == (a&0x80));

    processor->ProgrammCounter++;
}
void RAL(Processor8080* processor, unsigned char *opcode)
{
    uint8_t a = processor->A;

    processor->A = (a << 1) | (processor->Flag.CY & 0x1);
    processor->Flag.CY = (0x80 == (a&0x80));

    processor->ProgrammCounter++;
}
void RAR(Processor8080* processor, unsigned char *opcode)
{
    uint8_t a = processor->A;

    processor->A = (processor->Flag.CY << 7) | (a >> 1);
    processor->Flag.CY = (1 == (a & 1));;

    processor->ProgrammCounter++;
}
void DAA(Processor8080* processor, unsigned char *opcode)
{
    if ((processor->A &0xf) > 9)
    {
        processor->A += 6;
    }
    
    if ((processor->A&0xf0) > 0x90)
    {
        uint16_t res = (uint16_t) processor->A + 0x60;
        ArithFlagsA(processor, res);
        processor->A = res & 0xff;
    }

    processor->ProgrammCounter++;
}
void CMA(Processor8080* processor, unsigned char *opcode)
{
    processor->A = ~(processor->A);

    processor->ProgrammCounter++;
}
void STC(Processor8080* processor, unsigned char *opcode)
{
    processor->Flag.CY = 1;

    processor->ProgrammCounter++;
}
void CMC(Processor8080* processor, unsigned char *opcode)
{
    processor->Flag.CY = 0;

    processor->ProgrammCounter++;
}

void DAD_H(Processor8080* processor, unsigned char *opcode)
{
    uint32_t HL = (processor->H << 8) | processor->L;
    uint32_t res = HL + HL;

    processor->H = (res & 0xff00) >> 8;
    processor->L = res & 0xFF;
    processor->Flag.CY = (res > 0xFFFF);

    processor->ProgrammCounter++;
}
void DAD_D(Processor8080* processor, unsigned char *opcode)
{
    uint32_t hl = (processor->H << 8) | processor->L;
    uint32_t de = (processor->D << 8) | processor->E;
    uint32_t res = hl + de;

    processor->H = (res & 0xff00) >> 8;
    processor->L = res & 0xFF;
    if (res > 0xFFFF) 
    {
        processor->Flag.CY = 1;
    } 
    else 
    {
        processor->Flag.CY = 0;
    }

    processor->ProgrammCounter++;
}
void DAD_B(Processor8080* processor, unsigned char *opcode)
{
    uint32_t hl = (processor->H << 8) | processor->L;
    uint32_t bc = (processor->B << 8) | processor->C;
    uint32_t res = hl + bc;

    processor->H = (res & 0xff00) >> 8;
    processor->L = res & 0xFF;
    if (res > 0xFFFF) 
    {
        processor->Flag.CY = 1;
    } 
    else 
    {
        processor->Flag.CY = 0;
    }

    processor->ProgrammCounter++;
}
void DAD_SP(Processor8080 *processor, unsigned char *opcode) 
{
    uint16_t hl = (processor->H << 8) | processor->L;
    uint32_t res = hl + processor->StackPointer;

    processor->H = (res & 0xff00) >> 8;
    processor->L = res & 0xFF;
    processor->Flag.CY = (res > 0xFFFF);

    processor->ProgrammCounter++;
}

void MVI_A(Processor8080* processor, unsigned char *opcode)
{
    processor->A = opcode[1];
    processor->ProgrammCounter += 2;
}
void MVI_M(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | processor->L;
    processor->Memory[offset] = opcode[1];
    
    processor->ProgrammCounter += 2;

}
void MVI_C(Processor8080* processor, unsigned char *opcode)
{
    processor->C = opcode[1];
    processor->ProgrammCounter += 2;
}
void MVI_H(Processor8080* processor, unsigned char *opcode)
{
    processor->H = opcode[1];
	processor->ProgrammCounter += 2;
}
void MVI_B(Processor8080* processor, unsigned char *opcode)
{
    processor->B = opcode[1];
    processor->ProgrammCounter += 2;
}
void MVI_E(Processor8080* processor, unsigned char *opcode)
{
    processor->E = opcode[1];
    processor->ProgrammCounter += 2;
}
void MVI_L(Processor8080* processor, unsigned char *opcode)
{
    processor->L = opcode[1];
    processor->ProgrammCounter += 2;
}
void MVI_D(Processor8080* processor, unsigned char *opcode)
{
    processor->D = opcode[1];
    processor->ProgrammCounter += 2;
}

void LXI_SP(Processor8080* processor, unsigned char *opcode)
{
    processor->StackPointer = (opcode[2] << 8) | opcode[1];
    processor->ProgrammCounter += 3;
}
void LXI_H(Processor8080* processor, unsigned char *opcode)
{
    processor->L = opcode[1];
    processor->H = opcode[2];
    processor->ProgrammCounter += 3;
}
void LXI_D(Processor8080* processor, unsigned char *opcode)
{
    processor->E = opcode[1];
    processor->D = opcode[2];
    processor->ProgrammCounter += 3;
}
void LXI_B(Processor8080* processor, unsigned char *opcode)
{
    processor->C = opcode[1];
    processor->B = opcode[2];
    processor->ProgrammCounter += 3;
}

void MOV_A_A(Processor8080* processor, unsigned char *opcode)
{
    processor->A = processor->A;

    processor->ProgrammCounter++;
}
void MOV_A_B(Processor8080* processor, unsigned char *opcode)
{
    processor->A = processor->B;

    processor->ProgrammCounter++;
}
void MOV_A_C(Processor8080* processor, unsigned char *opcode)
{
    processor->A = processor->C;

    processor->ProgrammCounter++;
}
void MOV_A_D(Processor8080* processor, unsigned char *opcode)
{
    processor->A = processor->D;

    processor->ProgrammCounter++;
}
void MOV_A_E(Processor8080* processor, unsigned char *opcode)
{
    processor->A = processor->E;

    processor->ProgrammCounter++;
}
void MOV_A_H(Processor8080* processor, unsigned char *opcode)
{
    processor->A = processor->H;

    processor->ProgrammCounter++;
}
void MOV_A_L(Processor8080* processor, unsigned char *opcode)
{
    processor->A = processor->L;

    processor->ProgrammCounter++;
}
void MOV_A_M(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | (processor->L);
	processor->A = processor->Memory[offset];

    processor->ProgrammCounter++;
}

void MOV_B_B(Processor8080* processor, unsigned char *opcode)
{
    processor->B = processor->B;

    processor->ProgrammCounter++;
}
void MOV_B_A(Processor8080* processor, unsigned char *opcode)
{
    processor->B = processor->A;

    processor->ProgrammCounter++;
}
void MOV_B_C(Processor8080* processor, unsigned char *opcode)
{
    processor->B = processor->C;

    processor->ProgrammCounter++;
}
void MOV_B_D(Processor8080* processor, unsigned char *opcode)
{
    processor->B = processor->D;

    processor->ProgrammCounter++;
}
void MOV_B_E(Processor8080* processor, unsigned char *opcode)
{
    processor->B = processor->E;

    processor->ProgrammCounter++;
}
void MOV_B_H(Processor8080* processor, unsigned char *opcode)
{
    processor->B = processor->H;

    processor->ProgrammCounter++;
}
void MOV_B_L(Processor8080* processor, unsigned char *opcode)
{
    processor->B = processor->L;

    processor->ProgrammCounter++;
}
void MOV_B_M(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | (processor->L);
	processor->B = processor->Memory[offset];

    processor->ProgrammCounter++;
}

void MOV_C_C(Processor8080* processor, unsigned char *opcode)
{
    processor->C = processor->C;

    processor->ProgrammCounter++;
}
void MOV_C_A(Processor8080* processor, unsigned char *opcode)
{
    processor->C = processor->A;

    processor->ProgrammCounter++;
}
void MOV_C_B(Processor8080* processor, unsigned char *opcode)
{
    processor->C = processor->B;

    processor->ProgrammCounter++;
}
void MOV_C_D(Processor8080* processor, unsigned char *opcode)
{
    processor->C = processor->D;

    processor->ProgrammCounter++;
}
void MOV_C_E(Processor8080* processor, unsigned char *opcode)
{
    processor->C = processor->E;

    processor->ProgrammCounter++;
}
void MOV_C_H(Processor8080* processor, unsigned char *opcode)
{
    processor->C = processor->H;

    processor->ProgrammCounter++;
}
void MOV_C_L(Processor8080* processor, unsigned char *opcode)
{
    processor->C = processor->L;

    processor->ProgrammCounter++;
}
void MOV_C_M(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | (processor->L);
	processor->C = processor->Memory[offset];

    processor->ProgrammCounter++;
}

void MOV_D_D(Processor8080* processor, unsigned char *opcode)
{
    processor->D = processor->D;

    processor->ProgrammCounter++;
}
void MOV_D_A(Processor8080* processor, unsigned char *opcode)
{
    processor->D = processor->A;

    processor->ProgrammCounter++;
}
void MOV_D_B(Processor8080* processor, unsigned char *opcode)
{
    processor->D = processor->B;

    processor->ProgrammCounter++;
}
void MOV_D_C(Processor8080* processor, unsigned char *opcode)
{
    processor->D = processor->C;

    processor->ProgrammCounter++;
}
void MOV_D_E(Processor8080* processor, unsigned char *opcode)
{
    processor->D = processor->E;

    processor->ProgrammCounter++;
}
void MOV_D_H(Processor8080* processor, unsigned char *opcode)
{
    processor->D = processor->H;

    processor->ProgrammCounter++;
}
void MOV_D_L(Processor8080* processor, unsigned char *opcode)
{
    processor->D = processor->L;

    processor->ProgrammCounter++;
}
void MOV_D_M(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | (processor->L);
	processor->D = processor->Memory[offset];

    processor->ProgrammCounter++;
}

void MOV_E_E(Processor8080* processor, unsigned char *opcode)
{
    processor->E = processor->E;

    processor->ProgrammCounter++;
}
void MOV_E_A(Processor8080* processor, unsigned char *opcode)
{
    processor->E = processor->A;

    processor->ProgrammCounter++;
}
void MOV_E_B(Processor8080* processor, unsigned char *opcode)
{
    processor->E = processor->B;

    processor->ProgrammCounter++;
}
void MOV_E_C(Processor8080* processor, unsigned char *opcode)
{
    processor->E = processor->C;

    processor->ProgrammCounter++;
}
void MOV_E_D(Processor8080* processor, unsigned char *opcode)
{
    processor->E = processor->D;

    processor->ProgrammCounter++;
}
void MOV_E_H(Processor8080* processor, unsigned char *opcode)
{
    processor->E = processor->H;

    processor->ProgrammCounter++;
}
void MOV_E_L(Processor8080* processor, unsigned char *opcode)
{
    processor->E = processor->L;

    processor->ProgrammCounter++;
}
void MOV_E_M(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | (processor->L);
	processor->E = processor->Memory[offset];

    processor->ProgrammCounter++;
}

void MOV_H_H(Processor8080* processor, unsigned char *opcode)
{
    processor->H = processor->H;

    processor->ProgrammCounter++;
}
void MOV_H_A(Processor8080* processor, unsigned char *opcode)
{
	processor->H = processor->A;

    processor->ProgrammCounter++;
}
void MOV_H_B(Processor8080* processor, unsigned char *opcode)
{
	processor->H = processor->B;

    processor->ProgrammCounter++;
}
void MOV_H_C(Processor8080* processor, unsigned char *opcode)
{
	processor->H = processor->C;

    processor->ProgrammCounter++;
}
void MOV_H_D(Processor8080* processor, unsigned char *opcode)
{
	processor->H = processor->D;

    processor->ProgrammCounter++;
}
void MOV_H_E(Processor8080* processor, unsigned char *opcode)
{
	processor->H = processor->E;

    processor->ProgrammCounter++;
}
void MOV_H_L(Processor8080* processor, unsigned char *opcode)
{
	processor->H = processor->L;

    processor->ProgrammCounter++;
}
void MOV_H_M(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | (processor->L);
	processor->H = processor->Memory[offset];

    processor->ProgrammCounter++;
}

void MOV_L_L(Processor8080* processor, unsigned char *opcode)
{
    processor->L = processor->L;

    processor->ProgrammCounter++;
}
void MOV_L_A(Processor8080* processor, unsigned char *opcode)
{
	processor->L = processor->A;

    processor->ProgrammCounter++;
}
void MOV_L_B(Processor8080* processor, unsigned char *opcode)
{
	processor->L = processor->B;

    processor->ProgrammCounter++;
}
void MOV_L_C(Processor8080* processor, unsigned char *opcode)
{
	processor->L = processor->C;

    processor->ProgrammCounter++;
}
void MOV_L_D(Processor8080* processor, unsigned char *opcode)
{
	processor->L = processor->D;
    
    processor->ProgrammCounter++;
}
void MOV_L_E(Processor8080* processor, unsigned char *opcode)
{
	processor->L = processor->E;

    processor->ProgrammCounter++;
}
void MOV_L_H(Processor8080* processor, unsigned char *opcode)
{
	processor->L = processor->H;

    processor->ProgrammCounter++;
}
void MOV_L_M(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | (processor->L);
	processor->L = processor->Memory[offset];

    processor->ProgrammCounter++;
}

void HLT(Processor8080* processor, unsigned char *opcode)
{
    processor->halted = 1;

    processor->ProgrammCounter++;
}
void MOV_M_A(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | (processor->L);
    processor->Memory[offset] = processor->A;

    processor->ProgrammCounter++;

}
void MOV_M_B(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | (processor->L);
    processor->Memory[offset] = processor->B;

    processor->ProgrammCounter++;

}
void MOV_M_C(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | (processor->L);
    processor->Memory[offset] = processor->C;

    processor->ProgrammCounter++;
}
void MOV_M_D(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | (processor->L);
    processor->Memory[offset] = processor->D;

    processor->ProgrammCounter++;
}
void MOV_M_E(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | (processor->L);
    processor->Memory[offset] = processor->E;

    processor->ProgrammCounter++;
}
void MOV_M_H(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | (processor->L);
    processor->Memory[offset] = processor->H;

    processor->ProgrammCounter++;
}
void MOV_M_L(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | (processor->L);
    processor->Memory[offset] = processor->L;

    processor->ProgrammCounter++;
}

void ADD(Processor8080* processor, unsigned char *opcode, uint8_t reg)
{
    uint16_t result = (uint16_t) processor->A + (uint16_t) reg;    
    
    processor->Flag.CY = (result > 0xff);
	processor->Flag.Z = ((result & 0xFF) == 0);
	processor->Flag.S = (0x80 == (result & 0x80));
	processor->Flag.P = parity(result & 0xff);

    processor->A = (result & 0xff);

    processor->ProgrammCounter++;
}
void ADD_C(Processor8080* processor, unsigned char *opcode)
{
    ADD(processor, opcode, processor->C);
}
void ADD_B(Processor8080* processor, unsigned char *opcode)
{
    ADD(processor, opcode, processor->B);
}
void ADD_D(Processor8080* processor, unsigned char *opcode)
{
    ADD(processor, opcode, processor->D);
}
void ADD_E(Processor8080* processor, unsigned char *opcode)
{
    ADD(processor, opcode, processor->E);
}
void ADD_H(Processor8080* processor, unsigned char *opcode)
{
    ADD(processor, opcode, processor->H);
}
void ADD_L(Processor8080* processor, unsigned char *opcode)
{
    ADD(processor, opcode, processor->L);
}
void ADD_M(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | processor->L;
    ADD(processor, opcode, processor->Memory[offset]);
}
void ADD_A(Processor8080* processor, unsigned char *opcode)
{
    ADD(processor, opcode, processor->A);
}

void ADC(Processor8080* processor, unsigned char *opcode, uint8_t reg)
{
    uint16_t result = (uint16_t) processor->A + (uint16_t) reg + processor->Flag.CY;
    ArithFlagsA(processor, result);
    processor->A = result & 0xFF;

    processor->ProgrammCounter++;
}
void ADC_B(Processor8080* processor, unsigned char *opcode)
{
    ADC(processor, opcode, processor->B);
}
void ADC_C(Processor8080* processor, unsigned char *opcode)
{
    ADC(processor, opcode, processor->C);
}
void ADC_D(Processor8080* processor, unsigned char *opcode)
{
    ADC(processor, opcode, processor->D);
}
void ADC_E(Processor8080* processor, unsigned char *opcode)
{
    ADC(processor, opcode, processor->E);
}
void ADC_H(Processor8080* processor, unsigned char *opcode)
{
    ADC(processor, opcode, processor->H);
}
void ADC_L(Processor8080* processor, unsigned char *opcode)
{
    ADC(processor, opcode, processor->L);
}
void ADC_M(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | processor->L;
    ADC(processor, opcode, processor->Memory[offset]);
}
void ADC_A(Processor8080* processor, unsigned char *opcode)
{
    ADC(processor, opcode, processor->A);
}

void SUB(Processor8080* processor, unsigned char *opcode, uint8_t reg)
{
    uint16_t result = (uint16_t) processor->A - (uint16_t) reg;
    ArithFlagsA(processor, result);
    processor->A = result & 0xFF;

    processor->ProgrammCounter++;
}
void SUB_B(Processor8080* processor, unsigned char *opcode)
{
    SUB(processor, opcode, processor->B);
}
void SUB_C(Processor8080* processor, unsigned char *opcode)
{
    SUB(processor, opcode, processor->C);
}
void SUB_D(Processor8080* processor, unsigned char *opcode)
{
    SUB(processor, opcode, processor->D);
}
void SUB_E(Processor8080* processor, unsigned char *opcode)
{
    SUB(processor, opcode, processor->E);
}
void SUB_H(Processor8080* processor, unsigned char *opcode)
{
    SUB(processor, opcode, processor->H);
}
void SUB_L(Processor8080* processor, unsigned char *opcode)
{
    SUB(processor, opcode, processor->L);
}
void SUB_M(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | processor->L;
    SUB(processor, opcode, processor->Memory[offset]);
}
void SUB_A(Processor8080* processor, unsigned char *opcode)
{
    SUB(processor, opcode, processor->A);
}

void SBB(Processor8080* processor, unsigned char *opcode, uint8_t reg)
{
    uint16_t result = (uint16_t) processor->A - (uint16_t) reg - processor->Flag.CY;
    ArithFlagsA(processor, result);
    processor->A = result & 0xFF;

    processor->ProgrammCounter++;
}
void SBB_B(Processor8080* processor, unsigned char *opcode)
{
    SBB(processor, opcode, processor->B);
}
void SBB_C(Processor8080* processor, unsigned char *opcode)
{
    SBB(processor, opcode, processor->C);
}
void SBB_D(Processor8080* processor, unsigned char *opcode)
{
    SBB(processor, opcode, processor->D);
}
void SBB_E(Processor8080* processor, unsigned char *opcode)
{
    SBB(processor, opcode, processor->E);
}
void SBB_H(Processor8080* processor, unsigned char *opcode)
{
    SBB(processor, opcode, processor->H);
}
void SBB_L(Processor8080* processor, unsigned char *opcode)
{
    SBB(processor, opcode, processor->L);
}
void SBB_M(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | processor->L;
    SBB(processor, opcode, processor->Memory[offset]);
}
void SBB_A(Processor8080* processor, unsigned char *opcode)
{
    SBB(processor, opcode, processor->A);
}

void ORA(Processor8080* processor, unsigned char *opcode, uint8_t reg)
{
    processor->A = processor->A | reg;
    LogicFlagsA(processor);

    processor->ProgrammCounter++;
}
void ORA_B(Processor8080* processor, unsigned char *opcode)
{
    ORA(processor, opcode, processor->B);
}
void ORA_C(Processor8080* processor, unsigned char *opcode)
{
    ORA(processor, opcode, processor->C);
}
void ORA_D(Processor8080* processor, unsigned char *opcode)
{
    ORA(processor, opcode, processor->D);
}
void ORA_E(Processor8080* processor, unsigned char *opcode)
{
    ORA(processor, opcode, processor->E);
}
void ORA_H(Processor8080* processor, unsigned char *opcode)
{
    ORA(processor, opcode, processor->H);
}
void ORA_L(Processor8080* processor, unsigned char *opcode)
{
    ORA(processor, opcode, processor->L);
}
void ORA_M(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | processor->L;
    ORA(processor, opcode, processor->Memory[offset]);
}
void ORA_A(Processor8080* processor, unsigned char *opcode)
{
    ORA(processor, opcode, processor->A);
}

void CMP(Processor8080* processor, unsigned char *opcode, uint8_t reg)
{
    uint16_t result = (uint16_t) processor->A - (uint16_t) reg;
    ArithFlagsA(processor, result);

    processor->ProgrammCounter++;
}
void CMP_B(Processor8080* processor, unsigned char *opcode)
{
    CMP(processor, opcode, processor->B);
}
void CMP_C(Processor8080* processor, unsigned char *opcode)
{
    CMP(processor, opcode, processor->C);
}
void CMP_D(Processor8080* processor, unsigned char *opcode)
{
    CMP(processor, opcode, processor->D);
}
void CMP_E(Processor8080* processor, unsigned char *opcode)
{
    CMP(processor, opcode, processor->E);
}
void CMP_H(Processor8080* processor, unsigned char *opcode)
{
    CMP(processor, opcode, processor->H);
}
void CMP_L(Processor8080* processor, unsigned char *opcode)
{
    CMP(processor, opcode, processor->L);
}
void CMP_M(Processor8080* processor, unsigned char *opcode)
{
    uint16_t offset = (processor->H<<8) | processor->L;
    CMP(processor, opcode, processor->Memory[offset]);
}
void CMP_A(Processor8080* processor, unsigned char *opcode)
{
    CMP(processor, opcode, processor->A);
}

void INR(Processor8080* processor, uint8_t *reg)
{
    uint8_t res = *reg + 1;
    FlagsZSP(processor, res);
    *reg = res;

    processor->ProgrammCounter++;
}
void INR_B(Processor8080* processor, unsigned char *opcode)
{
    INR(processor, &processor->B);
}
void INR_C(Processor8080* processor, unsigned char *opcode)
{
    INR(processor, &processor->C);
}
void INR_D(Processor8080* processor, unsigned char *opcode)
{
    INR(processor, &processor->D);
}
void INR_E(Processor8080* processor, unsigned char *opcode)
{
    INR(processor, &processor->E);
}
void INR_H(Processor8080* processor, unsigned char *opcode)
{
    INR(processor, &processor->H);
}
void INR_L(Processor8080* processor, unsigned char *opcode)
{
    INR(processor, &processor->L);
}
void INR_M(Processor8080* processor, unsigned char *opcode)
{
    uint16_t address = ((processor->H << 8) | processor->L);
    INR(processor, &processor->Memory[address]);
}
void INR_A(Processor8080* processor, unsigned char *opcode)
{
    INR(processor, &processor->A);
}

void DCR(Processor8080* processor, uint8_t *reg)
{
    uint8_t res = *reg - 1;
    FlagsZSP(processor, res);
    *reg = res;

    processor->ProgrammCounter++;
}
void DCR_B(Processor8080* processor, unsigned char *opcode)
{
    DCR(processor, &processor->B);
}
void DCR_C(Processor8080* processor, unsigned char *opcode)
{
    DCR(processor, &processor->C);
}
void DCR_D(Processor8080* processor, unsigned char *opcode)
{
    DCR(processor, &processor->D);
}
void DCR_E(Processor8080* processor, unsigned char *opcode)
{
    DCR(processor, &processor->E);
}
void DCR_H(Processor8080* processor, unsigned char *opcode)
{
    DCR(processor, &processor->H);
}
void DCR_L(Processor8080* processor, unsigned char *opcode)
{
    DCR(processor, &processor->L);
}
void DCR_M(Processor8080* processor, unsigned char *opcode)
{
    uint16_t address = (processor->H << 8) | processor->L;
    DCR(processor, &processor->Memory[address]);
}
void DCR_A(Processor8080* processor, unsigned char *opcode)
{
    DCR(processor, &processor->A);
}

void DCX_B(Processor8080* processor, unsigned char *opcode)
{
    processor->C -= 1;
	if (processor->C == 0xff)
        processor->B -=1 ;

    processor->ProgrammCounter++;
}
void DCX_D(Processor8080* processor, unsigned char *opcode)
{
    processor->E -= 1;
	if (processor->E == 0xff)
        processor->D -=1 ;

    processor->ProgrammCounter++;
}
void DCX_H(Processor8080* processor, unsigned char *opcode)
{
    processor->L -= 1;
	if (processor->L == 0xff)
        processor->H -= 1;

    processor->ProgrammCounter++;
}
void DCX_SP(Processor8080* processor, unsigned char *opcode)
{
    processor->StackPointer--;

    processor->ProgrammCounter++;
}

void SHLD(Processor8080* processor, unsigned char *opcode)
{
    uint16_t address = (opcode[2] << 8) | opcode[1];

    WriteMem(processor, address, processor->L);
    WriteMem(processor, address + 1, processor->H);

    processor->ProgrammCounter += 3;
}
void LHLD(Processor8080* processor, unsigned char *opcode)
{
    uint16_t address = (opcode[2] << 8) | opcode[1];

    processor->L = processor->Memory[address];
    processor->H = processor->Memory[address + 1];

    processor->ProgrammCounter += 3; 
}

void RNZ(Processor8080* processor, unsigned char *opcode)
{
    if (0 == processor->Flag.Z) 
    {
        processor->ProgrammCounter = processor->Memory[processor->StackPointer] | (processor->Memory[processor->StackPointer + 1] <<8);
		processor->StackPointer += 2;
    }
    else
    {
        processor->ProgrammCounter++;
    }
}
void RNC(Processor8080* processor, unsigned char *opcode)
{
    if (0 == processor->Flag.CY) 
    {
        processor->ProgrammCounter = processor->Memory[processor->StackPointer] | (processor->Memory[processor->StackPointer+1]<<8);
		processor->StackPointer += 2;
    }
    else
    {
        processor->ProgrammCounter++;
    }
}
void RPO(Processor8080* processor, unsigned char *opcode)
{
    if (0 == processor->Flag.P)
    {
        processor->ProgrammCounter = processor->Memory[processor->StackPointer] | (processor->Memory[processor->StackPointer+1]<<8);
		processor->StackPointer += 2;
    }
    else
    {
        processor->ProgrammCounter++;
    }
}
void RP(Processor8080* processor, unsigned char *opcode)
{
    if (0 == processor->Flag.S) 
    {
        processor->ProgrammCounter = processor->Memory[processor->StackPointer] | (processor->Memory[processor->StackPointer+1]<<8);
		processor->StackPointer += 2;
    }
    else
    {
        processor->ProgrammCounter++;
    }
}

void XTHL(Processor8080 *processor, unsigned char *opcode)
{
    uint8_t h = processor->H;
    uint8_t l = processor->L;

    processor->L = processor->Memory[processor->StackPointer];
    processor->H = processor->Memory[processor->StackPointer + 1];   

    WriteMem(processor, processor->StackPointer, l);
    WriteMem(processor, processor->StackPointer + 1, h);

    processor->ProgrammCounter++;
}
void DI(Processor8080 *processor, unsigned char *opcode)
{
    processor->int_enable = 0;

    processor->ProgrammCounter++;
}

void CNZ(Processor8080 *processor, unsigned char *opcode)
{
    if (0 == processor->Flag.Z) 
    {
        uint16_t ret = processor->ProgrammCounter + 3;
        WriteMem(processor, processor->StackPointer-1, (ret >> 8) & 0xff);
        WriteMem(processor, processor->StackPointer-2, (ret & 0xff));
		processor->StackPointer = processor->StackPointer - 2;
		processor->ProgrammCounter = (opcode[2] << 8) | opcode[1];
    } 
    else 
    {
        processor->ProgrammCounter += 3;
    }
}
void CNC(Processor8080 *processor, unsigned char *opcode)
{
    if (0 == processor->Flag.CY) 
    {
        uint16_t ret = processor->ProgrammCounter + 3;
        processor->StackPointer = processor->StackPointer - 2;
        WriteMem(processor, processor->StackPointer + 1, (ret >> 8) & 0xff);
        WriteMem(processor, processor->StackPointer, (ret & 0xff));
        processor->ProgrammCounter = (opcode[2] << 8) | opcode[1];
    } 
    else 
    {
        processor->ProgrammCounter += 3;
    }
}
void CPO(Processor8080 *processor, unsigned char *opcode)
{
    if (0 == processor->Flag.P) 
    {
        uint16_t ret = processor->ProgrammCounter + 3;
        WriteMem(processor, processor->StackPointer-1, (ret >> 8) & 0xff);
        WriteMem(processor, processor->StackPointer-2, (ret & 0xff));
        processor->StackPointer = processor->StackPointer - 2;
        processor->ProgrammCounter = (opcode[2] << 8) | opcode[1];
    } 
    else 
    {
        processor->ProgrammCounter += 3;
    }
}
void CP(Processor8080 *processor, unsigned char *opcode)
{
    if (0 == processor->Flag.S) 
    {
        uint16_t ret = processor->ProgrammCounter + 3;
        WriteMem(processor, processor->StackPointer-1, (ret >> 8) & 0xff);
        WriteMem(processor, processor->StackPointer-2, (ret & 0xff));
        processor->StackPointer = processor->StackPointer - 2;
        processor->ProgrammCounter = (opcode[2] << 8) | opcode[1];
    } 
    else 
    {
        processor->ProgrammCounter += 3;
    }
}

void RST(Processor8080 *processor, uint8_t offset)
{
    uint16_t ret = processor->ProgrammCounter + 3;
    WriteMem(processor, processor->StackPointer-1, (ret >> 8) & 0xff);
    WriteMem(processor, processor->StackPointer-2, (ret & 0xff));
    processor->StackPointer = processor->StackPointer - 2;

    processor->ProgrammCounter = offset;
}
void RST_0(Processor8080 *processor, unsigned char *opcode)
{
    RST(processor, 0x0000);
}
void RST_1(Processor8080 *processor, unsigned char *opcode)
{
    RST(processor, 0x0008);
}
void RST_2(Processor8080 *processor, unsigned char *opcode)
{
    RST(processor, 0x10);
}
void RST_3(Processor8080 *processor, unsigned char *opcode)
{
    RST(processor, 0x18);
}
void RST_4(Processor8080 *processor, unsigned char *opcode)
{
    RST(processor, 0x20);
}
void RST_5(Processor8080 *processor, unsigned char *opcode)
{
    RST(processor, 0x28);
}
void RST_6(Processor8080 *processor, unsigned char *opcode)
{
    RST(processor, 0x30);
}
void RST_7(Processor8080 *processor, unsigned char *opcode)
{
    RST(processor, 0x38);
}
void RZ(Processor8080 *processor, unsigned char *opcode)
{
    if (processor->Flag.Z) 
    {
        processor->ProgrammCounter = (processor->Memory[processor->StackPointer+1] << 8 | processor->Memory[processor->StackPointer]);
		processor->StackPointer += 2;
    }
    else 
    {
        processor->ProgrammCounter++;
    }
}
void RC(Processor8080 *processor, unsigned char *opcode)
{
    if (0 != processor->Flag.CY) 
    {
        processor->ProgrammCounter = processor->Memory[processor->StackPointer] | (processor->Memory[processor->StackPointer+1]<<8);
        processor->StackPointer += 2;
    }
    else 
    {
        processor->ProgrammCounter++;
    }
}
void RPE(Processor8080 *processor, unsigned char *opcode)
{
    if (0 != processor->Flag.P) 
    {
        processor->ProgrammCounter = processor->Memory[processor->StackPointer] | (processor->Memory[processor->StackPointer+1]<<8);
		processor->StackPointer += 2;
    }
    else 
    {
        processor->ProgrammCounter++;
    }
}
void RM(Processor8080 *processor, unsigned char *opcode)
{
    if (0 != processor->Flag.S) 
    {
        processor->ProgrammCounter = processor->Memory[processor->StackPointer] | (processor->Memory[processor->StackPointer+1]<<8);
		processor->StackPointer += 2;
    }
    else 
    {
        processor->ProgrammCounter++;
    }
}

void PCHL(Processor8080 *processor, unsigned char *opcode) 
{
    processor->ProgrammCounter = (processor->H << 8) | processor->L;
}
void SPHL(Processor8080 *processor, unsigned char *opcode) 
{
    processor->StackPointer = (processor->H << 8) | processor->L;

    processor->ProgrammCounter++;
}

void JC(Processor8080 *processor, unsigned char *opcode)
{
    if (0 != processor->Flag.CY) 
    {
        processor->ProgrammCounter = ((opcode[2] << 8) | opcode[1]);
    } 
    else 
    {
        processor->ProgrammCounter += 3;
    }
}
void JZ(Processor8080 *processor, unsigned char *opcode)
{
    if (processor->Flag.Z) 
    {
        processor->ProgrammCounter = ((opcode[2] << 8) | opcode[1]);
    } 
    else 
    {
        processor->ProgrammCounter += 3;
    }   
}
void JPE(Processor8080 *processor, unsigned char *opcode)
{
    if (0 != processor->Flag.P) 
    {
        processor->ProgrammCounter = ((opcode[2] << 8) | opcode[1]);
    } 
    else 
    {
        processor->ProgrammCounter += 3;
    }   
}
void JM(Processor8080 *processor, unsigned char *opcode)
{
    if (0 != processor->Flag.S) 
    {
        processor->ProgrammCounter = ((opcode[2] << 8) | opcode[1]);
    } 
    else 
    {
        processor->ProgrammCounter += 3;
    }   
}

void IN(Processor8080 *processor, unsigned char *opcode)
{
    processor->ProgrammCounter += 2;
}

void CZ(Processor8080 *processor, unsigned char *opcode)
{
    if (1 == processor->Flag.Z) 
    {
        uint16_t ret = processor->ProgrammCounter + 3;
        WriteMem(processor, processor->StackPointer-1, (ret >> 8) & 0xff);
        WriteMem(processor, processor->StackPointer-2, (ret & 0xff));
        processor->StackPointer = processor->StackPointer - 2;
        processor->ProgrammCounter = (opcode[2] << 8) | opcode[1];
    } 
    else 
    {
        processor->ProgrammCounter += 3;
    }
}
void CC(Processor8080 *processor, unsigned char *opcode)
{
    if (0 != processor->Flag.CY) 
    {
        uint16_t ret = processor->ProgrammCounter + 3;
        WriteMem(processor, processor->StackPointer-1, (ret >> 8) & 0xff);
        WriteMem(processor, processor->StackPointer-2, (ret & 0xff));
        processor->StackPointer = processor->StackPointer - 2;
        processor->ProgrammCounter = (opcode[2] << 8) | opcode[1];
    } 
    else
    {
        processor->ProgrammCounter += 3;
    }
}
void CPE(Processor8080 *processor, unsigned char *opcode)
{
    if (0 != processor->Flag.P) 
    {
        uint16_t ret = processor->ProgrammCounter + 3;
        WriteMem(processor, processor->StackPointer-1, (ret >> 8) & 0xff);
        WriteMem(processor, processor->StackPointer-2, (ret & 0xff));
		processor->StackPointer = processor->StackPointer - 2;
		processor->ProgrammCounter = (opcode[2] << 8) | opcode[1];
    } 
    else
    {
        processor->ProgrammCounter += 3;
    }
}
void CM(Processor8080 *processor, unsigned char *opcode)
{
    if (0 != processor->Flag.CY) 
    {
        uint16_t ret = processor->ProgrammCounter+3;
        WriteMem(processor, processor->StackPointer-1, (ret >> 8) & 0xff);
        WriteMem(processor, processor->StackPointer-2, (ret & 0xff));
        processor->StackPointer = processor->StackPointer - 2;
        processor->ProgrammCounter = (opcode[2] << 8) | opcode[1];
    } 
    else
    {
        processor->ProgrammCounter += 3;
    }
}

void ACI(Processor8080 *processor, unsigned char *opcode)
{
    uint16_t result = processor->A + opcode[1] + processor->Flag.CY;
    FlagsZSP(processor, result&0xff);
    processor->Flag.CY = (result > 0xff);
    processor->A = result & 0xFF;
    
    processor->ProgrammCounter += 2; 
}
void SBI(Processor8080 *processor, unsigned char *opcode)
{
    uint16_t result = processor->A - opcode[1] - processor->Flag.CY;
    FlagsZSP(processor, result & 0xff);
    processor->Flag.CY = (result > 0xff);
    processor->A = result & 0xFF;
    
    processor->ProgrammCounter += 2; 
}
void XRI(Processor8080 *processor, unsigned char *opcode)
{
    uint8_t result = processor->A ^ opcode[1];
    FlagsZSP(processor, result);
	processor->Flag.CY = 0;
	processor->A = result;
    
    processor->ProgrammCounter += 2;
}
void CPI(Processor8080 *processor, unsigned char *opcode)
{
    uint8_t a = processor->A - opcode[1];
    FlagsZSP(processor, a);
    processor->Flag.CY = (processor->A < opcode[1]);

    processor->ProgrammCounter += 2;
}

// Таблица инструкций процессора Intel 8080: https://www.pastraiser.com/cpu/i8080/i8080_opcodes.html
// Таблица функциональности опкодов: https://storage.googleapis.com/wzukusers/user-34792836/documents/5ca6602deded5ndTp15a/8080_OPCODES.pdf;
const Instructions instructions[256] = 
{
    // 0x00 - 0x0f
    &NOP, &LXI_B, &STAX_B, &INX_B, &INR_B, &DCR_B, &MVI_B, &RLC, 
    &NOP, &DAD_B, &LDAX_B, &DCX_B, &INR_C, &DCR_C, &MVI_C, &RRC,

    // 0x10 - 0x1f
    &NOP, &LXI_D, &STAX_D, &INX_D, &INR_D, &DCR_D, &MVI_D, &RAL,
    &NOP, &DAD_D, &LDAX_D, &DCX_D, &INR_E, &DCR_E, &MVI_E, &RAR,

    // 0x20 - 0x2f
    &NOP, &LXI_H, &SHLD, &INX_H, &INR_H, &DCR_H, &MVI_H, &DAA,
    &NOP, &DAD_H, &LHLD, &DCX_H, &INR_L, &DCR_L, &MVI_L, &CMA,

    // 0x30 - 0x3f
    &NOP, &LXI_SP, &STA, &INX_SP, &INR_M, &DCR_M, &MVI_M, &STC,
    &NOP, &DAD_SP, &LDA, &DCX_SP, &INR_A, &DCR_A, &MVI_A, &CMC,

    // 0x40 - 0x4f
    &MOV_B_B, &MOV_B_C, &MOV_B_D, &MOV_B_E, &MOV_B_H, &MOV_B_L, &MOV_B_M, &MOV_B_A,
    &MOV_C_B, &MOV_C_C, &MOV_C_D, &MOV_C_E, &MOV_C_H, &MOV_C_L, &MOV_C_M, &MOV_C_A,

    // 0x50 - 0x5f
    &MOV_D_B, &MOV_D_C, &MOV_D_D, &MOV_D_E, &MOV_D_H, &MOV_D_L, &MOV_D_M, &MOV_D_A,
    &MOV_E_B, &MOV_E_C, &MOV_E_D, &MOV_E_E, &MOV_E_H, &MOV_E_L, &MOV_E_M, &MOV_E_A,

    // 0x60 - 0x6f
    &MOV_H_B, &MOV_H_C, &MOV_H_D, &MOV_H_E, &MOV_H_H, &MOV_H_L, &MOV_H_M, &MOV_H_A,
    &MOV_L_B, &MOV_L_C, &MOV_L_D, &MOV_L_E, &MOV_L_H, &MOV_L_L, &MOV_L_M, &MOV_L_A,

    // 0x70 - 0x7f
    &MOV_M_B, &MOV_M_C, &MOV_M_D, &MOV_M_E, &MOV_M_H, &MOV_M_L, &HLT, &MOV_M_A,
    &MOV_A_B, &MOV_A_C, &MOV_A_D, &MOV_A_E, &MOV_A_H, &MOV_A_L, &MOV_A_M, &MOV_A_A,

    // 0x80 - 0x8f
    &ADD_B, &ADD_C, &ADD_D, &ADD_E, &ADD_H, &ADD_L, &ADD_M, &ADD_A,
    &ADC_B, &ADC_C, &ADC_D, &ADC_E, &ADC_H, &ADC_L, &ADC_M, &ADC_A,

    // 0x90 - 0x9f
    &SUB_B, &SUB_C, &SUB_D, &SUB_E, &SUB_H, &SUB_L, &SUB_M, &SUB_A,
    &SBB_B, &SBB_C, &SBB_D, &SBB_E, &SBB_H, &SBB_L, &SBB_M, &SBB_A,

    // 0xa0 - 0xaf
    &ANA_B, &ANA_C, &ANA_D, &ANA_E, &ANA_H, &ANA_L, &ANA_M, &ANA_A,
    &XRA_B, &XRA_C, &XRA_D, &XRA_E, &XRA_H, &XRA_L, &XRA_M, &XRA_A,

    // 0xb0 - 0xb1
    &ORA_B, &ORA_C, &ORA_D, &ORA_E, &ORA_H, &ORA_L, &ORA_M, &ORA_A,
    &CMP_B, &CMP_C, &CMP_D, &CMP_E, &CMP_H, &CMP_L, &CMP_M, &CMP_A,

    // 0xc0 - 0xcf
    &RNZ, &POP_B, &JNZ, &JMP, &CNZ, &PUSH_B, &ADI, &RST_0, 
    &RZ, &RET, &JZ, &JMP, &CZ, &CALL, &ACI, &RST_1,

    // 0xd0 - 0xdf
    &RNC, &POP_D, &JNC, &OUT, &CNC, &PUSH_D, &SUI, &RST_2,
    &RC, &RET, &JC, &IN, &CC, &CALL, &SBI, &RST_3,

    // 0xe0 - 0xef
    &RPO, &POP_H, &JPO, &XTHL, &CPO, &PUSH_H, &ANI, &RST_4,
    &RPE, &PCHL, &JPE, &XCHG, &CPE, &CALL, &XRI, &RST_5,

    // 0xf0 - 0x-ff
    &RP, &POP_PSW, &JP, &DI, &CP, &PUSH_PSW, &ORI, &RST_6,
    &RM, &SPHL, &JM, &EI, &CM, &CALL, &CPI, &RST_7,
};

const uint8_t cycles[256] = {
    4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x00..0x0f
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x10..0x1f
	4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4, //etc
	4, 10, 13, 5, 10, 10, 10, 4, 4, 10, 13, 5, 5, 5, 7, 4,
	
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5, //0x40..0x4f
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,
	
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4, //0x80..8x4f
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	
	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11, //0xc0..0xcf
	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11, 
	11, 10, 10, 18, 17, 11, 7, 11, 11, 5, 10, 5, 17, 17, 7, 11, 
	11, 10, 10, 4, 17, 11, 7, 11, 11, 5, 10, 4, 17, 17, 7, 11, 
};
#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Flags {    
    uint8_t    Z:1;    
    uint8_t    S:1;    
    uint8_t    P:1;    
    uint8_t    CY:1;    
    uint8_t    AC:1;    
    uint8_t    PAD:3;    
} Flags;

typedef struct Processor8080 {    
    uint8_t    A;    
    uint8_t    B;    
    uint8_t    C;    
    uint8_t    D;    
    uint8_t    E;    
    uint8_t    H;    
    uint8_t    L;    
    uint16_t   StackPointer;    
    uint16_t   ProgrammCounter;
    uint64_t   CyclesCounter;    
    uint8_t    *Memory;    
    Flags       Flag;    
    uint8_t     int_enable;    
    uint8_t     halted;
    uint8_t     p1_input_port;
	uint8_t     p2_input_port;
} Processor8080;

// Массив инструкций ассемблера Intel 8080
// Таблица инструкций процессора Intel 8080: https://www.pastraiser.com/cpu/i8080/i8080_opcodes.html
// Таблица функциональности опкодов: https://storage.googleapis.com/wzukusers/user-34792836/documents/5ca6602deded5ndTp15a/8080_OPCODES.pdf;
typedef void (*Instructions) (Processor8080*, unsigned char*);
extern const Instructions instructions[256];
extern const uint8_t cycles[256];

// Функции для работы
int parity(int value);
void WriteMem(Processor8080* processor, uint16_t address, uint8_t value);
void WriteToHL(Processor8080* processor, uint8_t value);
void Push(Processor8080* processor, uint8_t high, uint8_t low);
void Pop(Processor8080* processor, uint8_t *high, uint8_t *low);
void LogicFlagsA(Processor8080 *processor);
void ArithFlagsA(Processor8080 *processor, uint16_t res);
void FlagsZSP(Processor8080 *processor, uint8_t value);

// Функции опкодов
void STAX_B(Processor8080 *processor, unsigned char *opcode);
void STAX_D(Processor8080 *processor, unsigned char *opcode);
void NOP(Processor8080* processor, unsigned char *opcode);
void EI(Processor8080* processor, unsigned char *opcode);
void XCHG(Processor8080* processor, unsigned char *opcode);
void SUI(Processor8080* processor, unsigned char *opcode);
void ORI(Processor8080* processor, unsigned char *opcode);
void ANI(Processor8080* processor, unsigned char *opcode);
void ADI(Processor8080* processor, unsigned char *opcode);
void OUT(Processor8080* processor, unsigned char *opcode);
void CALL(Processor8080* processor, unsigned char *opcode);
void JMP(Processor8080* processor, unsigned char *opcode);
void RET(Processor8080* processor, unsigned char *opcode);

void PUSH_PSW(Processor8080* processor, unsigned char *opcode);
void PUSH_H(Processor8080* processor, unsigned char *opcode);
void PUSH_D(Processor8080* processor, unsigned char *opcode);
void PUSH_B(Processor8080* processor, unsigned char *opcode);

void JNZ(Processor8080* processor, unsigned char *opcode);
void JNC(Processor8080* processor, unsigned char *opcode);
void JPO(Processor8080* processor, unsigned char *opcode);
void JP(Processor8080* processor, unsigned char *opcode);

void POP_PSW(Processor8080* processor, unsigned char *opcode);
void POP_H(Processor8080* processor, unsigned char *opcode);
void POP_D(Processor8080* processor, unsigned char *opcode);
void POP_B(Processor8080* processor, unsigned char *opcode);

void XRA(Processor8080* processor, unsigned char *opcode, uint8_t reg);
void XRA_A(Processor8080* processor, unsigned char *opcode);
void XRA_B(Processor8080* processor, unsigned char *opcode);
void XRA_C(Processor8080* processor, unsigned char *opcode);
void XRA_D(Processor8080* processor, unsigned char *opcode);
void XRA_E(Processor8080* processor, unsigned char *opcode);
void XRA_H(Processor8080* processor, unsigned char *opcode);
void XRA_L(Processor8080* processor, unsigned char *opcode);
void XRA_M(Processor8080* processor, unsigned char *opcode);

void ANA(Processor8080* processor, unsigned char *opcode, uint8_t reg);
void ANA_A(Processor8080* processor, unsigned char *opcode);
void ANA_B(Processor8080* processor, unsigned char *opcode);
void ANA_C(Processor8080* processor, unsigned char *opcode);
void ANA_D(Processor8080* processor, unsigned char *opcode);
void ANA_E(Processor8080* processor, unsigned char *opcode);
void ANA_H(Processor8080* processor, unsigned char *opcode);
void ANA_L(Processor8080* processor, unsigned char *opcode);
void ANA_M(Processor8080* processor, unsigned char *opcode);
void LDA(Processor8080* processor, unsigned char *opcode);
void STA(Processor8080* processor, unsigned char *opcode);
void LDAX_B(Processor8080* processor, unsigned char *opcode);
void LDAX_D(Processor8080* processor, unsigned char *opcode);
void INX_B(Processor8080* processor, unsigned char *opcode);
void INX_H(Processor8080* processor, unsigned char *opcode);
void INX_D(Processor8080* processor, unsigned char *opcode);
void INX_SP(Processor8080* processor, unsigned char *opcode);
void RRC(Processor8080* processor, unsigned char *opcode);
void RLC(Processor8080* processor, unsigned char *opcode);
void RAL(Processor8080* processor, unsigned char *opcode);
void RAR(Processor8080* processor, unsigned char *opcode);
void DAA(Processor8080* processor, unsigned char *opcode);
void CMA(Processor8080* processor, unsigned char *opcode);
void STC(Processor8080* processor, unsigned char *opcode);
void CMC(Processor8080* processor, unsigned char *opcode);

void DAD_H(Processor8080* processor, unsigned char *opcode);
void DAD_D(Processor8080* processor, unsigned char *opcode);
void DAD_B(Processor8080* processor, unsigned char *opcode);
void DAD_SP(Processor8080 *processor, unsigned char *opcode);

void MVI_A(Processor8080* processor, unsigned char *opcode);
void MVI_B(Processor8080* processor, unsigned char *opcode);
void MVI_C(Processor8080* processor, unsigned char *opcode);
void MVI_D(Processor8080* processor, unsigned char *opcode);
void MVI_E(Processor8080* processor, unsigned char *opcode);
void MVI_H(Processor8080* processor, unsigned char *opcode);
void MVI_L(Processor8080* processor, unsigned char *opcode);
void MVI_M(Processor8080* processor, unsigned char *opcode);

void LXI_SP(Processor8080* processor, unsigned char *opcode);
void LXI_H(Processor8080* processor, unsigned char *opcode);
void LXI_D(Processor8080* processor, unsigned char *opcode);
void LXI_B(Processor8080* processor, unsigned char *opcode);

void MOV_A_A(Processor8080* processor, unsigned char *opcode);
void MOV_A_B(Processor8080* processor, unsigned char *opcode);
void MOV_A_C(Processor8080* processor, unsigned char *opcode);
void MOV_A_D(Processor8080* processor, unsigned char *opcode);
void MOV_A_E(Processor8080* processor, unsigned char *opcode);
void MOV_A_H(Processor8080* processor, unsigned char *opcode);
void MOV_A_L(Processor8080* processor, unsigned char *opcode);
void MOV_A_M(Processor8080* processor, unsigned char *opcode);

void MOV_B_A(Processor8080* processor, unsigned char *opcode);
void MOV_B_B(Processor8080* processor, unsigned char *opcode);
void MOV_B_C(Processor8080* processor, unsigned char *opcode);
void MOV_B_D(Processor8080* processor, unsigned char *opcode);
void MOV_B_E(Processor8080* processor, unsigned char *opcode);
void MOV_B_H(Processor8080* processor, unsigned char *opcode);
void MOV_B_L(Processor8080* processor, unsigned char *opcode);
void MOV_B_M(Processor8080* processor, unsigned char *opcode);

void MOV_C_A(Processor8080* processor, unsigned char *opcode);
void MOV_C_B(Processor8080* processor, unsigned char *opcode);
void MOV_C_C(Processor8080* processor, unsigned char *opcode);
void MOV_C_D(Processor8080* processor, unsigned char *opcode);
void MOV_C_E(Processor8080* processor, unsigned char *opcode);
void MOV_C_H(Processor8080* processor, unsigned char *opcode);
void MOV_C_L(Processor8080* processor, unsigned char *opcode);
void MOV_C_M(Processor8080* processor, unsigned char *opcode);

void MOV_D_A(Processor8080* processor, unsigned char *opcode);
void MOV_D_B(Processor8080* processor, unsigned char *opcode);
void MOV_D_C(Processor8080* processor, unsigned char *opcode);
void MOV_D_D(Processor8080* processor, unsigned char *opcode);
void MOV_D_E(Processor8080* processor, unsigned char *opcode);
void MOV_D_H(Processor8080* processor, unsigned char *opcode);
void MOV_D_L(Processor8080* processor, unsigned char *opcode);
void MOV_D_M(Processor8080* processor, unsigned char *opcode);

void MOV_E_A(Processor8080* processor, unsigned char *opcode);
void MOV_E_B(Processor8080* processor, unsigned char *opcode);
void MOV_E_C(Processor8080* processor, unsigned char *opcode);
void MOV_E_D(Processor8080* processor, unsigned char *opcode);
void MOV_E_E(Processor8080* processor, unsigned char *opcode);
void MOV_E_H(Processor8080* processor, unsigned char *opcode);
void MOV_E_L(Processor8080* processor, unsigned char *opcode);
void MOV_E_M(Processor8080* processor, unsigned char *opcode);

void MOV_H_A(Processor8080* processor, unsigned char *opcode);
void MOV_H_B(Processor8080* processor, unsigned char *opcode);
void MOV_H_C(Processor8080* processor, unsigned char *opcode);
void MOV_H_D(Processor8080* processor, unsigned char *opcode);
void MOV_H_E(Processor8080* processor, unsigned char *opcode);
void MOV_H_H(Processor8080* processor, unsigned char *opcode);
void MOV_H_L(Processor8080* processor, unsigned char *opcode);
void MOV_H_M(Processor8080* processor, unsigned char *opcode);

void MOV_L_A(Processor8080* processor, unsigned char *opcode);
void MOV_L_B(Processor8080* processor, unsigned char *opcode);
void MOV_L_C(Processor8080* processor, unsigned char *opcode);
void MOV_L_D(Processor8080* processor, unsigned char *opcode);
void MOV_L_E(Processor8080* processor, unsigned char *opcode);
void MOV_L_H(Processor8080* processor, unsigned char *opcode);
void MOV_L_L(Processor8080* processor, unsigned char *opcode);
void MOV_L_M(Processor8080* processor, unsigned char *opcode);

void MOV_M_A(Processor8080* processor, unsigned char *opcode);
void MOV_M_B(Processor8080* processor, unsigned char *opcode);
void MOV_M_C(Processor8080* processor, unsigned char *opcode);
void MOV_M_D(Processor8080* processor, unsigned char *opcode);
void MOV_M_E(Processor8080* processor, unsigned char *opcode);
void MOV_M_H(Processor8080* processor, unsigned char *opcode);
void MOV_M_L(Processor8080* processor, unsigned char *opcode);

void HLT(Processor8080* processor, unsigned char *opcode);

void ADD(Processor8080* processor, unsigned char *opcode, uint8_t reg);
void ADD_A(Processor8080* processor, unsigned char *opcode);
void ADD_B(Processor8080* processor, unsigned char *opcode);
void ADD_C(Processor8080* processor, unsigned char *opcode);
void ADD_D(Processor8080* processor, unsigned char *opcode);
void ADD_E(Processor8080* processor, unsigned char *opcode);
void ADD_H(Processor8080* processor, unsigned char *opcode);
void ADD_L(Processor8080* processor, unsigned char *opcode);
void ADD_M(Processor8080* processor, unsigned char *opcode);

void ADC(Processor8080* processor, unsigned char *opcode, uint8_t reg);
void ADC_A(Processor8080* processor, unsigned char *opcode);
void ADC_B(Processor8080* processor, unsigned char *opcode);
void ADC_C(Processor8080* processor, unsigned char *opcode);
void ADC_D(Processor8080* processor, unsigned char *opcode);
void ADC_E(Processor8080* processor, unsigned char *opcode);
void ADC_H(Processor8080* processor, unsigned char *opcode);
void ADC_L(Processor8080* processor, unsigned char *opcode);
void ADC_M(Processor8080* processor, unsigned char *opcode);

void SUB(Processor8080* processor, unsigned char *opcode, uint8_t reg);
void SUB_A(Processor8080* processor, unsigned char *opcode);
void SUB_B(Processor8080* processor, unsigned char *opcode);
void SUB_C(Processor8080* processor, unsigned char *opcode);
void SUB_D(Processor8080* processor, unsigned char *opcode);
void SUB_E(Processor8080* processor, unsigned char *opcode);
void SUB_H(Processor8080* processor, unsigned char *opcode);
void SUB_L(Processor8080* processor, unsigned char *opcode);
void SUB_M(Processor8080* processor, unsigned char *opcode);

void SBB(Processor8080* processor, unsigned char *opcode, uint8_t reg);
void SBB_A(Processor8080* processor, unsigned char *opcode);
void SBB_B(Processor8080* processor, unsigned char *opcode);
void SBB_C(Processor8080* processor, unsigned char *opcode);
void SBB_D(Processor8080* processor, unsigned char *opcode);
void SBB_E(Processor8080* processor, unsigned char *opcode);
void SBB_H(Processor8080* processor, unsigned char *opcode);
void SBB_L(Processor8080* processor, unsigned char *opcode);
void SBB_M(Processor8080* processor, unsigned char *opcode);

void ORA(Processor8080* processor, unsigned char *opcode, uint8_t reg);
void ORA_A(Processor8080* processor, unsigned char *opcode);
void ORA_B(Processor8080* processor, unsigned char *opcode);
void ORA_C(Processor8080* processor, unsigned char *opcode);
void ORA_D(Processor8080* processor, unsigned char *opcode);
void ORA_E(Processor8080* processor, unsigned char *opcode);
void ORA_H(Processor8080* processor, unsigned char *opcode);
void ORA_L(Processor8080* processor, unsigned char *opcode);
void ORA_M(Processor8080* processor, unsigned char *opcode);

void CMP(Processor8080* processor, unsigned char *opcode, uint8_t reg);
void CMP_A(Processor8080* processor, unsigned char *opcode);
void CMP_B(Processor8080* processor, unsigned char *opcode);
void CMP_C(Processor8080* processor, unsigned char *opcode);
void CMP_D(Processor8080* processor, unsigned char *opcode);
void CMP_E(Processor8080* processor, unsigned char *opcode);
void CMP_H(Processor8080* processor, unsigned char *opcode);
void CMP_L(Processor8080* processor, unsigned char *opcode);
void CMP_M(Processor8080* processor, unsigned char *opcode);

void INR(Processor8080* processor, uint8_t *reg);
void INR_A(Processor8080* processor, unsigned char *opcode);
void INR_B(Processor8080* processor, unsigned char *opcode);
void INR_C(Processor8080* processor, unsigned char *opcode);
void INR_D(Processor8080* processor, unsigned char *opcode);
void INR_E(Processor8080* processor, unsigned char *opcode);
void INR_H(Processor8080* processor, unsigned char *opcode);
void INR_L(Processor8080* processor, unsigned char *opcode);
void INR_M(Processor8080* processor, unsigned char *opcode);

void DCR(Processor8080* processor, uint8_t *reg);
void DCR_A(Processor8080* processor, unsigned char *opcode);
void DCR_B(Processor8080* processor, unsigned char *opcode);
void DCR_C(Processor8080* processor, unsigned char *opcode);
void DCR_D(Processor8080* processor, unsigned char *opcode);
void DCR_E(Processor8080* processor, unsigned char *opcode);
void DCR_H(Processor8080* processor, unsigned char *opcode);
void DCR_L(Processor8080* processor, unsigned char *opcode);
void DCR_M(Processor8080* processor, unsigned char *opcode);

void DCX_B(Processor8080* processor, unsigned char *opcode);
void DCX_D(Processor8080* processor, unsigned char *opcode);
void DCX_H(Processor8080* processor, unsigned char *opcode);
void DCX_SP(Processor8080* processor, unsigned char *opcode);

void SHLD(Processor8080* processor, unsigned char *opcode);
void LHLD(Processor8080* processor, unsigned char *opcode);
void RNZ(Processor8080* processor, unsigned char *opcode);
void RNC(Processor8080* processor, unsigned char *opcode);
void RPO(Processor8080* processor, unsigned char *opcode);
void RP(Processor8080* processor, unsigned char *opcode);
void XTHL(Processor8080 *processor, unsigned char *opcode);
void DI(Processor8080 *processor, unsigned char *opcode);
void CNZ(Processor8080 *processor, unsigned char *opcode);
void CNC(Processor8080 *processor, unsigned char *opcode);
void CPO(Processor8080 *processor, unsigned char *opcode);
void CP(Processor8080 *processor, unsigned char *opcode);

void RST(Processor8080 *processor, uint8_t offset);
void RST_0(Processor8080 *processor, unsigned char *opcode);
void RST_1(Processor8080 *processor, unsigned char *opcode);
void RST_2(Processor8080 *processor, unsigned char *opcode);
void RST_3(Processor8080 *processor, unsigned char *opcode);
void RST_4(Processor8080 *processor, unsigned char *opcode);
void RST_5(Processor8080 *processor, unsigned char *opcode);
void RST_6(Processor8080 *processor, unsigned char *opcode);
void RST_7(Processor8080 *processor, unsigned char *opcode);

void RZ(Processor8080 *processor, unsigned char *opcode);
void RC(Processor8080 *processor, unsigned char *opcode);
void RPE(Processor8080 *processor, unsigned char *opcode);
void RM(Processor8080 *processor, unsigned char *opcode);
void PCHL(Processor8080 *processor, unsigned char *opcode);
void SPHL(Processor8080 *processor, unsigned char *opcode);
void JC(Processor8080 *processor, unsigned char *opcode);
void JZ(Processor8080 *processor, unsigned char *opcode);
void JPE(Processor8080 *processor, unsigned char *opcode);
void JM(Processor8080 *processor, unsigned char *opcode);
void IN(Processor8080 *processor, unsigned char *opcode);
void CZ(Processor8080 *processor, unsigned char *opcode);
void CC(Processor8080 *processor, unsigned char *opcode);
void CPE(Processor8080 *processor, unsigned char *opcode);
void CM(Processor8080 *processor, unsigned char *opcode);
void ACI(Processor8080 *processor, unsigned char *opcode);
void SBI(Processor8080 *processor, unsigned char *opcode);
void XRI(Processor8080 *processor, unsigned char *opcode);
void CPI(Processor8080 *processor, unsigned char *opcode);
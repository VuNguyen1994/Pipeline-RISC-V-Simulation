#ifndef __CORE_H__
#define __CORE_H__

#include "Instruction_Memory.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define BOOL bool
#define MAX_INT 4294967296

/*Control Signal*/
struct SIGNAL{
    bool Branch, MemRead, MemtoReg, MemWrite, ALUSrc, RegWrite;
    uint8_t ALUOp;
}signal;
/*Binary Instruction Element*/
struct inst_element
{
    unsigned rs1;
    unsigned rs2;
    unsigned rd;
    unsigned opcode;
    uint64_t im;
    unsigned f3;
    unsigned f7;
}bininstruction;


/*IF Stage Config*/
struct IF
{
    Addr PC;
}IF_Stage;

struct IFID_Register
{
    Addr PC;
    unsigned IFID_register; //32 bit instruction
}IF_ID_Register;


/*ID Stage Config*/
struct ID //Get data from IFID Register
{
    Addr PC;
    unsigned fetched_instruction;
}ID_Stage;

struct IDEX_Register 
{
    Addr PC;
    /*Store instruction information*/
    unsigned rs1;
    unsigned rs2;
    unsigned rd;
    unsigned opcode;
    uint64_t im;
    unsigned f3;
    unsigned f7;
    /*Store the signal determined by opcode*/
    bool Branch, MemRead, MemtoReg, MemWrite, ALUSrc, RegWrite;
    uint8_t ALUOp;

    /*Read data from register if rs true*/
    int64_t read_dat1;
    int64_t read_dat2;
}ID_EX_Register;


/*EX stage*/
struct EX
{
    Addr PC;
    /*Store instruction information*/
    unsigned rs1;
    unsigned rs2;
    unsigned rd;
    unsigned opcode;
    uint64_t im;
    unsigned f3;
    unsigned f7;
    /*Store the signal determined by opcode*/
    bool Branch, MemRead, MemtoReg, MemWrite, ALUSrc, RegWrite;
    uint8_t ALUOp;
    unsigned alucontrol_signal;
    int64_t read_dat1;
    int64_t read_dat2;
    int64_t input2; //input go to ALU imm or rs2
}EX_Stage;

struct EXMEM_Register
{
    Addr PC;
    unsigned result;
    unsigned alucontrol_signal;
    unsigned opcode;
    int64_t read_dat1;
    int64_t read_dat2;
    int64_t input2;
    unsigned rd;
    uint64_t im;
    /*Store the signal determined by opcode*/
    bool Branch, MemRead, MemtoReg, MemWrite, ALUSrc, RegWrite;
    uint8_t ALUOp;
}EX_MEM_Register;

/*MEM stage*/
struct MEM
{
    Addr PC;
    unsigned new_PC;
    int64_t r_dat;
    int w_dat;
    unsigned opcode;
    unsigned rd;
    uint64_t im;
    unsigned result;
    /*Store the signal determined by opcode*/
    bool Branch, MemRead, MemtoReg, MemWrite, ALUSrc, RegWrite;
    uint8_t ALUOp;
}MEM_Stage;

struct MEMWB_Register
{
    Addr PC;
    int64_t r_dat;
    int w_dat;
    unsigned opcode;
    unsigned rd;
    unsigned result;
    /*Store the signal for next stage*/
    bool MemtoReg, RegWrite;
}MEM_WB_Register;

struct WB
{
    Addr PC;
    int64_t r_dat;
    int w_dat;
    unsigned opcode;
    unsigned rd;
    unsigned result;
    /*Store the signal determined by opcode*/
    bool MemtoReg, RegWrite;
}WB_Stage;

struct Core;
typedef struct Core Core;
typedef struct Core
{
    Tick clk; // Keep track of core clock
    Addr PC; // Keep track of program counter

    long int register_file[32];
    int data_memory[256]; 
    Instruction_Memory *instr_mem;
    
    // TODO, simulation function
    bool (*tick)(Core *core);
}Core;

Core *initCore(Instruction_Memory *i_mem);
bool tickFunc(Core *core);

void decode(unsigned instruction);
void controlsignal(unsigned opcode);
uint64_t mux (bool signal, uint64_t input1, uint64_t input2);
unsigned getALUControl();
uint64_t convert64 (uint64_t imm);
unsigned ALU(int64_t input1, int64_t input2, unsigned ALU_Control);
bool config_branch(unsigned input1, unsigned input2);

#endif

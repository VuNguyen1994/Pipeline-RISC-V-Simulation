#include "Core.h"
#include "Registers.h"

Core *initCore(Instruction_Memory *i_mem)
{
    Core *core = (Core *)malloc(sizeof(Core));
    core->clk = 0;
    core->PC = 0;
    core->instr_mem = i_mem;
    core->tick = tickFunc;
    int i;
    /*Default init - DO NOT CHANGE */
    core->register_file[0] = 0;
    for (i = 1; i < 32; i++) { 
        core->register_file[i] = 0;
    }
    for(int i = 0; i < 16; i++)
	    core->data_memory[i*8] = 1;

    /*Init register value, memory value FOR PROJECT1*/
    // core->register_file[25] = 4;
    // core->register_file[10] = 4;
    // core->register_file[22] = 1;
    
    // core->data_memory[0] = 16;
    // core->data_memory[8] = 128;
    // core ->data_memory[16]= 8;
    // core->data_memory[24] = 4;

    /* Init values for MATRIX Project1*/
    // for (int k = 0; k < 128; k+=8)
    // {
    //     core->data_memory[k] = k/8; 
    // }

    /* UNCOMMENT TO SET DEFAULT VALUES FOR task_0 */
    core->register_file[1] = 0;
    core->register_file[2] = 10;
    core->register_file[3] = -15;
    core->register_file[4] = 20;
    core->register_file[5] = 30;
    core->register_file[6] = -35;
    core->data_memory[40] = -63;
    core->data_memory[48] = 63;

    /* UNCOMMENT TO SET DEFAULT VALUES FOR task_1 */
    // core->register_file[1] = 8;
    // core->register_file[3] = -4;
    // core->register_file[5] = 255;
    // core->register_file[6] = 1023;

    /* UNCOMMENT TO SET DEFAULT VALUES FOR task_1 */
    // core->register_file[1] = 8;
    // core->register_file[3] = -15;
    // core->register_file[5] = 255;
    // core->register_file[6] = 1023;

    /* UNCOMMENT TO SET DEFAULT VALUES FOR task_2 */ 
    // core->register_file[5] = 26;
    // core->register_file[6] = -27;
    // core->data_memory[20] = 100; 
						     
    /* UNCOMMENT TO SET DEFAULT VALUES FOR task_3 */
    // core->register_file[1] = 0;
    // core->register_file[2] = -5;
    // core->register_file[5] = -10;
    // core->register_file[6] = 25;
    // core->data_memory[100] = -100; 

    /* UNCOMMENT TO SET DEFAULT VALUES FOR task_3 */
    // core->register_file[1] = 8;
    // core->register_file[2] = -5;
    // core->register_file[5] = -10;
    // core->register_file[6] = 25;
    // core->data_memory[100] = -100;
    
    return core;
}

/*Control functions*/
/*Check for negative imm + Convert from 12bits to 64 bits*/
uint64_t convert64 (uint64_t imm)
{
    uint64_t imm64 = imm;
    if (bininstruction.opcode == 111) //UJ type - 20 bits
    {
        if (imm & 0x80000)
        {
            imm64= imm | 0xfff00000;
            imm64 -= MAX_INT; //convert to negative signed number
        }
    }
    else //regular opcode 12 bits
    {
        if (imm & 0x800){ //if im negative
            imm64= imm | 0xfffff000;
            imm64 -= MAX_INT;
        } 
    }
    return imm64;
}

/*decode the fields of binary instruction*/
void decode(unsigned instruction){
    /*Init the field of the parsed binary instruction*/
    bininstruction.opcode = (instruction << 25) >> 25; //opcode always the last 7 bits
    bininstruction.rs1 = 0;
    bininstruction.rs2 = 0;
    bininstruction.rd = 0;
    bininstruction.im = 0;
    bininstruction.f3 = 0;
    bininstruction.f7 = 0;
    if (bininstruction.opcode == 51) // R type
    {
        bininstruction.f3 = (instruction << 17) >> 29;
        bininstruction.f7 = (instruction >> 25);
        bininstruction.rs1 = (instruction << 12) >> 27;
        bininstruction.rs2 = (instruction << 7) >> 27; 
        bininstruction.rd = (instruction << 20 ) >> 27;
    }
    else if (bininstruction.opcode == 19) // I type
    {
        bininstruction.rs1 = (instruction << 12) >> 27;
        bininstruction.im = instruction >> 20; //left 12 digit
        bininstruction.im = convert64(bininstruction.im);
        bininstruction.f3 = (instruction << 17) >> 29;
        bininstruction.rd = (instruction << 20 ) >> 27;
    }
    else if(bininstruction.opcode == 3) // I type, ld
    {
        bininstruction.im = instruction >> 20;
        bininstruction.im = convert64(bininstruction.im);
        bininstruction.f3 = (instruction << 17) >> 29;
        bininstruction.rs1 = (instruction << 12) >> 27;
        bininstruction.rd = (instruction << 20 ) >> 27;
    }
    else if (bininstruction.opcode == 103) // I type, jarl
    {
        bininstruction.rs1 = (instruction << 12) >> 27;
        bininstruction.im = instruction >> 20; //left 12 digit
        bininstruction.im = convert64(bininstruction.im);
        bininstruction.f3 = (instruction << 17) >> 29;
        bininstruction.rd = (instruction << 20 ) >> 27;
    }
    else if (bininstruction.opcode == 35) // S type, sd
    {
        bininstruction.im = ((instruction >> 25)<<5) | ((instruction << 20)  >> 27);
        bininstruction.im = convert64(bininstruction.im);
        bininstruction.f3 = (instruction << 17) >> 29;
        bininstruction.rs1 = (instruction << 12) >> 27;
        bininstruction.rs2 = (instruction << 7) >> 27; 
    }
    else if (bininstruction.opcode == 99) //SB type
    {
        //buil im for sb
        unsigned i12, i11, i10_5, i4_1;
        i12 = ((instruction >> 31) <<11);
        i11 =  (((instruction << 24)>>31) << 10);
        i10_5 = (((instruction << 1) >> 26)<<4);
        i4_1 = ((instruction <<20) >> 28);
        bininstruction.im = (i12 | i11 | i10_5 | i4_1);
        bininstruction.im = convert64(bininstruction.im);
        bininstruction.f3 = (instruction << 17) >> 29;
        bininstruction.rs1 = (instruction << 12) >> 27;
        bininstruction.rs2 = (instruction << 7) >> 27; 
    }
    else if (bininstruction.opcode == 111) //UJ type, jal
    {
        //build im for uj type
        unsigned i20, i10_1, i11, i19_12;
        i20 = (instruction >> 31) << 19;
        i10_1 = ((instruction << 1) >> 22);
        i11 = ((instruction << 11) >> 31) << 10;
        i19_12 = ((instruction << 12) >> 24) << 11;
        bininstruction.im = i20 | i19_12 | i11 | i10_1;
        bininstruction.im = convert64(bininstruction.im);
        bininstruction.rd = (instruction << 20) >> 27;
    }
    else
    {
        printf("decode Error: Opcode Unfound!\n");
    }
}
/*Config branch signal bne, beq, blt OR bge*/
bool config_branch(unsigned input1, unsigned input2)
{    
    if (bininstruction.opcode == 99){
        switch(bininstruction.f3){
            
            case 0: //beq
                if (input1== input2)
                    return true;
                else
                {
                    return false;
                }
                
                break;
                
            case 1: //bne
                if (input1 != input2)
                {
                    return true;
                }
                else
                {
                    return false;
                }
                
                break;
                
            case 4: //blt
                if (input1 <= input2)
                {
                    return true;
                }
                else
                {
                    return false;
                }
                
                break;
                
            case 5: //bge
                if (input1 >= input2)
                    return true;
                else
                {
                    return false;
                }
                
                break;
                
            default: 
                return false;
                break;
        }
    }
    if (bininstruction.opcode == 111 || bininstruction.opcode == 103)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*Config signals in system*/
void controlsignal(unsigned opcode){
    /*Init signals as false and 0 so it is reset for each call for each instruction */
    signal.ALUSrc = false;
    signal.MemtoReg = false;
    signal.RegWrite = false;
    signal.MemRead = false;
    signal.MemWrite = false;
    signal.Branch = false;
    signal.ALUOp = 0;
    
    if (opcode == 51) // R type
    {
        signal.RegWrite = true;
        signal.ALUOp = 2;
    }
    else if (opcode == 19) // I type
    {
        signal.ALUSrc = true;
        signal.RegWrite = true;
        signal.ALUOp = 2;
    }
    else if(opcode == 3) // I type, ld
    {
        signal.ALUSrc = true;
        signal.MemtoReg = true;
        signal.RegWrite = true;
        signal.MemRead = true;
        signal.ALUOp = 0;
    }
    else if (opcode == 103) // I type, jarl
    {
        signal.ALUSrc = true;
        signal.RegWrite = true;
        signal.Branch = true;
        signal.ALUOp = 0;
    }
    else if (opcode == 35) // S type, sd
    {
        signal.ALUSrc = true;
        signal.MemWrite = true;
        signal.ALUOp = 0;
    }
    else if (opcode == 99) //SB type, bne, beq, blt, bge
    {
        signal.Branch = true;
        signal.ALUOp = 1;
    }
    else if (opcode == 111) //UJ type, jal
    {
        signal.ALUSrc = true;
        signal.RegWrite = true;
        signal.Branch = true;
        signal.ALUOp = 0;
    }
    else
    {
        printf("Error: Undefined opcode!\n");
    }
}

/*ALU Operations*/
unsigned ALU(int64_t input1, int64_t input2, unsigned ALU_Control)
{   
    unsigned final_output = 0;
    switch(ALU_Control){
            
        case 0: 
            final_output = input1 & input2;
            break;
        case 1: 
            final_output = input1 | input2;
            break;            
        case 2: 
            final_output = input1 + input2;
            break;
        case 3: 
            final_output = input1 << input2;
            break;
        case 4: 
            final_output = input1 >> input2;
            break;
        case 5: 
            final_output = input1 ^ input2;
            break;
        case 6: 
            final_output = input1 - input2;
            break;
        case 7: 
            final_output = (input1 < input2 ? 1 : 0);
            break; 
        default:
            final_output = (input1 < input2 ? 1 : 0);
    }
    
    return final_output; 
}

/*Convert ALUOp to ALUControl on ALU*/
unsigned getALUControl(){ 
        // ld, sd
    if (ID_EX_Register.ALUOp == 0){
        return 2;
    }
    else if (ID_EX_Register.ALUOp == 1){
        // beq, bne
        if (ID_EX_Register.f3 == 0 || ID_EX_Register.f3 == 1)
            return 6;

        // blt, bge
        if (ID_EX_Register.f3 >= 4 && ID_EX_Register.f3 <= 5)
            return 7;
    }
    //  R-type
    else if(ID_EX_Register.ALUOp == 2){
        // add, addi
        if (ID_EX_Register.f3 == 0){
            if (ID_EX_Register.f7 == 0){
                return 2;
            }
        // sub
            else if (ID_EX_Register.f7 == 0b0100000){
                return 6; 
            }
        }
        // sll, slli
        if (ID_EX_Register.f3 == 1){
            return 3;
        }
        // slt
        if (ID_EX_Register.f3 == 2){
            return 7;
        }
        // xor, xori
        if (ID_EX_Register.f3 == 4){
            return 5;
        }
        // srl, srli
        if (ID_EX_Register.f3 == 5){
            return 4;
        }
        // or, ori
        if (ID_EX_Register.f3 == 6){
            return 1;
        }
        // and, andi
        if (ID_EX_Register.f3 == 7){
            return 0;
        }
    }
    else {
        printf("Can't retrieve ALUControl: Unknown ALUOp Signal!\n");
        return -1;
    }
    return -1;
}

/*Muxes*/
uint64_t mux (bool signal, uint64_t input1, uint64_t input2){
    if (signal){
        return input1;
    }
    else
    {
        return input2;
    }
}


// FIXME, implement this function
bool tickFunc(Core *core)
{
    //IF Stage**************************
    unsigned instruction = 0;
    instruction = core->instr_mem->instructions[core->PC / 4].instruction;
    IF_Stage.PC = core->PC;
    IF_ID_Register.PC = IF_Stage.PC;
    IF_ID_Register.IFID_register = instruction;

    //ID Stage************************
        /*Separate the binary instruction received into opcode, rd, rs and immediate*/
    ID_Stage.PC = IF_ID_Register.PC;
    ID_Stage.fetched_instruction = IF_ID_Register.IFID_register;
    decode(ID_Stage.fetched_instruction); //decode and store info into bininstruction structure
    /*Save to IDEX Register */
    ID_EX_Register.PC = ID_Stage.PC;
    ID_EX_Register.f3 = bininstruction.f3;
    ID_EX_Register.f7 = bininstruction.f7;
    ID_EX_Register.opcode = bininstruction.opcode;
    ID_EX_Register.im = bininstruction.im;
    ID_EX_Register.rd = bininstruction.rd;
    ID_EX_Register.rs1 = bininstruction.rs1;
    ID_EX_Register.rs2 = bininstruction.rs2;

    /*Get the control signals. Store info into signal structure*/
    controlsignal(ID_EX_Register.opcode);
    /*Save signal info to IDEX stage register*/
    ID_EX_Register.MemRead = signal.MemRead;
    ID_EX_Register.MemtoReg = signal.MemtoReg;
    ID_EX_Register.MemWrite = signal.MemWrite;
    ID_EX_Register.RegWrite = signal.RegWrite;
    ID_EX_Register.ALUOp = signal.ALUOp;
    ID_EX_Register.ALUSrc = signal.ALUSrc;
    ID_EX_Register.Branch = signal.Branch;
    /*Read from register memory*/
    ID_EX_Register.read_dat1 = core->register_file[ID_EX_Register.rs1];
    ID_EX_Register.read_dat2 = core->register_file[ID_EX_Register.rs2];
    
    /*EX Stage**************************/
    /*Passing on instruction and signal infor to EX Stage*/
    EX_Stage.PC = ID_EX_Register.PC;
    EX_Stage.rs1 = ID_EX_Register.rs1;
    EX_Stage.rs2 = ID_EX_Register.rs2;
    EX_Stage.rd = ID_EX_Register.rd;
    EX_Stage.im = ID_EX_Register.im;
    EX_Stage.opcode = ID_EX_Register.opcode;
    EX_Stage.f3 = ID_EX_Register.f3;
    EX_Stage.f7 = ID_EX_Register.f7;
    EX_Stage.MemRead = ID_EX_Register.MemRead;
    EX_Stage.MemtoReg = ID_EX_Register.MemtoReg;
    EX_Stage.MemWrite = ID_EX_Register.MemWrite;
    EX_Stage.RegWrite = ID_EX_Register.RegWrite;
    EX_Stage.ALUSrc = ID_EX_Register.ALUSrc;
    EX_Stage.ALUOp = ID_EX_Register.ALUOp;
    EX_Stage.Branch = ID_EX_Register.Branch;
    
    EX_Stage.alucontrol_signal = getALUControl(); //Get data from IDEX Register and find alucontrol_signal
    EX_Stage.read_dat1 = ID_EX_Register.read_dat1;
    EX_Stage.read_dat2 = ID_EX_Register.read_dat2;
    EX_Stage.input2 = mux(EX_Stage.ALUSrc, EX_Stage.im, EX_Stage.read_dat2); //imm or rs2, input go to ALU

    /*ALU Calculation. Store result to EXMEM Register*/
    EX_MEM_Register.result = ALU(EX_Stage.read_dat1, EX_Stage.input2, EX_Stage.alucontrol_signal);
    EX_MEM_Register.ALUOp = EX_Stage.ALUOp;
    EX_MEM_Register.ALUSrc = EX_Stage.ALUSrc;
    EX_MEM_Register.Branch = EX_Stage.Branch;
    EX_MEM_Register.MemRead = EX_Stage.MemRead;
    EX_MEM_Register.MemtoReg = EX_Stage.MemtoReg;
    EX_MEM_Register.MemWrite = EX_Stage.MemWrite;
    EX_MEM_Register.RegWrite = EX_Stage.RegWrite;
    EX_MEM_Register.PC = EX_Stage.PC;
    EX_MEM_Register.rd = EX_Stage.rd; 
    EX_MEM_Register.im = EX_Stage.im;
    EX_MEM_Register.opcode = EX_Stage.opcode;
    EX_MEM_Register.input2 = EX_Stage.input2; //input go into ALU rs2 or imm
    EX_MEM_Register.read_dat1 = EX_Stage.read_dat1;
    EX_MEM_Register.read_dat2 = EX_Stage.read_dat2;

    /*MEM Stage*********************/
    // (Step 3) Signal Handler for Memory and register file 
    MEM_Stage.PC = EX_MEM_Register.PC;
    MEM_Stage.r_dat = 0;
    MEM_Stage.w_dat = 0;
    MEM_Stage.rd = EX_MEM_Register.rd;
    MEM_Stage.opcode = EX_MEM_Register.opcode;
    MEM_Stage.im = EX_MEM_Register.im;
    MEM_Stage.result = EX_MEM_Register.result;
    MEM_Stage.ALUOp = EX_MEM_Register.ALUOp;
    MEM_Stage.ALUSrc = EX_MEM_Register.ALUSrc;
    MEM_Stage.Branch = EX_MEM_Register.Branch;
    MEM_Stage.Branch = config_branch(EX_MEM_Register.read_dat1, EX_MEM_Register.input2); //update signal.Branch
    MEM_Stage.MemRead = EX_MEM_Register.MemRead;
    MEM_Stage.MemtoReg = EX_MEM_Register.MemtoReg;
    MEM_Stage.MemWrite = EX_MEM_Register.MemWrite;
    MEM_Stage.RegWrite = EX_MEM_Register.RegWrite;
    /*Handle data memory*/
    if(MEM_Stage.MemWrite) //Only sd has MemWrite
    {
        MEM_Stage.r_dat = EX_MEM_Register.read_dat2 ; //core->register_file[bininstruction.rs2];
        for(int i = 0; i < 64; i+=8)
            core->data_memory[MEM_Stage.result+i/8] = MEM_Stage.r_dat << i;
    }

    if(MEM_Stage.MemRead)  
    {
        for(int i = 0; i < 64; i+=8)
            MEM_Stage.r_dat |= core->data_memory[MEM_Stage.result+i/8] << i;
    }
    // Config PC 
    MEM_Stage.new_PC = MEM_Stage.PC /*core->PC*/ + MEM_Stage.im; //True for all cases except jalr
    if(MEM_Stage.opcode == 103 /*jalr*/)
    {
        MEM_Stage.new_PC = MEM_Stage.result; //RS1+imm
    }
    
    if(MEM_Stage.Branch)/*bne, beq, blt, bge, jal, jalr*/
    {
        core->PC = MEM_Stage.new_PC;
    }
    else
    {
        core->PC += 4;
    }
    /*Store data to MEMWB stage register*/
    MEM_WB_Register.r_dat = MEM_Stage.r_dat;
    MEM_WB_Register.w_dat = MEM_Stage.w_dat;
    MEM_WB_Register.MemtoReg = MEM_Stage.MemtoReg;
    MEM_WB_Register.RegWrite = MEM_Stage.RegWrite;
    MEM_WB_Register.opcode = MEM_Stage.opcode;
    MEM_WB_Register.rd = MEM_Stage.rd;
    MEM_WB_Register.result = MEM_Stage.result;


    /*WB Stage *****************/
    //REad data from MEMWB stage register
    WB_Stage.r_dat = MEM_WB_Register.r_dat;
    WB_Stage.w_dat = MEM_WB_Register.w_dat;
    WB_Stage.result = MEM_WB_Register.result;
    WB_Stage.rd = MEM_WB_Register.rd;
    WB_Stage.opcode = MEM_WB_Register.opcode;
    WB_Stage.MemtoReg = MEM_WB_Register.MemtoReg;
    WB_Stage.RegWrite = MEM_WB_Register.RegWrite;

    if(WB_Stage.MemtoReg)
    {
        WB_Stage.w_dat = MEM_WB_Register.r_dat;
    }
    else if(WB_Stage.opcode == 103 /*jal*/ || WB_Stage.opcode == 111 /*jalr*/)
    {
        WB_Stage.w_dat = core->PC + 4;
    }
    else
    {
        WB_Stage.w_dat = WB_Stage.result;
    }

    if(WB_Stage.rd != 0 && WB_Stage.RegWrite)
    {
        core->register_file[WB_Stage.rd] = WB_Stage.w_dat;
    }
    

    // (Step 5) Output
    /*Output Format*/
    printf("\nInstruction (Hex): %x\n", instruction);
    printf("Op: %u\t rd: %u\t rs1: %u\t rs2: %u \timm: %ld \tresult: %d\n", bininstruction.opcode, bininstruction.rd, bininstruction.rs1, bininstruction.rs2, bininstruction.im, WB_Stage.result);
    printf("REGISTER VALUE (NON-ZERO): \n");
    for(int i = 0; i < 32 ; i++) //# of regs is 32
    {
        if (core->register_file[i] != 0) //Print out only registers with nonzero value
        {
            printf("%s: %ld\n", REGISTER_NAME[i], core->register_file[i]);
        }
    }

    printf("DATA MEMORY: \n");
    printf("Address  |   Data\n");
    for(int i = 0; i < 256; i += 8) //#of byte in mem is 256 //8
    {
	    int dat = 0;
        int j = 0;
	    while(j < 7)
	    {
	        dat |= (core->data_memory[i+j] << (j * 8));
            j++;
	    }
        
	    printf("%4d     |   %d\n", i, dat);
    } 
    

    
    // (Step 6)
    ++core->clk;
    /* Are we reaching the final instruction?*/
    if (core->PC > core->instr_mem->last->addr)
    {
        return false;
    }
    return true;
}

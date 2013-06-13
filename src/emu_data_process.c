#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "emulate.h"
#include "emu_data_process.h"
#include "library.h"

enum opcode_type   {AND,
                    EOR,
                    SUB,
                    RSB,
                    ADD,
                    TST,
                    TEQ,
                    CMP,
                    ORR,
                    MOV};

static enum opcode_type get_opcode(uint32_t instruction) {

    uint32_t op_number= mask4 & (instruction >> 21);

    enum opcode_type op; 

    switch(op_number){
        case(0):  op = AND; break;
        case(1):  op = EOR; break;    
        case(2):  op = SUB; break;
        case(3):  op = RSB; break;
        case(4):  op = ADD; break;
        case(8):  op = TST; break;
        case(9):  op = TEQ; break;
        case(10): op = CMP; break;
        case(12): op = ORR; break;
        case(13): op = MOV; break;
    }
   return op;
}

static int32_t get_operand_1(uint32_t instruction) {
    //Operand1 is always in Rn 19-16

    uint32_t reg_no = ((instruction >> 16) & mask4);
    return registers[reg_no]; 
}

static int32_t get_operand_2(uint32_t instruction) {
    //Operand2 is either immediate (in which case we just take the value and extend)
    //Or it is in a register in which case we have to apply shift operations

    //Pushes these sections to the first bits of a 32-bit int
    
    int32_t result;

    uint32_t I = mask1 & (instruction >> 25); //gets the value of I alone

    if ( I == 1 ) {

        //Operand2 immediate

        uint32_t imm = mask8 & instruction;
        uint32_t rotate = (mask4 & (instruction >>  8)) << 1; 
        // shifted the instruction by 8 to get the rotate, applied the mask, then x2.
        result = rotate_right(rotate, imm);

    } else {

        //Operand2 register
        uint32_t shift = mask8 & (instruction >> 4); //shift instruction(11-4)
        uint32_t shiftType = mask2 & (shift >> 1); //shift type shift(6 -5)

        uint32_t scale = mask5 & (shift >> 3); //integer shift(11-7)
        if (shift & mask1) {
            scale = registers[mask4 & (shift >> 4)];
        }
        uint32_t rm = mask4 & instruction; //rm instruction(3 -0)
        
        uint32_t rm_value = registers[rm];
        
        switch(shiftType) {
            case(0): 
                result = log_shift_left(rm_value, scale);
                break;
            case(1):
                result = log_shift_right(rm_value, scale);
                break;
            case(2):
                result = arith_shift_right(rm_value, scale);
                break;
            case(3):
                result = rotate_right(rm_value, scale);
                break;
        }
    }
    return result;
}

static void set_flags(int32_t result, uint32_t S) {
    //update flags
   
   if(S == 1) {

    uint32_t cspr = registers[16];

    // Set N if result is negative
    uint32_t N = result & (1 << 31); 
    // Set Z if result is zero
    uint32_t Z = (result == 0) << 30 ;
    // Set carry out
    uint32_t C = carryout << 29 ;

    uint32_t cspr_mask = generate_mask(28);
    cspr &= cspr_mask;
    cspr |= N | Z | C;
    registers[16] = cspr;
   }
}

void data_process(uint32_t instruction) {

    enum opcode_type op = get_opcode(instruction);

    int32_t operand1 = get_operand_1(instruction);
    int32_t operand2 = get_operand_2(instruction);
    uint32_t destination_register = (mask4 & (instruction >> 12));
    uint32_t S = (instruction >> 20) & mask1;
    int32_t result = registers[destination_register];
    int32_t temp_result;

    switch(op){

        case(AND): result = (operand1 & operand2); set_flags(result, S); break;
        case(EOR): result = (operand1 ^ operand2); set_flags(result, S); break;
        case(SUB): result = (operand1 - operand2); set_flags(result, S); break;
        case(RSB): result = (operand2 - operand1); set_flags(result, S); break;
        case(ADD): result = (operand1 + operand2); set_flags(result, S); break;

        case(TST): temp_result = operand1 & operand2; set_flags(temp_result, S); break; 
        case(TEQ): temp_result = operand1 ^ operand2; set_flags(temp_result, S); break; 
        case(CMP): temp_result = operand1 - operand2; set_flags(temp_result, S); break; 

        case(ORR): result = (operand1 | operand2); set_flags(result, S); break;
        case(MOV): result = operand2; set_flags(result, S); break;
    }
    registers[destination_register] = result;
}
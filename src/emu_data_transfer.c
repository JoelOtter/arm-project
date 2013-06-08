#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "emu_data_transfer.h"
#include "emulate.h"
#include "library.h"


static uint32_t get_operand_2(uint32_t instruction) {
    //Operand2 is either immediate (in which case we just take the value and extend)
    //Or it is in a register in which case we have to apply shift operations

    //Pushes these sections to the first bits of a 32-bit int
    
    uint32_t result;

    uint32_t I = mask1 & (instruction >> 25); //gets the value of I alone

    if ( I != 1 ) {

        //Operand2 immediate

        uint32_t imm = mask8 & instruction;
        uint32_t rotate = (mask4 & (instruction >>  8)) << 1; 
        // shifted the instruction by 8 to get the rotate, applied the mask, then x2.
        result = rotate_right(rotate, imm);

    } else {

        //Operand2 register

        uint32_t shift = mask8 & (instruction >> 4);    // shift   instruction(11-4)
        uint32_t shift_type = mask2 & (shift >> 1); //  - shift type shift(6 -5)
        uint32_t scale   = mask5 & (shift >> 3); //  - integer    shift(11-7)
        uint32_t rm    = mask4 & instruction;      // rm      instruction(3 -0)
        uint32_t rm_value = registers[rm];
        switch(shift_type) {
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

static void load(uint32_t P, uint32_t U, uint32_t offset, uint32_t base_reg, uint32_t dest_reg){
    int32_t offset_new = offset;
    if (!U) offset_new = offset_new * (-1);
    uint32_t reg_value = registers[base_reg];
    if (P) reg_value += offset_new;
    //printf("0x%08x\n", reg_value);
    if (reg_value + 3 < SIZE_OF_MEMORY) registers[dest_reg] = get_from_memory(memory, reg_value);
    else if (reg_value == 0x20200000) {
        printf("One GPIO pin from 0 to 9 has been accessed\n");
        registers[dest_reg] = reg_value;
    }
    else if (reg_value == 0x20200004) {
        printf("One GPIO pin from 10 to 19 has been accessed\n"); 
        registers[dest_reg] = reg_value;
    }
    else if (reg_value == 0x20200008) {
        printf("One GPIO pin from 20 to 29 has been accessed\n");
        registers[dest_reg] = reg_value;
    }
    else printf("Error: Out of bounds memory access at address 0x%08x\n", reg_value);
    if (!P) registers[base_reg] += offset_new;
}

static void store(uint32_t P, uint32_t U, uint32_t offset, uint32_t base_reg, uint32_t dest_reg){
    int32_t offset_new = offset;
    if (!U) offset_new = offset_new * (-1);
    uint32_t reg_value = registers[base_reg];
    if (P) reg_value += offset_new;
    if (reg_value + 3 < SIZE_OF_MEMORY) write_to_memory(memory, reg_value, registers[dest_reg]);
    else if (reg_value == 0x20200000) printf("One GPIO pin from 0 to 9 has been accessed\n");
    else if (reg_value == 0x20200004) printf("One GPIO pin from 10 to 19 has been accessed\n");
    else if (reg_value == 0x20200008) printf("One GPIO pin from 20 to 29 has been accessed\n");
    else if (reg_value == 0x20200028) printf("LED OFF!\n");
    else if (reg_value == 0x2020001c) printf("LED ON!\n");
    else printf("Error: Out of bounds memory access at address 0x%08x\n", reg_value);
    if (!P) registers[base_reg] += offset_new;
}

void single_data_transfer(uint32_t instruction){
    uint32_t P = ((instruction >> 24) & mask1);
    uint32_t U = ((instruction >> 23) & mask1);
    uint32_t L = ((instruction >> 20) & mask1);
    uint32_t base_reg = ((instruction >> 16) & mask4);
    uint32_t dest_reg = ((instruction >> 12) & mask4);
    uint32_t offset = get_operand_2(instruction);
    
    if (L) load(P, U, offset, base_reg, dest_reg);
    else store(P, U, offset, base_reg, dest_reg);
}


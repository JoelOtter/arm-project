#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "library.h"
#include "emulate.h"

enum opcodeType    {AND,
                    EOR,
                    SUB,
                    RSB,
                    ADD,
                    TST,
                    TEQ,
                    CMP,
                    ORR,
                    MOV};



static void set_flags_multiply(uint32_t result, uint32_t S) {
    //update flags
   
   if(S) {

    uint32_t cpsr = registers[16];

    // Set N if result is negative
    uint32_t N = result & (1 << 31); 
    // Set Z if result is zero
    uint32_t Z = (result == 0) << 30 ;
    //generate mask of 30 since V and C are unchanged
    uint32_t cpsr_mask = generate_mask(30);
    cpsr &= cpsr_mask;

    cpsr |= N | Z;

    registers[16] = cpsr;
   }
}

void multiply(uint32_t instruction) {

    uint32_t Rd = (instruction >> 16) & mask4;
    uint32_t Rn = (instruction >> 12) & mask4;
    uint32_t Rs = (instruction >> 8) & mask4;
    uint32_t Rm = instruction & mask4;
    uint32_t S = (instruction >> 20) & mask1;

    uint32_t temp = registers[Rm] * registers[Rs];

    // if accumulate bit is set, then also add value of Rn
    if ((instruction >> 21) & mask1) {
        temp += registers[Rn];
    } 
    registers[Rd] = temp;

    set_flags_multiply(temp, S);

}
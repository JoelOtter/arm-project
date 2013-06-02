#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "emulate.h"
#include "data_Process.h"
#include "library.h"

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

static enum opcodeType getOpcode(uint32_t instruction) {

    uint32_t opNumber= mask4 & (instruction >> 21);

    enum opcodeType op; 

    switch(opNumber){
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


static int32_t getOperand1(uint32_t instruction) {
    //Operand1 is always in Rn 19-16

    uint32_t regNo = ((instruction >> 16) & mask4);
    return registers[regNo]; 
}

static int32_t getOperand2(uint32_t instruction) {
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
        result = rotateRight(rotate, imm);

    } else {

        //Operand2 register

        uint32_t shift = mask8 & (instruction >> 4);    // shift   instruction(11-4)
        uint32_t shiftType = mask2 & (shift >> 1); //  - shift type shift(6 -5)
        uint32_t scale   = mask5 & (shift >> 3); //  - integer    shift(11-7)
        uint32_t rm    = mask4 & instruction;      // rm      instruction(3 -0)
        
        uint32_t rmValue = registers[rm];
        int32_t temp;
        switch(shiftType) {
            case(0): 
                result = logShiftLeft(rmValue, scale);
                break;
            case(1):
                result = logShiftRight(rmValue, scale);
                break;
            case(2):
                result = arithShiftRight(rmValue, scale);
                break;
            case(3):
                result = rotateRight(rmValue, scale);
                break;
        }

    }

    return result;

}


static void setFlags(int32_t result, uint32_t S) {
    //update flags
   
   if(S == 1) {

    uint32_t cspr = registers[16];

    // Set N if result is negative
    uint32_t N = result & (1 << 31); 
    // Set Z if result is zero
    uint32_t Z = (result == 0) << 30 ;
    printf("(Result == 0) = %d\n", (result == 0));
    // Set carry out
    uint32_t C = carryout << 29 ;

    uint32_t csprMask = generateMask(28);
    cspr &= csprMask;

    printf("N    = "); printBits(N);
    printf("Z    = "); printBits(Z);
    printf("C    = "); printBits(C);
    printf("cpsr = "); printBits(cspr);
    cspr |= N | Z | C;
    printf("cpsr = "); printBits(cspr);

    registers[16] = cspr;
   }



}



void data_Process(uint32_t instruction) {

    enum opcodeType op = getOpcode(instruction);

    int32_t operand1 = getOperand1(instruction);
    int32_t operand2 = getOperand2(instruction);
    uint32_t destinationRegister = (mask4 & (instruction >> 12));
    uint32_t S = (instruction >> 20) & mask1;
    int32_t result = registers[destinationRegister];
    int32_t tempResult;

    printf("S = %d\n", S);
    switch(op){

        case(AND): result = (operand1 && operand2); setFlags(result, S); break;
        case(EOR): result = (operand1 ^ operand2) ; setFlags(result, S); break;
        case(SUB): result = (operand1 - operand2) ; setFlags(result, S); break;
        case(RSB): result = (operand2 - operand1) ; setFlags(result, S); break;
        case(ADD): result = (operand1 + operand2) ; setFlags(result, S); break;

        case(TST): tempResult = operand1 && operand2 ; setFlags(tempResult, S); break; 
        case(TEQ): tempResult = operand1 ^ operand2  ; setFlags(tempResult, S); break; 
        case(CMP): tempResult = operand1 - operand2  ; setFlags(tempResult, S); break; 

        case(ORR): result = (operand1 || operand2); setFlags(result, S); break;
        case(MOV): result = operand2              ; setFlags(result, S); break;
    }

    registers[destinationRegister] = result;



}
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

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

uint32_t registers[20]; // just put it here so it compiles but 
                       // can change it later 

// All the masks are up here
const uint32_t mask1 =   1; // 0000 0001
const uint32_t mask2 =   3; // 0000 0011
const uint32_t mask4 =  15; // 0000 1111 
const uint32_t mask8 = 255; // 1111 1111 

int carryout;




int checkCondition(uint32_t instruction) {

    //CSPR register values
    uint32_t cspr = registers[16];
    uint32_t N = mask1 & (cspr >> 31);
    uint32_t Z = mask1 & (cspr >> 30);
    uint32_t V = mask1 & (cspr >> 28);

    uint32_t condNumber = mask4 & (instruction >> 28);
    int value = 0;

    switch(condNumber) {

        case(0):
            // eq
            value = Z;
            break;
        case(1):
            // ne
            value = Z;
            break;
        case(10):
            // ge
            value = (N == V);
            break;
        case(11): 
            // lt
            value = (N != V);
            break;
        case(12):
            // gt
            value = ((Z == 0) && (N == V));
            break;
        case(13):
            //le
            value = ((Z == 1) && (N != V));
            break;
        case(14):
            // al (always 1)
            value = 1;
            break;
    }
    return value;
}







enum opcodeType getOpcode(uint32_t instruction) {

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




int32_t getOperand1(uint32_t instruction) {
    //Operand1 is always in Rn 19-16

    uint32_t regNo = ((instruction >> 12) & mask4);
    return registers[regNo]; 
}




int32_t getOperand2(uint32_t instruction) {
    //Operand2 is either immediate (in which case we just take the value and extend)
    //Or it is in a register in which case we have to apply shift operations

    //Pushes these sections to the first bits of a 32-bit int
    uint32_t shiftForShift  = instruction >>  4; // shifter for Shift


    int32_t value;

    uint32_t I = mask1 & (instruction >> 25); //gets the value of I alone

    if ( I == 1 ) {

        //Operand2 immediate

        uint32_t imm = mask8 & instruction;
        uint32_t rotate = (mask4 & (instruction >>  8)) << 1; 
        // shifted the instruction by 8 to get the rotate, applied the mask, then x2.
        value = imm >> rotate;

    } else {

        //Operand2 register

        uint32_t shift = mask8 & shiftForShift;    // shift   instruction(11-4)
        uint32_t shiftType = mask2 & (shift >> 1); //  - shift type shift(6 -5)
        uint32_t integer   = mask4 & (shift >> 4); //  - integer    shift(11-7)
        uint32_t rm    = mask4 & instruction;      // rm      instruction(3 -0)
        
        value = registers[rm];
        int32_t temp;
        switch(shiftType) {
            case(0): 
                carryout = (value >> (32 - integer)) & mask1;
                value = value << integer;
                break;
            case(1):
                carryout = (value >> (integer - 1)) & mask1;
                value = value >> integer;
                break;
            case(2):
                carryout = (value >> (integer - 1)) & mask1;
                //TODO
                value = value >> integer;
                break;
            case(3):
                // rotate right, so 
                temp = ((mask1 & value) << 31); // gets the 0th bit, then pushes it to 31st
                value = temp & ( value >> 1 ); // shifts the whole thing by 1 right, then adds new 31st bit
                break;
        }

    }

    return value;

}


void setFlags(int32_t value) {
    //update flags
   
    uint32_t cspr = registers[16];

    uint32_t N = (( value >> 31 ) & mask1) << 31 ;
    uint32_t Z = (( value == 0 )  & mask1) << 30 ;
    uint32_t C = ( carryout       & mask1) << 29 ;

    cspr = cspr & N & Z & C;

    registers[16] = cspr;

}



void executeInstruction(uint32_t instruction) {

    enum opcodeType op = getOpcode(instruction);

    int32_t operand1 = getOperand1(instruction);
    int32_t operand2 = getOperand2(instruction);
    uint32_t destinationRegister = (mask4 & (instruction >> 16));
    uint32_t S;
    int32_t value;

    switch(op){
        case(AND): value = (operand1 && operand2); break;
        case(EOR): value = (operand1 ^ operand2) ; break;
        case(SUB): value = (operand1 - operand2) ; break;
        case(RSB): value = (operand2 - operand1) ; break;
        case(ADD): value = (operand1 + operand2) ; break;
        case(TST): operand1 && operand2          ; break; //TODO
        case(TEQ): operand1 ^ operand2           ; break; //TODO
        case(CMP): operand1 - operand2           ; break; //TODO
        case(ORR): value = (operand1 || operand2); break;
        case(MOV): value = operand2              ; break;
    }

    S = ( value >> 20 ) & mask1;
    if ( S == 1 ) {
        setFlags(value);
    }

    registers[destinationRegister] = value;

}




int main(int argc, char const *argv[]) {

    uint32_t instruction = atoi(argv[1]);

    if ( checkCondition(instruction) == 1 ) {
        executeInstruction(instruction);
    } else { 
        //ignore instruction? not sure what to put here //TODO
    }

    return 0;
}







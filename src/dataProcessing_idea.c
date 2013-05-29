#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

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
const uint32_t mask5 =  31; // 0001 1111
const uint32_t mask8 = 255; // 1111 1111 

int carryout;

int checkCondition(uint32_t instruction) {

    //CSPR register values
    uint32_t cspr = registers[16];
    uint32_t N = mask1 & (cspr >> 31);
    uint32_t Z = mask1 & (cspr >> 30);
    uint32_t V = mask1 & (cspr >> 28);

    uint32_t condNumber = mask4 & (instruction >> 28);
    int condflag = 0;

    switch(condNumber) {

        case(0):
            // eq
            condflag = Z;
            break;
        case(1):
            // ne
            condflag = Z;
            break;
        case(10):
            // ge
            condflag = (N == V);
            break;
        case(11): 
            // lt
            condflag = (N != V);
            break;
        case(12):
            // gt
            condflag = ((Z == 0) && (N == V));
            break;
        case(13):
            //le
            condflag = ((Z == 1) || (N != V));
            break;
        case(14):
            // al (always 1)
            condflag = 1;
            break;
    }
    return condflag;
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

    uint32_t regNo = ((instruction >> 16) & mask4);
    return registers[regNo]; 
}


uint32_t logShiftLeft(uint32_t value, uint32_t scale) {

    carryout = (value >> (32 - scale)) & mask1;
    value <<= scale;
    return value;

}

uint32_t logShiftRight(uint32_t value, uint32_t scale) {
    
    carryout = 0;
    if (scale > 0) {
    carryout = (value >> (scale - 1)) & mask1;
    }
    value >>= scale;
    return value;

}

uint32_t arithShiftRight(uint32_t value, uint32_t scale) {

    uint32_t sign = value >> 31;

    value = logShiftRight(value, scale);
    
    if (sign == 1) {
        uint32_t mask = 0;
        for (uint32_t i = 0; i < scale; ++i)
        {
          mask <<= 1;
          mask += 1;
        }  
        mask <<= 32-scale;
        value |= mask;
    }
    return value;

}

uint32_t rotateRight(uint8_t rotate, uint32_t value){

    // rotate has to be multiple of 2 and between 0 and 30
    assert(rotate <= 30 && rotate % 2 == 0);

    // Creates a mask of 1's that's as long as the bits that need to be copied
    uint32_t mask = 0;
    for (uint32_t i = 0; i < rotate; ++i)
    {
      mask <<= 1;
      mask += 1;
    }  
    // Cuts out the bits that will be lost in the shift
    uint32_t cut_bits = value & mask; 
    // shifts the remaining bits the appropriate amount
    uint32_t remaining_bits = value >> rotate;
     // positions the cut bits correctly
    cut_bits <<= (32 - rotate);
    
    uint32_t result = (cut_bits | remaining_bits);
    
    carryout = result >> 31;

    return (result);    
}

int32_t getOperand2(uint32_t instruction) {
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
    uint32_t destinationRegister = (mask4 & (instruction >> 12));
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






